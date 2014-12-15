#pragma once

#include <Texture.h>

class Texture : public BaseObject<Texture>, public ITexture
{
public:
    // Create texture suitable for consumption in a shader, and optionally support being rendered into
    static std::shared_ptr<Texture> CreateTexture(_In_ const ComPtr<ID3D11Device>& device, _In_ uint32_t width, _In_ uint32_t height, _In_ DXGI_FORMAT format, _In_opt_ const void* pixels, _In_ uint32_t pitch, _In_ bool generateMips, _In_ bool supportAsRenderTarget);

    // Create texture suitable for depth use, may also be used as shader resource if desired. Does NOT support being a render target
    static std::shared_ptr<Texture> CreateDepthTexture(_In_ const ComPtr<ID3D11Device>& device, _In_ uint32_t width, _In_ uint32_t height, _In_ DXGI_FORMAT surfaceFormat, _In_ DXGI_FORMAT depthFormat, _In_ bool supportAsShaderResource, _In_ DXGI_FORMAT shaderResourceFormat);

    //
    // ITexture
    //

    DXGI_FORMAT GetFormat() const override;
    uint32_t GetWidth() const override;
    uint32_t GetHeight() const override;

    void UpdateContents(_In_ const void* pixels, _In_ uint32_t pitch) override;

    //
    // Internal
    //

    const ComPtr<ID3D11Texture2D>& GetTexture() const;
    const ComPtr<ID3D11ShaderResourceView>& GetShaderResourceView() const;
    const ComPtr<ID3D11RenderTargetView>& GetRenderTargetView() const;
    const ComPtr<ID3D11DepthStencilView>& GetDepthStencilView() const;

private:
    Texture(_In_ const ComPtr<ID3D11Texture2D>& texture, _In_ const ComPtr<ID3D11ShaderResourceView>& srv, _In_ const ComPtr<ID3D11RenderTargetView>& rtv, _In_ const ComPtr<ID3D11DepthStencilView>& dsv);

private:
    ComPtr<ID3D11Texture2D> _texture;
    ComPtr<ID3D11ShaderResourceView> _srv;
    ComPtr<ID3D11RenderTargetView> _rtv;
    ComPtr<ID3D11DepthStencilView> _dsv;

    D3D11_TEXTURE2D_DESC _desc;
};
