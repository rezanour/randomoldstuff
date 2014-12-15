#pragma once

namespace GDK {
namespace Graphics {

    class RuntimeGeometry;
    class RuntimeTexture;

    class DxGraphicsDeviceFactory : public RuntimeObject<IGraphicsDeviceFactory>
    {
        IMPL_GUID(0xce93a5e7, 0x5325, 0x4d52, 0xa1, 0x3e, 0x3, 0x11, 0x96, 0xfa, 0xee, 0x30);
    public:
        // IGraphicsDeviceFactory
        virtual HRESULT GDKAPI CreateGraphicsDevice(_In_ const GraphicsDeviceCreationParameters& parameters, _COM_Outptr_ IGraphicsDevice** device);
    };

    class DxGraphicsDevice : public RuntimeObject<IGraphicsDevice>
    {
    public:
        static const wchar_t* const DisplayName;

        static Microsoft::WRL::ComPtr<DxGraphicsDevice> Create(_In_ const GraphicsDeviceCreationParameters& parameters);
        DxGraphicsDevice(_In_ const GraphicsDeviceCreationParameters& parameters);

        ID3D11Device* GetDevice() { return _device.Get(); }
        ID3D11DeviceContext* GetContext() { return _context.Get(); }

        //
        // IGraphicsDevice
        //

        // Basic device control
        virtual void GDKAPI Clear(_In_ const Vector4& color);
        virtual void GDKAPI ClearDepth(_In_ float depth);
        virtual void GDKAPI Present();

        virtual void GDKAPI SetFillMode(_In_ GraphicsFillMode fillMode);

        // GPU resources
        virtual HRESULT GDKAPI CreateRuntimeGeometry(_In_ GDK::Content::IGeometryResource* data, _COM_Outptr_ IRuntimeGeometry** geometry);
        virtual HRESULT GDKAPI CreateRuntimeTexture(_In_ GDK::Content::ITextureResource* data, _COM_Outptr_ IRuntimeTexture** texture);

        // Configure Rendering Info
        virtual HRESULT GDKAPI ClearBoundResources();
        virtual HRESULT GDKAPI BindTexture(_In_ size_t slot, _In_opt_ IRuntimeTexture* texture);
        virtual HRESULT GDKAPI BindGeometry(_In_opt_ IRuntimeGeometry* geometry);
        virtual HRESULT GDKAPI SetViewProjection(_In_ const Matrix& view, _In_ const Matrix& projection);

        // Render the currently bound geometry
        virtual HRESULT GDKAPI Draw(_In_ const Matrix& world);

        // Render texture 0 as a screen space quad (normalized x, y from 0 -> 1)
        virtual HRESULT GDKAPI Draw2D(_In_ const RECT& destRect);

    private:
        ~DxGraphicsDevice();

        void GDKAPI UpdateVSConstantBuffer();
        void GDKAPI UpdatePSConstantBuffer();

        GraphicsDeviceCreationParameters _parameters;
        D3D_FEATURE_LEVEL _featureLevel;

        Microsoft::WRL::ComPtr<ID3D11Device> _device;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> _context;
        Microsoft::WRL::ComPtr<IDXGISwapChain> _swapChain;
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> _backBuffer;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilView> _depthStencil;
        Microsoft::WRL::ComPtr<ID3D11RasterizerState> _wireframeRasterizerState;

        // Render Info
        Microsoft::WRL::ComPtr<RuntimeGeometry> _boundGeometry;
        Microsoft::WRL::ComPtr<RuntimeTexture> _boundTexture[2];

        Microsoft::WRL::ComPtr<ID3D11InputLayout> _inputLayout;
        Microsoft::WRL::ComPtr<ID3D11VertexShader> _vertexShader;
        Microsoft::WRL::ComPtr<ID3D11PixelShader> _pixelShader;
        Microsoft::WRL::ComPtr<ID3D11Buffer> _vsConstantBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer> _psConstantBuffer;

        struct DirectionalLight
        {
            Vector4 Direction;
            Vector4 Color;
        };

        struct VSConstants
        {
            Matrix World;
            Matrix InvTransWorld;
            Matrix ViewProj;
            float AnimationLerp;
            Vector3 _padding_;
        } _vsConstants;

        struct PSConstants
        {
            DirectionalLight Light1;
            DirectionalLight Light2;
        } _psConstants;

        // Default texture used for untextured geometries
        Microsoft::WRL::ComPtr<RuntimeTexture> _defaultTexture;

        // Quad used for 2D rendering
        Microsoft::WRL::ComPtr<RuntimeGeometry> _quad;
    };

} // Graphics
} // GDK
