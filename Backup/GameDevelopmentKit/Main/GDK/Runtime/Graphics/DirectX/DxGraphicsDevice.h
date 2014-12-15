#pragma once

#include <Platform.h>
#include <GraphicsDevice.h>

#include <d3d11.h>
#include <wrl\client.h>

namespace GDK
{
    using Microsoft::WRL::ComPtr;

    class DxGraphicsDevice : public GraphicsDevice
    {
    public:
        static std::shared_ptr<DxGraphicsDevice> Create(_In_ const GraphicsDevice::CreateParameters& parameters);

        // Basic device control
        virtual void Clear(_In_ const Vector4& color) override;
        virtual void ClearDepth(_In_ float depth) override;
        virtual void Present() override;

        // GPU resources
        virtual std::shared_ptr<Geometry> CreateGeometry(_In_ const std::shared_ptr<GeometryContent>& data) override;
        virtual std::shared_ptr<Texture> CreateTexture(_In_ const std::shared_ptr<TextureContent>& data) override;

        // Configure Rendering Info
        virtual void ClearBoundResources() override;
        virtual void BindTexture(_In_ uint32_t slot, _In_ const std::shared_ptr<Texture>& texture) override;
        virtual void BindGeometry(_In_ const std::shared_ptr<Geometry>& geometry) override;
        virtual void SetViewProjection(_In_ const Matrix& view, _In_ const Matrix& projection) override;

        // Render the currently bound geometry
        virtual void Draw(_In_ const Matrix& world) override;

        // Render texture 0 as a screen space quad (normalized x, y from 0 -> 1)
        virtual void Draw2D(_In_ const RectangleF& destRect) override;

        ID3D11Device* GetDevice() { return _device.Get(); }
        ID3D11DeviceContext* GetContext() { return _context.Get(); }

    private:
        DxGraphicsDevice(_In_ const GraphicsDevice::CreateParameters& parameters);

        void UpdateVSConstantBuffer();
        void UpdatePSConstantBuffer();

        void CreateDefaultTexture();
        void CreateQuad();

        D3D_FEATURE_LEVEL _featureLevel;

        ComPtr<ID3D11Device> _device;
        ComPtr<ID3D11DeviceContext> _context;
        ComPtr<IDXGISwapChain> _swapChain;
        ComPtr<ID3D11RenderTargetView> _backBufferView;
        ComPtr<ID3D11DepthStencilView> _depthBufferView;

        // Render Info
        std::shared_ptr<Geometry> _boundGeometry;
        std::shared_ptr<Texture> _boundTextures[2];

        ComPtr<ID3D11InputLayout> _inputLayout;
        ComPtr<ID3D11VertexShader> _vertexShader;
        ComPtr<ID3D11PixelShader> _pixelShader;
        ComPtr<ID3D11Buffer> _vsConstantBuffer;
        ComPtr<ID3D11Buffer> _psConstantBuffer;

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
        std::shared_ptr<Texture> _defaultTexture;

        // Quad used for 2D rendering
        std::shared_ptr<Geometry> _quad;
    };
}
