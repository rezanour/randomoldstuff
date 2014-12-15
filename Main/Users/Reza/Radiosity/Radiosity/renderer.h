#pragma once

#include "lightpatch.h"

class Camera;

class Renderer : public std::enable_shared_from_this<Renderer>
{
public:
    Renderer(_In_ HWND target, _In_ uint32_t backBufferWidth, _In_ uint32_t backBufferHeight);

    void Render();

private:
    void CreateDeviceResources(_In_ HWND target, _In_ uint32_t backBufferWidth, _In_ uint32_t backBufferHeight);

    std::unique_ptr<BYTE[]> LoadShader(const wchar_t* filename, _Out_ DWORD* numBytes);
    void LoadShaders();

    void LoadTexture();
    void LoadScene();

    void BindPatchPipeline();
    void BindRenderPipeline();

    void Clear();
    void Present();

    void DrawScene();

private:
    friend class PatchManager;

    Microsoft::WRL::ComPtr<ID3D11Device> _device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> _context;
    Microsoft::WRL::ComPtr<IDXGISwapChain> _swapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> _backBuffer;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> _depthStencil;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> _vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> _pixelShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> _patchPixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> _inputLayout;
    Microsoft::WRL::ComPtr<ID3D11Buffer> _vertices;
    Microsoft::WRL::ComPtr<ID3D11Buffer> _indices;
    Microsoft::WRL::ComPtr<ID3D11Buffer> _vertexShaderConstantBuffer;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> _pointSampler;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> _linearSampler;
    uint32_t _wallsIndexOffset;
    uint32_t _wallsIndexCount;
    uint32_t _cubeIndexOffset;
    uint32_t _cubeIndexCount;

    struct VertexShaderConstants
    {
        DirectX::XMFLOAT4X4 World;
        DirectX::XMFLOAT4X4 ViewProjection;
    };
    VertexShaderConstants _vertexShaderConstants;

    std::shared_ptr<Camera> _camera;

    DirectX::XMFLOAT4X4 _wallsWorld;
    DirectX::XMFLOAT4X4 _cubeWorld;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> _lightMap;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _lightMapSRV;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> _diffuseMap;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _diffuseSRV;
    std::unique_ptr<PatchManager> _patchManager;
};
