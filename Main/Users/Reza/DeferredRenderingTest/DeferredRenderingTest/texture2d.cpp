#include "precomp.h"
#include "texture2d.h"

using namespace Microsoft::WRL;

Texture2D::Texture2D(ID3D11Device* device, uint32_t width, uint32_t height, uint32_t bindFlags, DXGI_FORMAT format)
{
    Init(device, width, height, bindFlags, format, format, format, format);
}

Texture2D::Texture2D(ID3D11Device* device, uint32_t width, uint32_t height, uint32_t bindFlags, DXGI_FORMAT textureFormat, DXGI_FORMAT rtvFormat, DXGI_FORMAT srvFormat, DXGI_FORMAT dsvFormat)
{
    Init(device, width, height, bindFlags, textureFormat, rtvFormat, srvFormat, dsvFormat);
}

void Texture2D::Init(ID3D11Device* device, uint32_t width, uint32_t height, uint32_t bindFlags, DXGI_FORMAT textureFormat, DXGI_FORMAT rtvFormat, DXGI_FORMAT srvFormat, DXGI_FORMAT dsvFormat)
{
    D3D11_TEXTURE2D_DESC td = {};
    td.ArraySize = 1;
    td.BindFlags = bindFlags;
    td.Format = textureFormat;
    td.Width = width;
    td.Height = height;

    // Can only generate mips if RT + SRV AND !DSV
    if ((bindFlags & D3D11_BIND_SHADER_RESOURCE) != 0 &&
        (bindFlags & D3D11_BIND_RENDER_TARGET) != 0 &&
        (bindFlags & D3D11_BIND_DEPTH_STENCIL) == 0)
    {
        td.MipLevels = 1;
        //td.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
    }
    else
    {
        td.MipLevels = 1;
    }

    td.SampleDesc.Count = 1;
    td.SampleDesc.Quality = 0;
    td.Usage = D3D11_USAGE_DEFAULT;

    if (FAILED(device->CreateTexture2D(&td, nullptr, &_texture)))
    {
        throw std::exception();
    }

    if ((bindFlags & D3D11_BIND_RENDER_TARGET) != 0)
    {
        D3D11_RENDER_TARGET_VIEW_DESC rtvd = {};
        rtvd.Format = rtvFormat;
        rtvd.Texture2D.MipSlice = 0;
        rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

        if (FAILED(device->CreateRenderTargetView(_texture.Get(), &rtvd, &_rtv)))
        {
            throw std::exception();
        }
    }

    if ((bindFlags & D3D11_BIND_SHADER_RESOURCE) != 0)
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC srvd = {};
        srvd.Format = srvFormat;
        srvd.Texture2D.MipLevels = static_cast<UINT>(-1);
        srvd.Texture2D.MostDetailedMip = 0;
        srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

        if (FAILED(device->CreateShaderResourceView(_texture.Get(), &srvd, &_srv)))
        {
            throw std::exception();
        }
    }

    if ((bindFlags & D3D11_BIND_DEPTH_STENCIL) != 0)
    {
        D3D11_DEPTH_STENCIL_VIEW_DESC dsvd = {};
        dsvd.Format = dsvFormat;
        dsvd.Texture2D.MipSlice = 0;
        dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

        if (FAILED(device->CreateDepthStencilView(_texture.Get(), &dsvd, &_dsv)))
        {
            throw std::exception();
        }
    }
}

const ComPtr<ID3D11Texture2D>& Texture2D::GetTexture() const
{
    return _texture;
}

const ComPtr<ID3D11RenderTargetView>& Texture2D::GetRenderTargetView() const
{
    return _rtv;
}

const ComPtr<ID3D11ShaderResourceView>& Texture2D::GetShaderResourceView() const
{
    return _srv;
}

const ComPtr<ID3D11DepthStencilView>& Texture2D::GetDepthStencilView() const
{
    return _dsv;
}
