#include "precomp.h"
#include "gbuffer.h"
#include "shaders.h"
#include "texture2d.h"
#include "object.h"
#include "vertex.h"
#include "camera.h"
#include "scene.h"
#include "..\..\DirectXTex\DirectXTex\DirectXTex.h"

using namespace Microsoft::WRL;
using namespace DirectX;

GBuffer::GBuffer(ID3D11DeviceContext* context, const std::shared_ptr<Texture2D>& diffuse, const std::shared_ptr<Texture2D>& normals, const std::shared_ptr<Texture2D>& depth) :
    _context(context),
    _diffuse(diffuse),
    _normals(normals),
    _depth(depth)
{
    ComPtr<ID3D11Device> device;
    _context->GetDevice(&device);

    _vertexShader = LoadVertexShader(device.Get(), L"vsGBuffer.cso");
    _pixelShader = LoadPixelShader(device.Get(), L"psGBuffer.cso");

    _vsPerFrameConstantBuffer = CreateConstantBuffer<vsPerFrameConstantData>(device.Get());
    _vsPerObjectConstantBuffer = CreateConstantBuffer<vsPerObjectConstantData>(device.Get());

    D3D11_INPUT_ELEMENT_DESC elems[3] = {};
    uint32_t size = _countof(elems);
    GetInputElementsForFormat(VertexFormat::PositionNormalTexture, elems, &size);

    uint32_t privDataSize = sizeof(IUnknown*);
    ComPtr<IUnknown> unk;
    if (FAILED(_vertexShader->GetPrivateData(__uuidof(ICachedShaderByteCode), &privDataSize, unk.GetAddressOf())))
    {
        throw std::exception();
    }

    ComPtr<ICachedShaderByteCode> code;
    if (FAILED(unk.As(&code)))
    {
        throw std::exception();
    }

    if (FAILED(device->CreateInputLayout(elems, size, code->GetData(), code->GetSize(), &_inputLayoutPositionNormal)))
    {
        throw std::exception();
    }

    TexMetadata metadata = {};
    ScratchImage image;
    if (FAILED(DirectX::LoadFromDDSFile(L"bubble_normalmap.dds", DDS_FLAGS_NONE, &metadata, image)))
    {
        throw std::exception();
    }

    D3D11_TEXTURE2D_DESC td = {};
    td.ArraySize = 1;
    td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    td.Format = metadata.format;
    td.Width = static_cast<UINT>(metadata.width);
    td.Height = static_cast<UINT>(metadata.height);
    td.MipLevels = 1;
    td.SampleDesc.Count = 1;
    td.Usage = D3D11_USAGE_DEFAULT;

    D3D11_SUBRESOURCE_DATA init = {};
    init.pSysMem = image.GetPixels();
    init.SysMemPitch = static_cast<UINT>(metadata.width) * 4;
    init.SysMemSlicePitch = 0;

    ComPtr<ID3D11Texture2D> texture;
    if (FAILED(device->CreateTexture2D(&td, &init, &texture)))
    {
        throw std::exception();
    }

    if (FAILED(device->CreateShaderResourceView(texture.Get(), nullptr, &_normalMap)))
    {
        throw std::exception();
    }

    D3D11_SAMPLER_DESC sd = {};
    sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sd.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;

    if (FAILED(device->CreateSamplerState(&sd, &_pointSampler)))
    {
        throw std::exception();
    }
}

void GBuffer::RenderFrame(const std::shared_ptr<Camera>& camera, const std::shared_ptr<Scene>& scene)
{
    BeginFrame(camera);
    RenderObjects(camera, scene);
}

void GBuffer::BeginFrame(const std::shared_ptr<Camera>& camera)
{
    Bind();
    Clear();

    _vsPerFrameConstants.View = camera->GetView();
    _vsPerFrameConstants.Projection = camera->GetProjection();
    UpdateConstantBuffer(_context.Get(), _vsPerFrameConstantBuffer.Get(), _vsPerFrameConstants);
}

void GBuffer::RenderObjects(const std::shared_ptr<Camera>& camera, const std::shared_ptr<Scene>& scene)
{
    uint32_t strides[] = { sizeof(VertexPositionNormalTexture) };
    uint32_t offsets[] = { 0 };

    UNREFERENCED_PARAMETER(camera);
    scene->GetObjectsInSphere(XMFLOAT3(0, 0, 0), 1.0f, _payload);

    for (auto object : _payload)
    {
        assert(object->GetVertexFormat() == VertexFormat::PositionNormalTexture);

        _context->IASetVertexBuffers(0, 1, object->GetVertexBuffer().GetAddressOf(), strides, offsets);
        _context->IASetIndexBuffer(object->GetIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);

        _vsPerObjectConstants.World = object->GetWorld();
        UpdateConstantBuffer(_context.Get(), _vsPerObjectConstantBuffer.Get(), _vsPerObjectConstants);

        _context->DrawIndexed(object->GetIndexCount(), 0, 0);
    }

    _payload.clear();
}

void GBuffer::Bind()
{
    // Input Assembler (only supports this one input layout for now)
    _context->IASetInputLayout(_inputLayoutPositionNormal.Get());
    _context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Vertex shader
    _context->VSSetShader(_vertexShader.Get(), nullptr, 0);

    ID3D11Buffer* vsBuffers[] = { _vsPerFrameConstantBuffer.Get(), _vsPerObjectConstantBuffer.Get() };
    _context->VSSetConstantBuffers(0, _countof(vsBuffers), vsBuffers);

    // Pixel shader
    _context->PSSetShader(_pixelShader.Get(), nullptr, 0);

    // shader resource views
    ID3D11ShaderResourceView* srvs[] = { nullptr, nullptr, nullptr, nullptr };
    _context->VSSetShaderResources(0, _countof(srvs), srvs);
    _context->PSSetShaderResources(0, _countof(srvs), srvs);
    _context->PSSetShaderResources(0, 1, _normalMap.GetAddressOf());
    _context->PSSetSamplers(0, 1, _pointSampler.GetAddressOf());

    // Render targets & depth
    ID3D11RenderTargetView* targets[] =
    {
        _diffuse->GetRenderTargetView().Get(),
        _normals->GetRenderTargetView().Get()
    };
    _context->OMSetRenderTargets(_countof(targets), targets, _depth->GetDepthStencilView().Get());

    D3D11_VIEWPORT viewport = {};
    viewport.Width = 1280.0f;
    viewport.Height = 720.0f;
    viewport.MaxDepth = 1.0f;

    _context->RSSetViewports(1, &viewport);
}

void GBuffer::Clear()
{
    static const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    _context->ClearRenderTargetView(_diffuse->GetRenderTargetView().Get(), clearColor);

    static const float clearNormals[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    _context->ClearRenderTargetView(_normals->GetRenderTargetView().Get(), clearColor);

    _context->ClearDepthStencilView(_depth->GetDepthStencilView().Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}
