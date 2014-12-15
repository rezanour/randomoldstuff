#include <Precomp.h>
#include "GraphicsSystemImpl.h"
#include "GraphicsSceneImpl.h"
#include "TextureImpl.h"
#include "TransparentPass.h"

_Use_decl_annotations_
std::shared_ptr<BuildTransparentListPass> BuildTransparentListPass::Create(const std::shared_ptr<GraphicsSystem>& graphics, const ComPtr<ID3D11DeviceContext>& context, const std::shared_ptr<Texture>& target, const std::shared_ptr<Texture>& depth)
{
    UNREFERENCED_PARAMETER(target);

    std::shared_ptr<BuildTransparentListPass> pass(new BuildTransparentListPass(graphics, context));
    pass->_debugRT = target->GetRenderTargetView();
    pass->_depth = depth->GetDepthStencilView();
    return pass;
}

_Use_decl_annotations_
BuildTransparentListPass::BuildTransparentListPass(const std::shared_ptr<GraphicsSystem>& graphics, const ComPtr<ID3D11DeviceContext>& context) :
    RenderPass(graphics, context)
{
    LoadShaders(L"vsTransparent.cso", VertexFormat::Position3DColor, L"psBuildTransparentList.cso");

    auto& device = GetDevice();

    CreateVSConstantBuffer(0, sizeof(vsPerFrame));
    CreateVSConstantBuffer(1, sizeof(vsPerDraw));
    CreatePSConstantBuffer(0, sizeof(psPerFrame));

    D3D11_BUFFER_DESC bd = {};
    bd.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
    bd.StructureByteStride = 12;
    bd.ByteWidth = bd.StructureByteStride * (10 * 1920 * 1080);
    bd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    bd.Usage = D3D11_USAGE_DEFAULT;

    if (FAILED(device->CreateBuffer(&bd, nullptr, &_nodesBuffer)))
    {
        throw std::exception();
    }

    bd.StructureByteStride = 4;
    bd.ByteWidth = bd.StructureByteStride * (1920 * 1080);
    bd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;

    std::unique_ptr<uint32_t[]> emptyNodeIndices(new uint32_t[1920 * 1080]);
    memset(emptyNodeIndices.get(), 0xFFFFFFFF, bd.ByteWidth);
    D3D11_SUBRESOURCE_DATA init = {};
    init.pSysMem = emptyNodeIndices.get();
    init.SysMemPitch = bd.ByteWidth;

    if (FAILED(device->CreateBuffer(&bd, &init, &_startOffsetBuffer)))
    {
        throw std::exception();
    }

    SetDepthState(graphics->GetDepthReadNoWriteState());

    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.StructureByteStride = sizeof(Position3DColorVertex);
    bd.ByteWidth = bd.StructureByteStride * MaxDebugVertices;
    bd.MiscFlags = 0;

    if (FAILED(device->CreateBuffer(&bd, nullptr, &_vertexBuffer)))
    {
        throw std::exception();
    }

    D3D11_UNORDERED_ACCESS_VIEW_DESC uavd = {};
    uavd.Format = DXGI_FORMAT_UNKNOWN;
    uavd.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    uavd.Buffer.FirstElement = 0;
    uavd.Buffer.NumElements = (10 * 1920 * 1080);
    uavd.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;

    HRESULT hr = device->CreateUnorderedAccessView(_nodesBuffer.Get(), &uavd, &_nodesUAV);
    if (FAILED(hr))
    {
        throw std::exception();
    }

    uavd.Format = DXGI_FORMAT_R32_TYPELESS;
    uavd.Buffer.FirstElement = 0;
    uavd.Buffer.NumElements = 1920 * 1080;
    uavd.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
    hr = device->CreateUnorderedAccessView(_startOffsetBuffer.Get(), &uavd, &_startOffsetUAV);
    if (FAILED(hr))
    {
        throw std::exception();
    }

    _psPerFrame.ScreenWidth = graphics->GetConfig().Width;
    UpdatePSConstantBuffer(0, &_psPerFrame, sizeof(_psPerFrame));
}

