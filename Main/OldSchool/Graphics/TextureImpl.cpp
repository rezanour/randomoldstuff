#include <Precomp.h>
#include "TextureImpl.h"

_Use_decl_annotations_
std::shared_ptr<Texture> Texture::CreateTexture(const ComPtr<ID3D11Device>& device, uint32_t width, uint32_t height, DXGI_FORMAT format, const void* pixels, uint32_t pitch, bool generateMips, bool supportAsRenderTarget)
{
    // parameter validation
    if (generateMips && supportAsRenderTarget)
    {
        // can't be both
        throw std::exception();
    }
    else if (generateMips && format != DXGI_FORMAT_R8G8B8A8_UNORM)
    {
        // mips not supported on other formats yet
        throw std::exception();
    }

    D3D11_TEXTURE2D_DESC td = {};
    td.ArraySize = 1;
    td.Width = width;
    td.Height = height;
    td.Format = format;
    td.MipLevels = 1;
    td.SampleDesc.Count = 1;
    td.Usage = D3D11_USAGE_DEFAULT;

    td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    if (supportAsRenderTarget)
    {
        td.BindFlags |= D3D11_BIND_RENDER_TARGET;
    }

    ComPtr<ID3D11Texture2D> texture;
    ComPtr<ID3D11ShaderResourceView> srv;
    ComPtr<ID3D11RenderTargetView> rtv;

    D3D11_SUBRESOURCE_DATA data = {};
    if (pixels != nullptr)
    {
        data.pSysMem = pixels;
        data.SysMemPitch = pitch;
        data.SysMemSlicePitch = pitch * height;
    }

    HRESULT hr = device->CreateTexture2D(&td, (pixels != nullptr ? &data : nullptr), &texture);
    if (FAILED(hr))
    {
        throw std::exception();
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvd = {};
    srvd.Format = format;
    srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvd.Texture2D.MipLevels = 1;

    hr = device->CreateShaderResourceView(texture.Get(), &srvd, &srv);
    if (FAILED(hr))
    {
        throw std::exception();
    }

    if (supportAsRenderTarget)
    {
        D3D11_RENDER_TARGET_VIEW_DESC rtvd = {};
        rtvd.Format = format;
        rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        rtvd.Texture2D.MipSlice = 0;

        hr = device->CreateRenderTargetView(texture.Get(), &rtvd, &rtv);
        if (FAILED(hr))
        {
            throw std::exception();
        }
    }
    else if (generateMips)
    {
        // Generate mipmaps for the texture if it's not a render target

        td.MipLevels = 0;
        td.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
        td.BindFlags |= D3D11_BIND_RENDER_TARGET;   // in order for generate mips to work, you actually have to say it's a render target :P

        ComPtr<ID3D11Texture2D> oldTexture = texture;

        hr = device->CreateTexture2D(&td, nullptr, &texture);
        if (FAILED(hr))
        {
            throw std::exception();
        }

        srvd.Texture2D.MipLevels = static_cast<uint32_t>(-1);
        hr = device->CreateShaderResourceView(texture.Get(), &srvd, &srv);
        if (FAILED(hr))
        {
            throw std::exception();
        }

        ComPtr<ID3D11DeviceContext> context;
        device->GetImmediateContext(&context);

        D3D11_BOX box = {};
        box.right = td.Width;
        box.bottom = td.Height;
        box.back = 1;
        context->CopySubresourceRegion(texture.Get(), 0, 0, 0, 0, oldTexture.Get(), 0, &box);
        context->GenerateMips(srv.Get());
    }

    return std::shared_ptr<Texture>(new Texture(texture, srv, rtv, nullptr));
}

_Use_decl_annotations_
std::shared_ptr<Texture> Texture::CreateDepthTexture(const ComPtr<ID3D11Device>& device, uint32_t width, uint32_t height, DXGI_FORMAT surfaceFormat, DXGI_FORMAT depthFormat, bool supportAsShaderResource, DXGI_FORMAT shaderResourceFormat)
{
    D3D11_TEXTURE2D_DESC td = {};
    td.ArraySize = 1;
    td.Width = width;
    td.Height = height;
    td.Format = surfaceFormat;
    td.MipLevels = 1;
    td.SampleDesc.Count = 1;
    td.Usage = D3D11_USAGE_DEFAULT;

    td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    if (supportAsShaderResource)
    {
        td.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
    }

    ComPtr<ID3D11Texture2D> texture;
    ComPtr<ID3D11ShaderResourceView> srv;
    ComPtr<ID3D11DepthStencilView> dsv;

    HRESULT hr = device->CreateTexture2D(&td, nullptr, &texture);
    if (FAILED(hr))
    {
        throw std::exception();
    }

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvd = {};
    dsvd.Format = depthFormat;
    dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvd.Texture2D.MipSlice = 0;

    hr = device->CreateDepthStencilView(texture.Get(), &dsvd, &dsv);
    if (FAILED(hr))
    {
        throw std::exception();
    }

    if (supportAsShaderResource)
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC srvd = {};
        srvd.Format = shaderResourceFormat;
        srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvd.Texture2D.MipLevels = 1;

        hr = device->CreateShaderResourceView(texture.Get(), &srvd, &srv);
        if (FAILED(hr))
        {
            throw std::exception();
        }
    }

    return std::shared_ptr<Texture>(new Texture(texture, srv, nullptr, dsv));
}

_Use_decl_annotations_
Texture::Texture(const ComPtr<ID3D11Texture2D>& texture, const ComPtr<ID3D11ShaderResourceView>& srv, const ComPtr<ID3D11RenderTargetView>& rtv, const ComPtr<ID3D11DepthStencilView>& dsv) :
    _texture(texture), _srv(srv), _rtv(rtv), _dsv(dsv)
{
    _texture->GetDesc(&_desc);
}

DXGI_FORMAT Texture::GetFormat() const
{
    return _desc.Format;
}

uint32_t Texture::GetWidth() const
{
    return _desc.Width;
}

uint32_t Texture::GetHeight() const
{
    return _desc.Height;
}

_Use_decl_annotations_
void Texture::UpdateContents(const void* pixels, uint32_t pitch)
{
    ComPtr<ID3D11Device> device;
    _texture->GetDevice(&device);

    ComPtr<ID3D11DeviceContext> context;
    device->GetImmediateContext(&context);

    context->UpdateSubresource(_texture.Get(), 0, nullptr, pixels, pitch, 0);
}

const ComPtr<ID3D11Texture2D>& Texture::GetTexture() const
{
    return _texture;
}

const ComPtr<ID3D11ShaderResourceView>& Texture::GetShaderResourceView() const
{
    return _srv;
}

const ComPtr<ID3D11RenderTargetView>& Texture::GetRenderTargetView() const
{
    return _rtv;
}

const ComPtr<ID3D11DepthStencilView>& Texture::GetDepthStencilView() const
{
    return _dsv;
}
