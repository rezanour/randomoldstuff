#pragma once

class Texture2D
{
public:
    Texture2D(ID3D11Device* device, uint32_t width, uint32_t height, uint32_t bindFlags, DXGI_FORMAT format);
    Texture2D(ID3D11Device* device, uint32_t width, uint32_t height, uint32_t bindFlags, DXGI_FORMAT textureFormat, DXGI_FORMAT rtvFormat, DXGI_FORMAT srvFormat, DXGI_FORMAT dsvFormat);

    const Microsoft::WRL::ComPtr<ID3D11Texture2D>& GetTexture() const;

    // These may be null if the texture wasn't created with support for that view
    const Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& GetRenderTargetView() const;
    const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetShaderResourceView() const;
    const Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& GetDepthStencilView() const;

private:
    void Init(ID3D11Device* device, uint32_t width, uint32_t height, uint32_t bindFlags, DXGI_FORMAT textureFormat, DXGI_FORMAT rtvFormat, DXGI_FORMAT srvFormat, DXGI_FORMAT dsvFormat);

private:
    Microsoft::WRL::ComPtr<ID3D11Texture2D> _texture;
    // These may be null if the texture wasn't created with support for that view
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> _rtv;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _srv;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> _dsv;
};

struct TEXTURE2D
{
    ID3D11Texture2D* Texture;

    // Some of these may be null
    ID3D11ShaderResourceView* ShaderResourceView;
    ID3D11RenderTargetView* RenderTargetView;
    ID3D11DepthStencilView* DepthStencilView;

    TEXTURE2D() : Texture(nullptr), ShaderResourceView(nullptr), RenderTargetView(nullptr), DepthStencilView(nullptr)
    {
    }

    ~TEXTURE2D()
    {
        SafeRelease(Texture);
        SafeRelease(ShaderResourceView);
        SafeRelease(RenderTargetView);
        SafeRelease(DepthStencilView);
    }
};

std::shared_ptr<TEXTURE2D> CreateTexture2D(uint32_t width, uint32_t height, uint32_t bindFlags, DXGI_FORMAT format);
