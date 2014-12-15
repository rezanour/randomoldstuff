#include "precomp.h"
#include "combine.h"
#include "shaders.h"
#include "texture2d.h"
#include "camera.h"
#include "scene.h"
#include "vertex.h"
#include "object.h"

Combine::Combine(ID3D11DeviceContext* context, const std::shared_ptr<Texture2D>& texture1, const std::shared_ptr<Texture2D>& texture2, const std::shared_ptr<Texture2D>& combine, CombineType type) :
    _context(context), _texture1(texture1), _texture2(texture2), _combine(combine), _type(type)
{
    ComPtr<ID3D11Device> device;
    context->GetDevice(&device);

    D3D11_TEXTURE2D_DESC td = {};
    combine->GetTexture()->GetDesc(&td);
    _psPerFrameConstants.ScreenSize = XMFLOAT2(static_cast<float>(td.Width), static_cast<float>(td.Height));
    _psPerFrameConstants.Type = static_cast<uint32_t>(type);

    _vertexShader = LoadVertexShader(device.Get(), L"vsCombine.cso");
    _pixelShader = LoadPixelShader(device.Get(), L"psCombine.cso");

    D3D11_SAMPLER_DESC sd = {};
    sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

    if (FAILED(device->CreateSamplerState(&sd, &_linearSampler)))
    {
        throw std::exception();
    }

    _psPerFrameConstantBuffer = CreateConstantBuffer<psPerFrameConstants>(device.Get());
    UpdateConstantBuffer(_context.Get(), _psPerFrameConstantBuffer.Get(), _psPerFrameConstants);

    _quad = Object::CreateQuad(device.Get(), 0, 0, 1, 1);

    ComPtr<IUnknown> unk;
    uint32_t unkSize = sizeof(IUnknown*);
    if (FAILED(_vertexShader->GetPrivateData(__uuidof(ICachedShaderByteCode), &unkSize, unk.GetAddressOf())))
    {
        throw std::exception();
    }

    ComPtr<ICachedShaderByteCode> byteCode;
    unk.As(&byteCode);

    D3D11_INPUT_ELEMENT_DESC elems[2] = {};
    uint32_t size = _countof(elems);
    GetInputElementsForFormat(_quad->GetVertexFormat(), elems, &size);
    if (FAILED(device->CreateInputLayout(elems, size, byteCode->GetData(), byteCode->GetSize(), &_inputLayout)))
    {
        throw std::exception();
    }
}

void Combine::RenderFrame(const std::shared_ptr<Camera>& camera, const std::shared_ptr<Scene>& scene)
{
    UNREFERENCED_PARAMETER(camera);
    UNREFERENCED_PARAMETER(scene);

    D3D11_VIEWPORT viewport = {};
    viewport.Width = _psPerFrameConstants.ScreenSize.x;
    viewport.Height = _psPerFrameConstants.ScreenSize.y;
    viewport.MaxDepth = 1.0f;
    _context->RSSetViewports(1, &viewport);

    // Set buffer & nullptr for depth
    ID3D11RenderTargetView* rts[] = { _combine->GetRenderTargetView().Get(), nullptr, nullptr, nullptr };
    _context->OMSetRenderTargets(_countof(rts), rts, nullptr);

    float combineClear[] = { 0, 0, 0, 1 };
    _context->ClearRenderTargetView(_combine->GetRenderTargetView().Get(), combineClear);

    _context->VSSetShader(_vertexShader.Get(), nullptr, 0);
    _context->PSSetShader(_pixelShader.Get(), nullptr, 0);

    // Set depth and normal maps as inputs
    ID3D11ShaderResourceView* srvs[] = { _texture1->GetShaderResourceView().Get(), _texture2->GetShaderResourceView().Get() };
    _context->PSSetShaderResources(0, _countof(srvs), srvs);
    _context->PSSetConstantBuffers(0, 1, _psPerFrameConstantBuffer.GetAddressOf());
    _context->PSSetSamplers(0, 1, _linearSampler.GetAddressOf());

    // Draw quad
    uint32_t strides[] = { sizeof(VertexPositionNormal) };
    uint32_t offsets[] = { 0 };
    _context->IASetVertexBuffers(0, 1, _quad->GetVertexBuffer().GetAddressOf(), strides, offsets);
    _context->IASetIndexBuffer(_quad->GetIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);
    _context->IASetInputLayout(_inputLayout.Get());
    _context->DrawIndexed(_quad->GetIndexCount(), 0, 0);
}
