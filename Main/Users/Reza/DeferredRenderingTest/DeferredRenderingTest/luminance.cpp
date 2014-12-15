#include "precomp.h"
#include "luminance.h"
#include "shaders.h"
#include "texture2d.h"
#include "camera.h"
#include "scene.h"
#include "vertex.h"
#include "object.h"

Luminance::Luminance(ID3D11DeviceContext* context, const std::shared_ptr<Texture2D>& source, const std::shared_ptr<Texture2D>& luminance) :
    _context(context), _source(source), _luminance(luminance)
{
    ComPtr<ID3D11Device> device;
    context->GetDevice(&device);

    D3D11_TEXTURE2D_DESC td = {};
    luminance->GetTexture()->GetDesc(&td);
    _psPerFrameConstants.ScreenSize = XMFLOAT2(static_cast<float>(td.Width), static_cast<float>(td.Height));

    _vertexShader = LoadVertexShader(device.Get(), L"vsCombine.cso");
    _pixelShader = LoadPixelShader(device.Get(), L"psLuminance.cso");

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

void Luminance::RenderFrame(const std::shared_ptr<Camera>& camera, const std::shared_ptr<Scene>& scene)
{
    UNREFERENCED_PARAMETER(camera);
    UNREFERENCED_PARAMETER(scene);

    D3D11_VIEWPORT viewport = {};
    viewport.Width = _psPerFrameConstants.ScreenSize.x;
    viewport.Height = _psPerFrameConstants.ScreenSize.y;
    viewport.MaxDepth = 1.0f;
    _context->RSSetViewports(1, &viewport);

    // Set buffer & nullptr for depth
    ID3D11RenderTargetView* rts[] = { _luminance->GetRenderTargetView().Get(), nullptr, nullptr, nullptr };
    _context->OMSetRenderTargets(_countof(rts), rts, nullptr);

    _context->VSSetShader(_vertexShader.Get(), nullptr, 0);
    _context->PSSetShader(_pixelShader.Get(), nullptr, 0);

    // Set depth and normal maps as inputs
    ID3D11ShaderResourceView* srvs[] = { _source->GetShaderResourceView().Get() };
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

    rts[0] = nullptr;
    _context->OMSetRenderTargets(_countof(rts), rts, nullptr);
}
