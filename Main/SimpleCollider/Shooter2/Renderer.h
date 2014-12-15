#pragma once

struct RendererConfig
{
    HWND        Window;
    uint32_t    Width;
    uint32_t    Height;
    bool        DebugDevice;
};

class Renderer
{
public:
    Renderer(const RendererConfig& config);

    //
    // Properties
    //
    const ComPtr<ID3D11Device>&             Device() const { return _device; }
    const ComPtr<ID3D11DeviceContext>&      Context() const { return _context; }

    void Clear(const float color[4], float depth);
    void Present(bool waitForVSync);

private:
    void InitializeGraphics(const RendererConfig& config);

private:
    HANDLE _frameEvent;
    ComPtr<ID3D11Device> _device;
    ComPtr<IDXGISwapChain> _swapChain;
    ComPtr<ID3D11DeviceContext> _context;
    ComPtr<ID3D11RenderTargetView> _renderTarget;
    ComPtr<ID3D11DepthStencilView> _depthStencilView;
};
