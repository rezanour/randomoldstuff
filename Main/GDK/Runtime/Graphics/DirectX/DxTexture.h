#pragma once

#include "..\RuntimeTexture.h"
#include <d3d11.h>
#include <wrl\client.h>

namespace GDK
{
    using Microsoft::WRL::ComPtr;

    class DxTexture : public RuntimeTexture
    {
    public:
        static std::shared_ptr<DxTexture> Create(_In_ const std::shared_ptr<GraphicsDevice>& graphicsDevice, _In_ const std::shared_ptr<TextureContent>& content);
        static std::shared_ptr<DxTexture> Create(_In_ const std::shared_ptr<GraphicsDevice>& graphicsDevice, _In_ uint32_t width, _In_ uint32_t height, _In_ TextureFormat format, _In_ const byte_t* pixels);

        virtual void Bind(_In_ uint32_t slot) override;
        virtual void Unbind(_In_ uint32_t slot) override;

    private:
        DxTexture(_In_ const std::shared_ptr<GraphicsDevice>& graphicsDevice, _In_ uint32_t width, _In_ uint32_t height, _In_ TextureFormat format, _In_ const byte_t* pixels);

        static DXGI_FORMAT DxgiFormatFromTextureFormat(_In_ TextureFormat format);
        static uint32_t PixelSizeInBytes(_In_ DXGI_FORMAT format);

        std::weak_ptr<GraphicsDevice> _device;
        ComPtr<ID3D11Texture2D> _texture;
        ComPtr<ID3D11ShaderResourceView> _srv;
    };
}
