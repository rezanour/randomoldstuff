#include "DxGraphicsDevice.h"
#include "DxTexture.h"
#include "DxGeometry.h"

#include <GDKError.h>
#include <wrl\wrappers\corewrappers.h>

namespace GDK
{
    using Microsoft::WRL::ComPtr;
    using Microsoft::WRL::Wrappers::FileHandle;

    std::shared_ptr<DxGraphicsDevice> DxGraphicsDevice::Create(_In_ const GraphicsDevice::CreateParameters& parameters)
    {
        // because some initialization can't be done via the constructor (requires shared_from_this), we do it in two steps
        return std::shared_ptr<DxGraphicsDevice>(GDKNEW DxGraphicsDevice(parameters));
    }

    DxGraphicsDevice::DxGraphicsDevice(_In_ const GraphicsDevice::CreateParameters& parameters) :
        GraphicsDeviceBase(parameters)
    {
    }

    void DxGraphicsDevice::Clear(_In_ const Vector4& color)
    {
        _context->ClearRenderTargetView(_backBufferView.Get(), reinterpret_cast<const float*>(&color.x));
    }

    void DxGraphicsDevice::ClearDepth(_In_ float depth)
    {
        _context->ClearDepthStencilView(_depthBufferView.Get(), D3D10_CLEAR_DEPTH, depth, 0);
    }

    void DxGraphicsDevice::Present()
    {
        _swapChain->Present(_parameters.vsync ? 1 : 0, 0);
    }

    void DxGraphicsDevice::EnableZBuffer(_In_ bool enabled)
    {
        if (enabled)
        {
            _context->OMSetRenderTargets(1, _backBufferView.GetAddressOf(), _depthBufferView.Get());
            _context->RSSetState(nullptr);
        }
        else
        {
            _context->OMSetRenderTargets(1, _backBufferView.GetAddressOf(), nullptr);
            _context->RSSetState(_depthDisabledState.Get());
        }
    }

    std::shared_ptr<Geometry> DxGraphicsDevice::CreateGeometry(_In_ const std::shared_ptr<GeometryContent>& data)
    {
        return DxGeometry::Create(shared_from_this(), data);
    }

    std::shared_ptr<Texture> DxGraphicsDevice::CreateTexture(_In_ const std::shared_ptr<TextureContent>& data)
    {
        return DxTexture::Create(shared_from_this(), data);
    }

    void DxGraphicsDevice::InitializeDevice()
    {
        DXGI_SWAP_CHAIN_DESC scd = {};

        scd.OutputWindow = static_cast<HWND>(_parameters.windowIdentity);
        scd.Windowed = TRUE;
        scd.BufferCount = scd.Windowed ? 1 : 2;
        scd.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
        scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        scd.BufferDesc.Width = _parameters.backBufferWidth;
        scd.BufferDesc.Height = _parameters.backBufferHeight;
        scd.BufferDesc.RefreshRate.Numerator = 60;
        scd.BufferDesc.RefreshRate.Denominator = 1;
        scd.SampleDesc.Count = 1;
        scd.SampleDesc.Quality = 0;

        static const D3D_FEATURE_LEVEL featureLevels[] =
        {
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
        };

        D3D_FEATURE_LEVEL featureLevel;

        DWORD flags = 0;

    #if GDKDEBUG
        flags |= D3D11_CREATE_DEVICE_DEBUG;
    #endif

        CHECK_HR(D3D11CreateDeviceAndSwapChain(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            flags,
            featureLevels,
            _countof(featureLevels),
            D3D11_SDK_VERSION,
            &scd,
            &_swapChain,
            &_device,
            &featureLevel,
            &_context));

        ComPtr<ID3D11Texture2D> texture;

        // Get backbuffer
        CHECK_HR(_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &texture));
        CHECK_HR(_device->CreateRenderTargetView(texture.Get(), nullptr, &_backBufferView));

        // Setup the viewport
        D3D11_VIEWPORT vp = {};
        vp.Width = static_cast<float>(_parameters.backBufferWidth),
        vp.Height = static_cast<float>(_parameters.backBufferHeight);
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        vp.TopLeftX = 0;
        vp.TopLeftY = 0;

        _context->RSSetViewports(1, &vp);

        D3D11_TEXTURE2D_DESC texDesc = {};
        D3D11_DEPTH_STENCIL_VIEW_DESC depthDesc = {};

        // Create our depth & stencil buffer
        texDesc.Format = depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        texDesc.Width = _parameters.backBufferWidth;
        texDesc.Height = _parameters.backBufferHeight;
        texDesc.ArraySize = 1;
        texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        texDesc.MipLevels = 1;
        texDesc.Usage = D3D11_USAGE_DEFAULT;
        texDesc.SampleDesc.Count = scd.SampleDesc.Count;
        texDesc.SampleDesc.Quality = scd.SampleDesc.Quality;

        depthDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;

        CHECK_HR(_device->CreateTexture2D(&texDesc, nullptr, texture.ReleaseAndGetAddressOf()));
        CHECK_HR(_device->CreateDepthStencilView(texture.Get(), &depthDesc, &_depthBufferView));

        _context->OMSetRenderTargets(1, _backBufferView.GetAddressOf(), _depthBufferView.Get());

        D3D11_RASTERIZER_DESC rasterizerDesc = {};
        rasterizerDesc.CullMode = D3D11_CULL_BACK;
        rasterizerDesc.DepthClipEnable = FALSE;
        rasterizerDesc.FillMode = D3D11_FILL_SOLID;

        _device->CreateRasterizerState(&rasterizerDesc, &_depthDisabledState);
    }

    std::shared_ptr<Texture> DxGraphicsDevice::CreateDefaultTexture(_In_ uint32_t width, _In_ uint32_t height, _In_ TextureFormat format, _In_ const byte_t* pixels)
    {
        return DxTexture::Create(shared_from_this(), width, height, format, pixels);
    }

    std::shared_ptr<Geometry> DxGraphicsDevice::CreateQuad2D(_In_ uint32_t numVertices, _In_ const Geometry::Vertex2D* vertices, _In_ uint32_t numIndices, _In_ const uint32_t* indices)
    {
        return DxGeometry::Create(shared_from_this(), Geometry::Type::Geometry2D, 1, vertices, numVertices, indices, numIndices);
    }

    std::shared_ptr<Geometry> DxGraphicsDevice::CreateDynamicGeometry(_In_ uint32_t numVertices)
    {
        std::unique_ptr<Geometry::Vertex3D[]> vertices(new Geometry::Vertex3D[numVertices]);
        std::vector<uint32_t> indices;
        for (uint32_t i = 0; i < numVertices; ++i) indices.push_back(i);

        return DxGeometry::Create(shared_from_this(), Geometry::Type::Geometry3D, 1, vertices.get(), numVertices, indices.data(), static_cast<uint32_t>(indices.size()));
    }
}
