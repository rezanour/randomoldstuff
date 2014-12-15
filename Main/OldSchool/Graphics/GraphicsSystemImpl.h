#pragma once

#include <GraphicsSystem.h>

class Texture;
class Renderer2D;
class Renderer3D;

class GraphicsSystem : public BaseObject<GraphicsSystem>, public IGraphicsSystem, public OVR::MessageHandler
{
public:
    static std::shared_ptr<GraphicsSystem> Create(_In_ const GraphicsSystemConfig& config);

    //
    // IGraphicsSystem
    //

    const GraphicsSystemConfig& GetConfig() const override;

    std::shared_ptr<ITexture> CreateTexture(_In_ uint32_t width, _In_ uint32_t height, _In_ DXGI_FORMAT format, _In_opt_ const void* pixels, _In_ uint32_t pitch, _In_ bool generateMips) override;

    std::shared_ptr<IGeometry> CreateGeometry(_In_ uint32_t numVertices, _In_ VertexFormat format, _In_ const void* vertices, _In_ uint32_t numIndices, _In_ const uint32_t* indices) override;

    std::shared_ptr<IModel> CreateModel(_In_opt_ const std::shared_ptr<IReadonlyObject>& owner) override;

    std::shared_ptr<ILight> CreateLight(_In_ LightType type, _In_ const XMFLOAT3& color, _In_ float radius) override;

    std::shared_ptr<IGraphicsScene> CreateScene() override;

    void DrawScene(_In_ const std::shared_ptr<IGraphicsScene>& scene, _In_ const XMFLOAT4X4& view, _In_ const XMFLOAT4X4& projection) override;

    void DrawImage(_In_ const std::shared_ptr<ITexture>& texture, _In_ int32_t x, _In_ int32_t y) override;
    void DrawImage(_In_ const std::shared_ptr<ITexture>& texture, _In_ int32_t x, _In_ int32_t y, _In_ uint32_t width, _In_ uint32_t height) override;
    void DrawImage(_In_ const std::shared_ptr<ITexture>& texture, _In_ const RECT& source, _In_ const RECT& dest) override;

    void RenderFrame() override;

    bool IsStereoRenderingSupported() const override;
    void EnableStereoRendering(_In_ bool enable) override;
    bool IsStereoRenderingEnabled() const override;
    XMVECTOR GetHeadOrientation() const override;

    //
    // OVR::MessageHandler
    //
    void OnMessage(_In_ const OVR::Message& message) override;
    bool SupportsMessageType(_In_ OVR::MessageType messageType) const override;

    //
    // Internal
    //

    // Shaders
    ComPtr<ID3D11VertexShader> LoadVertexShader(_In_ const ComPtr<ID3D11Device>& device, _In_z_ const wchar_t* filename, _Out_ std::unique_ptr<uint8_t[]>& data, _Out_ size_t* size);
    ComPtr<ID3D11PixelShader> LoadPixelShader(_In_ const ComPtr<ID3D11Device>& device, _In_z_ const wchar_t* filename);
    void GetVertexFormatElements(_In_ VertexFormat format, _Out_ std::vector<D3D11_INPUT_ELEMENT_DESC>& elements);

    // Samplers
    const ComPtr<ID3D11SamplerState>& GetPointClampSampler();
    const ComPtr<ID3D11SamplerState>& GetLinearClampSampler();
    const ComPtr<ID3D11SamplerState>& GetPointWrapSampler();
    const ComPtr<ID3D11SamplerState>& GetLinearWrapSampler();

    // Blend States
    const ComPtr<ID3D11BlendState>& GetAlphaBlendState();
    const ComPtr<ID3D11BlendState>& GetAdditiveBlendState();
    const ComPtr<ID3D11BlendState>& GetPremultipliedAlphaBlendState();

    // Depth States
    const ComPtr<ID3D11DepthStencilState>& GetDepthReadNoWriteState();

private:
    GraphicsSystem(_In_ const GraphicsSystemConfig& config);

    void InitializeOculusSupport();
    void HandleVRDeviceAdded(_In_ OVR::DeviceBase* device);
    void HandleVRDeviceRemoved(_In_ OVR::DeviceBase* device);

    bool VRAvailable() const;
    void EnableVROutput(_In_ bool enable);

    void SetOutput(_In_ const ComPtr<IDXGIOutput>& output);
    void SetViewport(_In_ const D3D11_VIEWPORT& viewport);

private:
    ComPtr<ID3D11Device> _device;
    ComPtr<ID3D11DeviceContext> _context;
    ComPtr<IDXGISwapChain> _swapChain;
    ComPtr<ID3D11RenderTargetView> _frameBuffer;
    std::shared_ptr<Texture> _depthBuffer;

    GraphicsSystemConfig _config;

    // Renderers
    std::shared_ptr<Renderer2D> _renderer2D;
    std::shared_ptr<Renderer3D> _renderer3D;

    // Oculus support
    OVR::System _vrSystem;
    OVR::Ptr<OVR::DeviceManager> _vrDeviceManager;
    OVR::Ptr<OVR::HMDDevice> _vrDevice;
    OVR::Ptr<OVR::SensorDevice> _vrSensor;
    OVR::SensorFusion _vrSensorFusion;
    OVR::HMDInfo _hmdInfo;

    bool _enableStereo;

    //
    // Shared, global states
    //
    ComPtr<ID3D11SamplerState> _pointClampSampler;
    ComPtr<ID3D11SamplerState> _linearClampSampler;
    ComPtr<ID3D11SamplerState> _pointWrapSampler;
    ComPtr<ID3D11SamplerState> _linearWrapSampler;

    ComPtr<ID3D11BlendState> _alphaBlendState;
    ComPtr<ID3D11BlendState> _additiveBlendState;
    ComPtr<ID3D11BlendState> _premulAlphaBlendState;

    ComPtr<ID3D11DepthStencilState> _depthReadNoWrite;
};