_Use_decl_annotations_
void BuildTransparentListPass::HandleDrawScene(const std::shared_ptr<IGraphicsScene>& scene, const XMFLOAT4X4& view, const XMFLOAT4X4& projection)
{
    UNREFERENCED_PARAMETER(scene);
    UNREFERENCED_PARAMETER(view);
    UNREFERENCED_PARAMETER(projection);

    auto& context = GetContext();
    ID3D11UnorderedAccessView* uavs[] = { _nodesUAV.Get(), _startOffsetUAV.Get() };
    UINT initialCount = 0;
    context->OMSetRenderTargetsAndUnorderedAccessViews(1, _debugRT.GetAddressOf(), _depth.Get(), 1, _countof(uavs), uavs, &initialCount);

    static const UINT clearOffsets[] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF };
    context->ClearUnorderedAccessViewUint(_startOffsetUAV.Get(), clearOffsets);

    XMStoreFloat4x4(&_vsPerFrame.ViewProjection, XMMatrixMultiply(XMLoadFloat4x4(&view), XMLoadFloat4x4(&projection)));
    UpdateVSConstantBuffer(0, &_vsPerFrame, sizeof(_vsPerFrame));

    XMStoreFloat4x4(&_vsPerDraw.World, XMMatrixIdentity());
    UpdateVSConstantBuffer(1, &_vsPerDraw, sizeof(_vsPerDraw));

    auto theScene = static_cast<GraphicsScene*>(scene.get());

    uint32_t stride = GetVertexSize(VertexFormat::Position3DColor);
    uint32_t offset = 0;
    context->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);

    theScene->GetDebugTriangles(_debugTriangles);
    if (_debugTriangles.size() > 0)
    {
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        uint32_t numVertices = min(static_cast<uint32_t>(_debugTriangles.size() * 3), MaxDebugVertices);

        D3D11_BOX box = {};
        box.right = numVertices * stride;
        box.bottom = 1;
        box.back = 1;
        context->UpdateSubresource(_vertexBuffer.Get(), 0, &box, _debugTriangles.data(), box.right, 0);

        context->Draw(numVertices, 0);
        _debugLines.clear();
    }

    theScene->GetDebugLines(_debugLines);
    if (_debugLines.size() > 0)
    {
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

        uint32_t numVertices = min(static_cast<uint32_t>(_debugLines.size() * 2), MaxDebugVertices);

        D3D11_BOX box = {};
        box.right = numVertices * stride;
        box.bottom = 1;
        box.back = 1;
        context->UpdateSubresource(_vertexBuffer.Get(), 0, &box, _debugLines.data(), box.right, 0);

        context->Draw(numVertices, 0);
        _debugLines.clear();
    }

    theScene->ClearDebugContent();
}

_Use_decl_annotations_
std::shared_ptr<RenderTransparentPass> RenderTransparentPass::Create(const std::shared_ptr<GraphicsSystem>& graphics, const ComPtr<ID3D11DeviceContext>& context, const std::shared_ptr<Texture>& target, const std::shared_ptr<Texture>& depth, const std::shared_ptr<Texture>& background, const std::shared_ptr<BuildTransparentListPass>& buildPass)
{
    UNREFERENCED_PARAMETER(depth);

    std::shared_ptr<RenderTransparentPass> pass(new RenderTransparentPass(graphics, context));
    pass->SetRenderTarget(0, target);

    auto& device = pass->GetDevice();

    D3D11_SHADER_RESOURCE_VIEW_DESC srvd = {};
    srvd.Format = DXGI_FORMAT_UNKNOWN;
    srvd.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    srvd.Buffer.ElementOffset = 0;
    srvd.Buffer.NumElements = (5 * 1920 * 1080);

    HRESULT hr = device->CreateShaderResourceView(buildPass->GetNodesBuffer().Get(), &srvd, &pass->_nodesBuffer);
    if (FAILED(hr))
    {
        throw std::exception();
    }

    srvd.Format = DXGI_FORMAT_R32_UINT;
    srvd.Buffer.ElementOffset = 0;
    srvd.Buffer.NumElements = 1920 * 1080;
    hr = device->CreateShaderResourceView(buildPass->GetOffsetBuffer().Get(), &srvd, &pass->_startOffsetBuffer);
    if (FAILED(hr))
    {
        throw std::exception();
    }

    pass->_backgroundSRV = background->GetShaderResourceView();

    pass->_background = background;
    pass->_backgroundSource = target;

    return pass;
}

_Use_decl_annotations_
RenderTransparentPass::RenderTransparentPass(const std::shared_ptr<GraphicsSystem>& graphics, const ComPtr<ID3D11DeviceContext>& context) :
    RenderPass(graphics, context)
{
    LoadShaders(L"vsPassthrough.cso", VertexFormat::Position2DProj, L"psRenderTransparentList.cso");

    CreatePSConstantBuffer(0, sizeof(psPerFrame));

    _psPerFrame.ScreenWidth = graphics->GetConfig().Width;
    UpdatePSConstantBuffer(0, &_psPerFrame, sizeof(_psPerFrame));
}

_Use_decl_annotations_
void RenderTransparentPass::HandleDrawScene(const std::shared_ptr<IGraphicsScene>& scene, const XMFLOAT4X4& view, const XMFLOAT4X4& projection)
{
    UNREFERENCED_PARAMETER(scene);
    UNREFERENCED_PARAMETER(view);
    UNREFERENCED_PARAMETER(projection);

    auto& context = GetContext();

    context->CopyResource(_background->GetTexture().Get(), _backgroundSource->GetTexture().Get());

    ID3D11ShaderResourceView* srvs[] = { _nodesBuffer.Get(), _startOffsetBuffer.Get(), _backgroundSRV.Get() };
    context->PSSetShaderResources(0, _countof(srvs), srvs);
    DrawFullScreenQuad();
}
