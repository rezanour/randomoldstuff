#include "precomp.h"
#include "tonemapping.h"
#include "shaders.h"
#include "texture2d.h"
#include "camera.h"
#include "scene.h"
#include "vertex.h"
#include "object.h"
#include <stddef.h>

ToneMapping::ToneMapping(ID3D11DeviceContext* context, const std::shared_ptr<Texture2D>& source, const std::shared_ptr<Texture2D>& luminance, const std::shared_ptr<Texture2D>& dest) :
    _context(context), _source(source), _luminance(luminance), _dest(dest)
{
    ComPtr<ID3D11Device> device;
    context->GetDevice(&device);

    D3D11_TEXTURE2D_DESC td = {};
    dest->GetTexture()->GetDesc(&td);
    _psPerFrameConstants.ScreenSize = XMFLOAT2(static_cast<float>(td.Width), static_cast<float>(td.Height));

    D3D11_SHADER_RESOURCE_VIEW_DESC svd = {};
    luminance->GetTexture()->GetDesc(&td);
    svd.Format = td.Format;
    svd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    svd.Texture2D.MipLevels = 1;
    svd.Texture2D.MostDetailedMip = 0;
    if (FAILED(device->CreateShaderResourceView(luminance->GetTexture().Get(), &svd, &_luminanceView)))
    {
        throw std::exception();
    }

    td.BindFlags = 0;
    td.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    td.Format = DXGI_FORMAT_R32_FLOAT;
    td.MiscFlags = 0;
    td.Usage = D3D11_USAGE_STAGING;
    if (FAILED(device->CreateTexture2D(&td, nullptr, &_luminance2)))
    {
        throw std::exception();
    }

    _vertexShader = LoadVertexShader(device.Get(), L"vsCombine.cso");
    _pixelShader = LoadPixelShader(device.Get(), L"psTonemapping.cso");

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

void ToneMapping::RenderFrame(const std::shared_ptr<Camera>& camera, const std::shared_ptr<Scene>& scene)
{
    UNREFERENCED_PARAMETER(camera);
    UNREFERENCED_PARAMETER(scene);

    _context->CopyResource(_luminance2.Get(), _luminance->GetTexture().Get());

    D3D11_MAPPED_SUBRESOURCE mapped = {};
    _context->Map(_luminance2.Get(), 0, D3D11_MAP_READ, 0, &mapped);

    float totalLuminance = 0;
    float maxLuminance = 0;
    uint32_t width = static_cast<uint32_t>(_psPerFrameConstants.ScreenSize.x);
    uint32_t height = static_cast<uint32_t>(_psPerFrameConstants.ScreenSize.y);

    float* pixels = reinterpret_cast<float*>(mapped.pData);
    for (uint32_t y = 0; y < height; ++y)
    {
        for (uint32_t x = 0; x < width; ++x)
        {
            float L = *pixels;
            if (L > maxLuminance)
            {
                maxLuminance = L;
            }

            //L = expf(L);
            totalLuminance += L;
            pixels++;
        }
    }

    _psPerFrameConstants.AverageLuminance = /*logf*/(totalLuminance) / (width * height);
    _psPerFrameConstants.MaximumLuminance = maxLuminance;

    _context->Unmap(_luminance2.Get(), 0);
    UpdateConstantBuffer(_context.Get(), _psPerFrameConstantBuffer.Get(), _psPerFrameConstants);

    D3D11_VIEWPORT viewport = {};
    viewport.Width = _psPerFrameConstants.ScreenSize.x;
    viewport.Height = _psPerFrameConstants.ScreenSize.y;
    viewport.MaxDepth = 1.0f;
    _context->RSSetViewports(1, &viewport);

    // Set buffer & nullptr for depth
    ID3D11RenderTargetView* rts[] = { _dest->GetRenderTargetView().Get(), nullptr, nullptr, nullptr };
    _context->OMSetRenderTargets(_countof(rts), rts, nullptr);

    _context->VSSetShader(_vertexShader.Get(), nullptr, 0);
    _context->PSSetShader(_pixelShader.Get(), nullptr, 0);

    // Set depth and normal maps as inputs
    ID3D11ShaderResourceView* srvs[] = { _source->GetShaderResourceView().Get(), _luminanceView.Get(), nullptr, nullptr };
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
