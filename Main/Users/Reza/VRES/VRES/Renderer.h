#pragma once

namespace VRES
{
    class HMD;
    class Scene;
    class Model;
    class Material;

    struct RendererConfig
    {
        void*   Window;
        Size    Resolution;
        bool    CreateDebugDevice;
    };

    struct OutputDescription
    {
        D3D11_VIEWPORT  Viewport;
        XMFLOAT3        CameraPosition; // Center Eye, not per Eye
        XMFLOAT4X4      View;
        XMFLOAT4X4      Projection;
    };

    class Renderer
    {
    public:
        static std::unique_ptr<Renderer> Create(const RendererConfig& config);
        ~Renderer();

        ComPtr<ID3D11Device>& Device() { return _device; }
        ComPtr<ID3D11DeviceContext>& Context() { return _context; }
        ComPtr<ID3D11RenderTargetView>& BackBuffer() { return _renderTargetView; }
        ComPtr<IDXGISwapChain>& SwapChain() { return _swapChain; }

        //
        // Basic
        //
        void ClearBackBuffer(const float clearColor[4]);
        void ClearDepth(float depth);
        bool EndFrame();

        //
        // Resource creation
        //
        std::shared_ptr<Model> CreateCube(const XMFLOAT3& halfWidths);
        std::shared_ptr<Model> LoadModel(_In_z_ const wchar_t* filename);

        ComPtr<ID3D11ShaderResourceView> LoadTexture(_In_z_ const wchar_t* filename);

        //
        // Rendering
        //
        void RenderScene(const std::shared_ptr<Scene>& scene, const Transform& camera, const std::shared_ptr<HMD>& hmd);

        //
        // Materials
        //
        // Gives ownership of the material object to the Renderer, which will delete it later
        void AddMaterial(std::shared_ptr<Material>& material);
        std::shared_ptr<Material> GetMaterial(const std::string& name);

        //
        // Built-in objects
        //
        ComPtr<ID3D11SamplerState>& LinearWrapSampler() { return _linearWrapSampler; }
        ComPtr<ID3D11SamplerState>& PointClampSampler() { return _pointClampSampler; }

    private:
        Renderer();

        bool Initialize(const RendererConfig& config);

    private:
        ComPtr<ID3D11Device> _device;
        ComPtr<IDXGISwapChain> _swapChain;
        ComPtr<ID3D11DeviceContext> _context;
        ComPtr<ID3D11RenderTargetView> _renderTargetView;
        ComPtr<ID3D11DepthStencilView> _depthStencilView;

        // Built-in objects
        ComPtr<ID3D11SamplerState> _linearWrapSampler;
        ComPtr<ID3D11SamplerState> _pointClampSampler;

        std::map<std::string, std::shared_ptr<Material>> _materials;
    };
}
