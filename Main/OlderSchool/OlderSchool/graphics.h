#pragma once

//
// The Graphics object is the root of the graphics subsystem.
//
class Graphics
{
public:
    //
    // Returns unique pointer. Only WinMain controls this object's lifetime.
    //
    static std::unique_ptr<Graphics> Create(_In_ HWND window);
    ~Graphics();

    const ComPtr<ID3D11Device>& GetDevice() const { return _device; }

    void Clear(_In_ const XMFLOAT4& color);
    void Present();

private:
    Graphics(_In_ HWND window);

private:
    HWND _window;

    ComPtr<ID3D11Device> _device;
    ComPtr<ID3D11DeviceContext> _context;
    ComPtr<IDXGISwapChain> _swapChain;
    ComPtr<ID3D11RenderTargetView> _backBuffer;
};
