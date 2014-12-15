#pragma once

class GBuffer;
class Object;
class Camera;
class Scene;
class Texture2D;
class DirectionalLighting;
class PointLighting;
class DirectionalLightShadows;
class PointLightShadows;
class HighPassFilter;
class Luminance;
class ToneMapping;
class Combine;

class Renderer
{
public:
    Renderer(_In_ HWND target, _In_ uint32_t backBufferWidth, _In_ uint32_t backBufferHeight);

    void RenderFrame();

private:
    void CreateDeviceResources(_In_ HWND target, _In_ uint32_t backBufferWidth, _In_ uint32_t backBufferHeight);

    void Clear();
    void Present();

private:
    Microsoft::WRL::ComPtr<ID3D11Device> _device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> _context;
    Microsoft::WRL::ComPtr<IDXGISwapChain> _swapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> _backBuffer;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> _depthStencil;

    std::shared_ptr<Texture2D> _diffuse;
    std::shared_ptr<Texture2D> _normals;
    std::shared_ptr<Texture2D> _depth;
    std::shared_ptr<Texture2D> _light;
    std::shared_ptr<Texture2D> _lightHighPass;
    std::shared_ptr<Texture2D> _lightCombined;
    std::shared_ptr<Texture2D> _lightLuminance;
    std::shared_ptr<Texture2D> _tonemapped;
    std::shared_ptr<Texture2D> _combined;

    std::shared_ptr<GBuffer> _gbuffer;
    std::shared_ptr<DirectionalLightShadows> _directionalShadows;
    std::shared_ptr<PointLightShadows> _pointShadows;
    std::shared_ptr<DirectionalLighting> _lighting;
    std::shared_ptr<PointLighting> _pointLighting;
    std::shared_ptr<HighPassFilter> _highPass;
    std::shared_ptr<Combine> _lightingCombine;
    std::shared_ptr<Luminance> _luminance;
    std::shared_ptr<ToneMapping> _tonemapping;
    std::shared_ptr<Combine> _combine;

    std::shared_ptr<Camera> _camera;
    std::shared_ptr<Scene> _scene;
    std::shared_ptr<Object> _blade1;
    std::shared_ptr<Object> _blade2;
};
