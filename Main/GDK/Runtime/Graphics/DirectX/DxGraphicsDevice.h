#pragma once

#include "..\GraphicsDeviceBase.h"

#include <d3d11.h>
#include <wrl\client.h>

namespace GDK
{
    using Microsoft::WRL::ComPtr;

    class DxGraphicsDevice : public GraphicsDeviceBase
    {
    public:
        static std::shared_ptr<DxGraphicsDevice> Create(_In_ const GraphicsDevice::CreateParameters& parameters);

        // Basic device control
        virtual void Clear(_In_ const Vector4& color) override;
        virtual void ClearDepth(_In_ float depth) override;
        virtual void Present() override;

        virtual void EnableZBuffer(_In_ bool enabled) override;

        // GPU resources
        virtual std::shared_ptr<Geometry> CreateGeometry(_In_ const std::shared_ptr<GeometryContent>& data) override;
        virtual std::shared_ptr<Texture> CreateTexture(_In_ const std::shared_ptr<TextureContent>& data) override;

        ID3D11Device* GetDevice() { return _device.Get(); }
        ID3D11DeviceContext* GetContext() { return _context.Get(); }

        virtual std::shared_ptr<Geometry> CreateDynamicGeometry(_In_ uint32_t numVertices) override;


    private:
        DxGraphicsDevice(_In_ const GraphicsDevice::CreateParameters& parameters);

        virtual void InitializeDevice() override;
        virtual std::shared_ptr<Texture> CreateDefaultTexture(_In_ uint32_t width, _In_ uint32_t height, _In_ TextureFormat format, _In_ const byte_t* pixels) override;
        virtual std::shared_ptr<Geometry> CreateQuad2D(_In_ uint32_t numVertices, _In_ const Geometry::Vertex2D* vertices, _In_ uint32_t numIndices, _In_ const uint32_t* indices) override;

        ComPtr<ID3D11Device> _device;
        ComPtr<ID3D11DeviceContext> _context;
        ComPtr<IDXGISwapChain> _swapChain;
        ComPtr<ID3D11RenderTargetView> _backBufferView;
        ComPtr<ID3D11DepthStencilView> _depthBufferView;
        ComPtr<ID3D11RasterizerState> _depthDisabledState;
    };
}
