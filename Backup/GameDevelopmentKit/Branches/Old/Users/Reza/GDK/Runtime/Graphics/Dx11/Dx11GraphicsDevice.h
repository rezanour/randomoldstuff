#pragma once

#include "GraphicsDevice.h"
#include <d3d11.h>
#include <wrl\client.h>

namespace GDK
{
    namespace Graphics
    {
        using Microsoft::WRL::ComPtr;

        class Dx11GraphicsDevice : public GraphicsDevice
        {
        public:
            Dx11GraphicsDevice(_In_ const GraphicsDeviceCreationParameters& parameters);

            // Basic device control
            void Clear(_In_ const Vector4& color) override;
            void ClearDepth(_In_ float depth) override;
            void Present() override;

            // GPU resources
            std::shared_ptr<Geometry> CreateGeometry(_In_ const std::shared_ptr<GeometryContent>& data) override;
            std::shared_ptr<Texture> CreateTexture(_In_ const std::shared_ptr<TextureContent>& data) override;

            // Configure Rendering Info
            void ClearBoundResources() override;
            void BindGeometry(_In_ const std::shared_ptr<Geometry>& geometry) override;
            void BindTexture(_In_ size_t slot, _In_ const std::shared_ptr<Texture>& texture) override;
            void SetViewProjection(_In_ const Matrix& view, _In_ const Matrix& projection) override;

            // Render the currently bound geometry
            void Draw(_In_ const Matrix& world) override;

            // Render texture as a screen space quad (normalized x, y from 0 -> 1)
            void Draw2D(_In_ size_t slot, _In_ const RectangleF& destRect) override;

        private:
            ComPtr<ID3D11Device> _device;
            ComPtr<ID3D11DeviceContext> _context;
            ComPtr<IDXGISwapChain> _swapChain;
            ComPtr<ID3D11RenderTargetView> _backBufferView;
            ComPtr<ID3D11DepthStencilView> _depthBufferView;

            // Render Info
            std::shared_ptr<Geometry> _boundGeometry;
            std::shared_ptr<Texture> _boundTexture[2];

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
}
