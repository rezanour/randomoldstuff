#include "DxTexture.h"
#include "DxGraphicsDevice.h"
#include <GDKError.h>

namespace GDK
{
    std::shared_ptr<DxTexture> DxTexture::Create(_In_ const std::shared_ptr<GraphicsDevice>& graphicsDevice, _In_ const std::shared_ptr<TextureContent>& content)
    {
        return std::shared_ptr<DxTexture>(GDKNEW DxTexture(graphicsDevice, content->GetWidth(), content->GetHeight(), content->GetFormat(), content->GetPixels()));
    }

    std::shared_ptr<DxTexture> DxTexture::Create(_In_ const std::shared_ptr<GraphicsDevice>& graphicsDevice, _In_ uint32_t width, _In_ uint32_t height, _In_ TextureFormat format, _In_ const byte_t* pixels)
    {
        return std::shared_ptr<DxTexture>(GDKNEW DxTexture(graphicsDevice, width, height, format, pixels));
    }

    DxTexture::DxTexture(_In_ const std::shared_ptr<GraphicsDevice>& graphicsDevice, _In_ uint32_t width, _In_ uint32_t height, _In_ TextureFormat format, _In_ const byte_t* pixels) : 
        _device(graphicsDevice)
    {
        D3D11_TEXTURE2D_DESC desc = {};
        desc.ArraySize = 1;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.Format = DxgiFormatFromTextureFormat(format);
        desc.Width = width;
        desc.Height = height;
        desc.MipLevels = 1;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Usage = D3D11_USAGE_DEFAULT;

        D3D11_SUBRESOURCE_DATA initialData = {};
        initialData.pSysMem = pixels;
        initialData.SysMemPitch = width * PixelSizeInBytes(desc.Format);

        DxGraphicsDevice* dxGraphicsDevice = static_cast<DxGraphicsDevice*>(graphicsDevice.get());
        CHECK_HR(dxGraphicsDevice->GetDevice()->CreateTexture2D(&desc, &initialData, &_texture));
        CHECK_HR(dxGraphicsDevice->GetDevice()->CreateShaderResourceView(_texture.Get(), nullptr, &_srv));
    }

    void DxTexture::Bind(_In_ uint32_t slot)
    {
        ID3D11ShaderResourceView* shaderResourceViews[] = { _srv.Get() };
        DxGraphicsDevice* dxGraphicsDevice = static_cast<DxGraphicsDevice*>(_device.get());
        dxGraphicsDevice->GetContext()->PSSetShaderResources(slot, 1, shaderResourceViews);
    }

    void DxTexture::Unbind(_In_ uint32_t slot)
    {
        static ID3D11ShaderResourceView* shaderResourceViews[] = { nullptr };
        DxGraphicsDevice* dxGraphicsDevice = static_cast<DxGraphicsDevice*>(_device.get());
        dxGraphicsDevice->GetContext()->PSSetShaderResources(slot, 1, shaderResourceViews);
    }

    DXGI_FORMAT DxTexture::DxgiFormatFromTextureFormat(_In_ TextureFormat format)
    {
        switch (format)
        {
        case TextureFormat::R8G8B8A8:
            return DXGI_FORMAT_R8G8B8A8_UNORM;

        default:
            throw std::invalid_argument("Unsupported TextureFormat");
        }
    }

    uint32_t DxTexture::PixelSizeInBytes(_In_ DXGI_FORMAT format)
    {
        switch (format)
        {
        case DXGI_FORMAT_R8G8B8A8_UNORM:
            return 4;

        default:
            throw std::invalid_argument("Unsupported pixel format");
        }
    }
}
