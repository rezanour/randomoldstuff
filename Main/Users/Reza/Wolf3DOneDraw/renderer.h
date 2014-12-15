#pragma once

class Renderer
{
    struct ClearVertex
    {
        XMFLOAT2 Position;
    };

    struct ClearConstants
    {
        XMFLOAT4 CeilingColor;
        XMFLOAT4 FloorColor;
        XMFLOAT2 ScreenSize;
        XMFLOAT2 Padding;
    };

    struct Draw3DVertex
    {
        XMFLOAT3 Position;
        XMFLOAT3 Normal;
        XMFLOAT2 TexCoord;
    };

    struct Draw3DInstance
    {
        uint32_t TexIndex;
        uint32_t CellIndex;
    };

    struct Draw3DConstants
    {
        XMFLOAT4X4 View;
        XMFLOAT4X4 Projection;
    };

public:
    struct Level
    {
        char Name[16];
        uint32_t Width;
        uint32_t Height;
        std::vector<uint16_t> Data;
    };

    static HRESULT Create(_In_ HWND hwnd, _In_z_ const wchar_t* contentRoot, _Outptr_ Renderer** renderer);

    uint32_t GetNumLevels() const { return (uint32_t)_levels.size(); }
    const char* GetLevelName(uint32_t level) const { assert(level < _levels.size()); return _levels[level].Name; }

    const Level& GetLevel(uint32_t i) const { return _levels[i]; }
    HRESULT ChangeLevel(uint32_t level);

    void Clear();
    void Draw3DView(const XMFLOAT4X4& view, const XMFLOAT4X4& projection);
    void Present();

    ComPtr<ID3D11Device> GetDevice() const { return _device; }
    ComPtr<ID3D11RenderTargetView> GetRenderTarget() const { return _renderTargetView; }

private:
    Renderer();
    Renderer(const Renderer&);
    Renderer& operator= (const Renderer&);

    HRESULT Initialize(_In_ HWND hwnd, _In_z_ const wchar_t* contentRoot);
    HRESULT InitializeClearScreen(float width, float height);
    HRESULT InitialzeDraw3D();
    HRESULT LoadFile(_In_z_ const wchar_t* filename, _Out_ std::unique_ptr<uint8_t[]>* buffer, _Out_ size_t* size);

    HRESULT LoadWolf3DLevels();
    HRESULT LoadWolf3DTextures();

private:
    std::wstring _contentRoot;
    ComPtr<IDXGISwapChain> _swapChain;
    ComPtr<ID3D11Device> _device;
    ComPtr<ID3D11DeviceContext> _context;
    ComPtr<ID3D11RenderTargetView> _renderTargetView;
    ComPtr<ID3D11DepthStencilView> _depthStencilView;

    //
    // Clearing top/bottom
    //
    ComPtr<ID3D11InputLayout> _inputLayoutClear;
    ComPtr<ID3D11VertexShader> _vertexShaderClear;
    ComPtr<ID3D11PixelShader> _pixelShaderClear;
    ComPtr<ID3D11Buffer> _constantBufferClear;
    ComPtr<ID3D11Buffer> _vertexBufferClear;

    //
    // Draw 3D view
    //
    ComPtr<ID3D11InputLayout> _inputLayout3D;
    ComPtr<ID3D11VertexShader> _vertexShader3D;
    ComPtr<ID3D11PixelShader> _pixelShader3D;
    ComPtr<ID3D11Buffer> _constantBuffer3D;
    ComPtr<ID3D11Buffer> _vertexBuffer3D;
    ComPtr<ID3D11Buffer> _instanceBuffer3D;
    ComPtr<ID3D11Buffer> _indexBuffer3D;
    ComPtr<ID3D11ShaderResourceView> _textures;
    ComPtr<ID3D11SamplerState> _sampler;
    uint32_t _numInstances;

    //
    // Levels
    //
    std::vector<Level> _levels;
};
