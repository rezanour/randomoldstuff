#include <Precomp.h>
#include "GraphicsSystemImpl.h"
#include "TextureImpl.h"
#include "Shaders.h"
#include "Renderer2D.h"

_Use_decl_annotations_
std::shared_ptr<Renderer2D> Renderer2D::Create(const std::shared_ptr<GraphicsSystem>& system, const ComPtr<ID3D11DeviceContext>& context, const ComPtr<ID3D11RenderTargetView>& target, const ComPtr<ID3D11DepthStencilView>& depthBuffer)
{
    return std::shared_ptr<Renderer2D>(new Renderer2D(system, context, target, depthBuffer));
}

_Use_decl_annotations_
Renderer2D::Renderer2D(const std::shared_ptr<GraphicsSystem>& system, const ComPtr<ID3D11DeviceContext>& context, const ComPtr<ID3D11RenderTargetView>& target, const ComPtr<ID3D11DepthStencilView>& depthBuffer) :
    _system(system), _context(context), _target(target), _depthBuffer(depthBuffer), _rtWidth(0), _rtHeight(0)
{
    ComPtr<ID3D11Device> device;
    _context->GetDevice(&device);

    ComPtr<ID3D11Resource> resource;
    _target->GetResource(&resource);

    ComPtr<ID3D11Texture2D> texture;
    HRESULT hr = resource.As(&texture);
    if (FAILED(hr))
    {
        throw std::exception();
    }

    D3D11_TEXTURE2D_DESC td = {};
    texture->GetDesc(&td);

    _rtWidth = static_cast<float>(td.Width);
    _rtHeight = static_cast<float>(td.Height);

    std::unique_ptr<uint8_t[]> byteCode;
    size_t byteCodeSize = 0;
    _vertexShader = LoadVertexShader(device, L"vsDraw2D.cso", byteCode, &byteCodeSize);
    _pixelShader = LoadPixelShader(device, L"psDraw2D.cso");

    D3D11_INPUT_ELEMENT_DESC elems[5] = {};

    // vertex data
    elems[0].Format = DXGI_FORMAT_R32G32_FLOAT;
    elems[0].SemanticName = "POSITION";
    elems[1].AlignedByteOffset = sizeof(XMFLOAT2);
    elems[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    elems[1].SemanticName = "TEXCOORD";

    // instance data
    elems[2].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    elems[2].InputSlot = 1;
    elems[2].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
    elems[2].InstanceDataStepRate = 1;
    elems[2].SemanticIndex = 1;
    elems[2].SemanticName = "TEXCOORD";
    elems[3].AlignedByteOffset = sizeof(XMFLOAT4);
    elems[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    elems[3].InputSlot = 1;
    elems[3].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
    elems[3].InstanceDataStepRate = 1;
    elems[3].SemanticIndex = 2;
    elems[3].SemanticName = "TEXCOORD";
    elems[4].AlignedByteOffset = 2 * sizeof(XMFLOAT4);
    elems[4].Format = DXGI_FORMAT_R32_UINT;
    elems[4].InputSlot = 1;
    elems[4].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
    elems[4].InstanceDataStepRate = 1;
    elems[4].SemanticIndex = 3;
    elems[4].SemanticName = "TEXCOORD";

    hr = device->CreateInputLayout(elems, _countof(elems), byteCode.get(), byteCodeSize, &_inputLayout);
    if (FAILED(hr))
    {
        throw std::exception();
    }

    _sampler = system->GetLinearClampSampler();
    _blendState = system->GetPremultipliedAlphaBlendState();

    Vertex2D verts[] = 
    {
        { XMFLOAT2(0, 1),   XMFLOAT2(0, 1) },
        { XMFLOAT2(0, 0),   XMFLOAT2(0, 0) },
        { XMFLOAT2(1, 0),   XMFLOAT2(1, 0) },
        { XMFLOAT2(1, 1),   XMFLOAT2(1, 1) },
    };

    uint32_t indices[] = { 0, 1, 2, 0, 2, 3 };

    _numIndices = _countof(indices);

    D3D11_BUFFER_DESC bd = {};
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.ByteWidth = sizeof(verts);
    bd.StructureByteStride = sizeof(Vertex2D);
    bd.Usage = D3D11_USAGE_DEFAULT;

    D3D11_SUBRESOURCE_DATA init = {};
    init.pSysMem = verts;
    init.SysMemPitch = bd.ByteWidth;

    hr = device->CreateBuffer(&bd, &init, &_quadVertices);
    if (FAILED(hr))
    {
        throw std::exception();
    }

    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.ByteWidth = sizeof(indices);
    bd.StructureByteStride = sizeof(uint32_t);

    init.pSysMem = indices;
    init.SysMemPitch = bd.ByteWidth;

    hr = device->CreateBuffer(&bd, &init, &_quadIndices);
    if (FAILED(hr))
    {
        throw std::exception();
    }

    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.ByteWidth = sizeof(_vsPerFrame);
    bd.StructureByteStride = sizeof(_vsPerFrame);

    hr = device->CreateBuffer(&bd, nullptr, &_vsPerFrameBuffer);
    if (FAILED(hr))
    {
        throw std::exception();
    }

    _vsPerFrame.NumInstances = 0;
}

_Use_decl_annotations_
void Renderer2D::DrawImage(const std::shared_ptr<ITexture>& texture, int32_t x, int32_t y, uint32_t width, uint32_t height)
{
    RECT source = {};
    source.right = texture->GetWidth();
    source.bottom = texture->GetHeight();

    RECT dest = {};
    dest.left = x;
    dest.top = y;
    dest.right = x + width;
    dest.bottom = y + height;

    DrawImage(texture, source, dest);
}

_Use_decl_annotations_
void Renderer2D::DrawImage(const std::shared_ptr<ITexture>& texture, const RECT& source, const RECT& dest)
{
    Texture* tex2D = static_cast<Texture*>(texture.get());

    std::shared_ptr<Batch> batch;

    auto it = _batches.find(tex2D->GetShaderResourceView());
    if (it != std::end(_batches))
    {
        batch = it->second;
    }
    else
    {
        // Is there one available in our pool?
        if (_freeBatches.size() > 0)
        {
            batch = _freeBatches.back();
            _freeBatches.pop_back();

            batch->NumInstances = 0;
        }
        else
        {
            // create a new one
            batch = std::make_shared<Batch>();

            batch->Instances.reset(new DrawInstance[MaxInstances]);
            batch->NumInstances = 0;

            D3D11_BUFFER_DESC bd = {};
            bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            bd.ByteWidth = MaxInstances * sizeof(DrawInstance);
            bd.StructureByteStride = sizeof(DrawInstance);
            bd.Usage = D3D11_USAGE_DEFAULT;

            ComPtr<ID3D11Device> device;
            _context->GetDevice(&device);

            HRESULT hr = device->CreateBuffer(&bd, nullptr, &batch->InstanceBuffer);
            if (FAILED(hr))
            {
                throw std::exception();
            }
        }

        // regardless of whether it came from pool or is new, add to the map
        _batches[tex2D->GetShaderResourceView()] = batch;
    }

    // is there room in the batch?
    if (batch->NumInstances >= MaxInstances)
    {
        assert(false);

        // dropping instance...
        return;
    }

    // Add instance (from the back of the array forward, so that we render in backwards order to take advantage of the depth buffer)
    auto& instance = batch->Instances.get()[MaxInstances - batch->NumInstances - 1];
    batch->NumInstances++;

    float srcWidth = static_cast<float>(tex2D->GetWidth());
    float srcHeight = static_cast<float>(tex2D->GetHeight());
    instance.Source = XMFLOAT4(source.left / srcWidth, source.top / srcHeight, source.right / srcWidth, source.bottom / srcHeight);
    instance.Dest = XMFLOAT4(dest.left / _rtWidth, dest.top / _rtHeight, dest.right / _rtWidth, dest.bottom / _rtHeight);
    instance.InstanceId = _vsPerFrame.NumInstances++;
}

void Renderer2D::Render()
{
    static const uint32_t stride = sizeof(DrawInstance);
    static const uint32_t offset = 0;

    PrepareForFrame();

    for (auto batch : _batches)
    {
        assert(batch.second->NumInstances > 0);

        _context->UpdateSubresource(_vsPerFrameBuffer.Get(), 0, nullptr, &_vsPerFrame, sizeof(_vsPerFrame), 0);

        // Set the instance buffer
        _context->IASetVertexBuffers(1, 1, batch.second->InstanceBuffer.GetAddressOf(), &stride, &offset);

        D3D11_BOX box = {};
        box.left = 0;
        box.right = batch.second->NumInstances * sizeof(DrawInstance);
        box.bottom = 1;
        box.back = 1;
        _context->UpdateSubresource(batch.second->InstanceBuffer.Get(), 0, &box, &batch.second->Instances.get()[MaxInstances - batch.second->NumInstances], sizeof(DrawInstance) * batch.second->NumInstances, 0);

        _context->PSSetShaderResources(0, 1, batch.first.GetAddressOf());
        _context->DrawIndexedInstanced(_numIndices, batch.second->NumInstances, 0, 0, 0);

        batch.second->NumInstances = 0;
        _freeBatches.push_back(batch.second);
    }
    _batches.clear();

    _vsPerFrame.NumInstances = 0;
}

void Renderer2D::PrepareForFrame()
{
    static const float blendFactors[] = { 1, 1, 1, 1 };
    static const uint32_t stride = sizeof(Vertex2D);
    static const uint32_t offset = 0;

    D3D11_VIEWPORT vp = {};
    vp.Width = _rtWidth;
    vp.Height = _rtHeight;
    vp.MaxDepth = 1.0f;

    // Input Assembler
    _context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    _context->IASetInputLayout(_inputLayout.Get());
    _context->IASetVertexBuffers(0, 1, _quadVertices.GetAddressOf(), &stride, &offset);
    _context->IASetIndexBuffer(_quadIndices.Get(), DXGI_FORMAT_R32_UINT, 0);

    // Vertex Shader
    _context->VSSetShader(_vertexShader.Get(), nullptr, 0);
    _context->VSSetConstantBuffers(0, 1, _vsPerFrameBuffer.GetAddressOf());

    // Pixel Shader
    _context->PSSetShader(_pixelShader.Get(), nullptr, 0);
    _context->PSSetSamplers(0, 1, _sampler.GetAddressOf());

    // Rasterizer
    _context->RSSetViewports(1, &vp);

    // OutputMerger
    _context->OMSetBlendState(_blendState.Get(), blendFactors, 0xffffffff);
    _context->OMSetRenderTargets(1, _target.GetAddressOf(), _depthBuffer.Get());

    _context->ClearDepthStencilView(_depthBuffer.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}
