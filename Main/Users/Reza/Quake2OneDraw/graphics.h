#pragma once

class Bsp;

class Renderer
{
public:
    Renderer(_In_ HWND hwnd);

    void Clear();
    void Present();

    void DrawBsp(const XMFLOAT4X4& cameraWorld, const XMFLOAT4X4& view, const XMFLOAT4X4& projection, const std::unique_ptr<Bsp>& bsp);

    ComPtr<ID3D11Device> GetDevice() const { return _device; }
    ComPtr<ID3D11DeviceContext> GetContext() const { return _context; }

private:
    Renderer(const Renderer&);
    Renderer& operator= (const Renderer&);

private:
    HWND _hwnd;
    ComPtr<ID3D11Device> _device;
    ComPtr<ID3D11DeviceContext> _context;
    ComPtr<IDXGISwapChain> _swapChain;
    ComPtr<ID3D11RenderTargetView> _backBuffer;
    ComPtr<ID3D11DepthStencilView> _depthBuffer;

    // Pipeline
    ComPtr<ID3D11InputLayout> _inputLayout;
    ComPtr<ID3D11VertexShader> _vertexShader;
    ComPtr<ID3D11PixelShader> _pixelShader;
    ComPtr<ID3D11SamplerState> _sampler;
    ComPtr<ID3D11Buffer> _vsPerFrame;
    ComPtr<ID3D11Buffer> _indexBuffer;

    struct VSPerFrame
    {
        XMFLOAT4X4 View;
        XMFLOAT4X4 Projection;
    };

    static const uint32_t MaxIndices = 128000;
    std::unique_ptr<uint32_t[]> _indices;
    uint32_t _numIndices;
};
