#include "precomp.h"
#include "blur.h"
#include "shaders.h"
#include "texture2d.h"
#include "vertex.h"
#include "object.h"

Blur::Blur(ID3D11DeviceContext* context) :
    _context(context)
{
    ComPtr<ID3D11Device> device;
    context->GetDevice(&device);

    _vertexShader = LoadVertexShader(device.Get(), L"vsBlur.cso");
    _pixelShader = LoadPixelShader(device.Get(), L"psBlur.cso");

    _psConstantBuffer = CreateConstantBuffer<psConstants>(device.Get());

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
    GetInputElementsForFormat(VertexFormat::PositionNormal, elems, &size);
    if (FAILED(device->CreateInputLayout(elems, size, byteCode->GetData(), byteCode->GetSize(), &_inputLayout)))
    {
        throw std::exception();
    }

    D3D11_SAMPLER_DESC sd = {};
    sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

    if (FAILED(device->CreateSamplerState(&sd, &_linearSampler)))
    {
        throw std::exception();
    }

    _quad = Object::CreateQuad(device.Get(), 0, 0, 1, 1);
}

void Blur::BlurSurface(const std::shared_ptr<Texture2D>& surface, const std::shared_ptr<Texture2D>& scratch)
{
    ComPtr<ID3D11Device> device;
    _context->GetDevice(&device);

    _context->IASetInputLayout(_inputLayout.Get());

    uint32_t strides[] = { sizeof(VertexPositionNormal) };
    uint32_t offsets[] = { 0 };

    ID3D11RenderTargetView* rt[] = { nullptr };
    _context->OMSetRenderTargets(1, rt, nullptr);

    _context->VSSetShader(_vertexShader.Get(), nullptr, 0);
    _context->PSSetShader(_pixelShader.Get(), nullptr, 0);
    _context->PSSetConstantBuffers(0, 1, _psConstantBuffer.GetAddressOf());
    _context->PSSetShaderResources(0, 1, surface->GetShaderResourceView().GetAddressOf());
    _context->PSSetSamplers(0, 1, _linearSampler.GetAddressOf());

    D3D11_TEXTURE2D_DESC td = {};
    scratch->GetTexture()->GetDesc(&td);
    _psConstants.ScreenSize = XMFLOAT2(static_cast<float>(td.Width), static_cast<float>(td.Height));
    _psConstants.FlipXY = 0.0f;
     UpdateConstantBuffer(_context.Get(), _psConstantBuffer.Get(), _psConstants);

    D3D11_VIEWPORT viewport = {};
    viewport.Width = static_cast<float>(td.Width);
    viewport.Height = static_cast<float>(td.Height);
    viewport.MaxDepth = 1.0f;

    _context->RSSetViewports(1, &viewport);

    _context->OMSetRenderTargets(1, scratch->GetRenderTargetView().GetAddressOf(), nullptr);

    _context->IASetIndexBuffer(_quad->GetIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);
    _context->IASetVertexBuffers(0, 1, _quad->GetVertexBuffer().GetAddressOf(), strides, offsets);
    _context->DrawIndexed(_quad->GetIndexCount(), 0, 0);

    // and the other direction
    _context->OMSetRenderTargets(1, rt, nullptr);

    _context->PSSetShaderResources(0, 1, scratch->GetShaderResourceView().GetAddressOf());
    _context->OMSetRenderTargets(1, surface->GetRenderTargetView().GetAddressOf(), nullptr);

    _psConstants.FlipXY = 1.0f;
     UpdateConstantBuffer(_context.Get(), _psConstantBuffer.Get(), _psConstants);

    _context->IASetIndexBuffer(_quad->GetIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);
    _context->IASetVertexBuffers(0, 1, _quad->GetVertexBuffer().GetAddressOf(), strides, offsets);
    _context->DrawIndexed(_quad->GetIndexCount(), 0, 0);

    ID3D11ShaderResourceView* srv[] = { nullptr };
    _context->PSSetShaderResources(0, 1, srv);
    _context->OMSetRenderTargets(1, rt, nullptr);
}
