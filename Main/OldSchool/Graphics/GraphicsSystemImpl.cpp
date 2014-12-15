#include <Precomp.h>
#include "Renderer2D.h"
#include "Renderer3D.h"
#include "GeometryImpl.h"
#include "TextureImpl.h"
#include "ModelImpl.h"
#include "LightImpl.h"
#include "GraphicsSystemImpl.h"
#include "GraphicsSceneImpl.h"

using namespace OVR;

_Use_decl_annotations_
std::shared_ptr<IGraphicsSystem> CreateGraphicsSystem(const GraphicsSystemConfig& config)
{
    return GraphicsSystem::Create(config);
}

_Use_decl_annotations_
std::shared_ptr<GraphicsSystem> GraphicsSystem::Create(const GraphicsSystemConfig& config)
{
    std::shared_ptr<GraphicsSystem> system(new GraphicsSystem(config));

    // Create renderers
    system->_renderer2D = Renderer2D::Create(system, system->_context, system->_frameBuffer, system->_depthBuffer->GetDepthStencilView());
    system->_renderer3D = Renderer3D::Create(system, system->_context, system->_frameBuffer, system->_depthBuffer);

    return system;
}

_Use_decl_annotations_
GraphicsSystem::GraphicsSystem(const GraphicsSystemConfig& config) :
    _config(config), _vrSystem(Log::ConfigureDefaultLog(LogMask_All))
{
    assert(config.Window != nullptr);
    assert(config.Width > 0 && config.Height > 0);

    uint32_t flags = 0;

#ifndef NDEBUG
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    DXGI_SWAP_CHAIN_DESC swd = {};
    swd.BufferCount = 2;
    swd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swd.BufferDesc.Width = config.Width;
    swd.BufferDesc.Height = config.Height;
    swd.BufferDesc.RefreshRate.Numerator = 60;
    swd.BufferDesc.RefreshRate.Denominator = 1;
    swd.BufferDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
    swd.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swd.OutputWindow = config.Window;
    swd.SampleDesc.Count = 1;
    swd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swd.Windowed = TRUE;

    D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

    ComPtr<IDXGIFactory> factory;
    HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&factory));
    if (FAILED(hr))
    {
        throw std::exception();
    }

    // Enumerate and try creating 11.0 device with each adapter on the system until we find one that supports it, or run out of adapters
    uint32_t index = 0;
    while (SUCCEEDED(hr))
    {
        ComPtr<IDXGIAdapter> adapter;
        hr = factory->EnumAdapters(index++, &adapter);
        if (SUCCEEDED(hr))
        {
            DXGI_ADAPTER_DESC desc = {};
            adapter->GetDesc(&desc);

            hr = D3D11CreateDeviceAndSwapChain(adapter.Get(), D3D_DRIVER_TYPE_UNKNOWN, nullptr, flags,
                featureLevels, _countof(featureLevels), D3D11_SDK_VERSION, &swd, &_swapChain, &_device, &featureLevel, &_context);
            if (SUCCEEDED(hr))
            {
                // Found a suitable adapter and successfully created device
                break;
            }
            else
            {
                hr = S_OK;  // run another iteration
            }
        }
        // no more adapters, let it fall out of the loop
    }
    if (FAILED(hr))
    {
        throw std::exception();
    }

    ComPtr<ID3D11Resource> backBuffer;
    hr = _swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
    if (FAILED(hr))
    {
        throw std::exception();
    }

    hr = _device->CreateRenderTargetView(backBuffer.Get(), nullptr, &_frameBuffer);
    if (FAILED(hr))
    {
        throw std::exception();
    }

    _depthBuffer = Texture::CreateDepthTexture(_device, config.Width, config.Height, DXGI_FORMAT_R32_TYPELESS, DXGI_FORMAT_D32_FLOAT, true, DXGI_FORMAT_R32_FLOAT);

    InitializeOculusSupport();
}

const GraphicsSystemConfig& GraphicsSystem::GetConfig() const
{
    return _config;
}

_Use_decl_annotations_
std::shared_ptr<ITexture> GraphicsSystem::CreateTexture(uint32_t width, uint32_t height, DXGI_FORMAT format, const void* pixels, uint32_t pitch, bool generateMips)
{
    return Texture::CreateTexture(_device, width, height, format, pixels, pitch, generateMips, false);
}

_Use_decl_annotations_
std::shared_ptr<IGeometry> GraphicsSystem::CreateGeometry(uint32_t numVertices, VertexFormat format, const void* vertices, uint32_t numIndices, const uint32_t* indices)
{
    return Geometry::Create(_device, numVertices, format, vertices, numIndices, indices);
}

_Use_decl_annotations_
std::shared_ptr<IModel> GraphicsSystem::CreateModel(const std::shared_ptr<IReadonlyObject>& owner)
{
    return Model::Create(owner);
}

_Use_decl_annotations_
std::shared_ptr<ILight> GraphicsSystem::CreateLight(LightType type, const XMFLOAT3& color, float radius)
{
    return Light::Create(type, color, radius);
}

std::shared_ptr<IGraphicsScene> GraphicsSystem::CreateScene()
{
    return GraphicsScene::Create();
}

_Use_decl_annotations_
void GraphicsSystem::DrawScene(const std::shared_ptr<IGraphicsScene>& scene, const XMFLOAT4X4& view, const XMFLOAT4X4& projection)
{
    if (VRAvailable() && _enableStereo)
    {
        static const float metersToFeet = 3.2808399f; // to translate from meters to feet, since 1 unit of world translation in our game is 1 ft.

        // TODO: Compute these 1 time on device change and store off
        float aspectRatio = _hmdInfo.HResolution * 0.5f / _hmdInfo.VResolution;
        float halfScreenDistance = _hmdInfo.VScreenSize * 0.5f;
        float yfov = atanf(halfScreenDistance / _hmdInfo.EyeToScreenDistance);

        float viewCenter = _hmdInfo.HScreenSize * 0.25f;
        float eyeProjectionShift = viewCenter - _hmdInfo.LensSeparationDistance * 0.5f;
        float projectionCenterOffset = 4.0f * eyeProjectionShift / (_hmdInfo.HScreenSize);

        XMMATRIX proj = XMMatrixPerspectiveFovLH(aspectRatio, yfov, 0.3f, 1000.0f);
        XMMATRIX projLeft = XMMatrixMultiply(proj, XMMatrixTranslation(projectionCenterOffset, 0, 0));
        XMMATRIX projRight = XMMatrixMultiply(proj, XMMatrixTranslation(-projectionCenterOffset, 0, 0));

        float halfIPD = _hmdInfo.InterpupillaryDistance * 0.5f * metersToFeet;

        XMMATRIX viewLeft;
        XMMATRIX viewRight;

        XMMATRIX viewM = XMLoadFloat4x4(&view);
        viewLeft = XMMatrixMultiply(viewM, XMMatrixTranslation(halfIPD, 0, 0));
        viewRight = XMMatrixMultiply(viewM, XMMatrixTranslation(-halfIPD, 0, 0));

        XMFLOAT4X4 v, p;
        D3D11_VIEWPORT vp = {};

        vp.MaxDepth = 1.0f;
        vp.Width = static_cast<float>(_config.Width / 2);
        vp.Height = static_cast<float>(_config.Height);
        SetViewport(vp);

        XMStoreFloat4x4(&v, viewLeft);
        XMStoreFloat4x4(&p, projLeft);
        _renderer3D->DrawScene(scene, v, p);

        vp.TopLeftX = vp.Width;
        SetViewport(vp);

        XMStoreFloat4x4(&v, viewRight);
        XMStoreFloat4x4(&p, projRight);
        _renderer3D->DrawScene(scene, v, p);

        vp.TopLeftX = 0.0f;
        vp.Width = static_cast<float>(_config.Width);
        SetViewport(vp);
    }
    else
    {
        _renderer3D->DrawScene(scene, view, projection);
    }
}

_Use_decl_annotations_
void GraphicsSystem::DrawImage(const std::shared_ptr<ITexture>& texture, int32_t x, int32_t y)
{
    DrawImage(texture, x, y, texture->GetWidth(), texture->GetHeight());
}

_Use_decl_annotations_
void GraphicsSystem::DrawImage(const std::shared_ptr<ITexture>& texture, int32_t x, int32_t y, uint32_t width, uint32_t height)
{
    _renderer2D->DrawImage(texture, x, y, width, height);
}

_Use_decl_annotations_
void GraphicsSystem::DrawImage(const std::shared_ptr<ITexture>& texture, const RECT& source, const RECT& dest)
{
    _renderer2D->DrawImage(texture, source, dest);
}

void GraphicsSystem::RenderFrame()
{
    static const float clearColor[] = { 0.2f, 0.4f, 0.6f, 1 };
    _context->ClearRenderTargetView(_frameBuffer.Get(), clearColor);

    // Draw 3D content
    _renderer3D->Render();

    // Draw 2D content
    _renderer2D->Render();

    ProfileMark profilePresent(CodeTag::Present);
    _swapChain->Present(_config.VSync ? 1 : 0, 0);
}

bool GraphicsSystem::IsStereoRenderingSupported() const
{
    return VRAvailable();
}

_Use_decl_annotations_
void GraphicsSystem::EnableStereoRendering(bool enable)
{
    _enableStereo = enable;
    if (VRAvailable())
    {
        EnableVROutput(_enableStereo);
    }
}

bool GraphicsSystem::IsStereoRenderingEnabled() const
{
    return VRAvailable() && _enableStereo;
}

XMVECTOR GraphicsSystem::GetHeadOrientation() const
{
    if (VRAvailable() && _enableStereo)
    {
        Quatf orientation = _vrSensorFusion.GetOrientation();
        return XMVectorSet(orientation.x, orientation.y, -orientation.z, -orientation.w);
    }
    else
    {
        return XMQuaternionIdentity();
    }
}

_Use_decl_annotations_
void GraphicsSystem::OnMessage(const Message& message)
{
    switch (message.Type)
    {
    case MessageType::Message_DeviceAdded:
        HandleVRDeviceAdded(message.pDevice);
        break;

    case MessageType::Message_DeviceRemoved:
        HandleVRDeviceRemoved(message.pDevice);
        break;
    }

    // In all cases, thunk to base class for default handling
    MessageHandler::OnMessage(message);
}

_Use_decl_annotations_
bool GraphicsSystem::SupportsMessageType(MessageType messageType) const
{
    UNREFERENCED_PARAMETER(messageType);
    return true;
}

void GraphicsSystem::InitializeOculusSupport()
{
    _vrDeviceManager = *DeviceManager::Create();
    if (_vrDeviceManager == nullptr)
    {
        throw std::exception();
    }

    _vrDeviceManager->SetMessageHandler(this);

    Ptr<HMDDevice> device = *_vrDeviceManager->EnumerateDevices<HMDDevice>().CreateDevice();
    if (device != nullptr)
    {
        // Device initially connected. Just leverage our device added handler, which does all the init
        HandleVRDeviceAdded(device.GetPtr());
    }
}

_Use_decl_annotations_
void GraphicsSystem::HandleVRDeviceAdded(OVR::DeviceBase* device)
{
    if (_vrDevice != device && device != nullptr)
    {
        DeviceInfo deviceInfo;
        if (!device->GetDeviceInfo(&deviceInfo))
        {
            throw std::exception();
        }

        if (deviceInfo.Type == DeviceType::Device_HMD)
        {
            OutputDebugString(L"Oculus Rift detected. Initializing...\n");

            _vrDevice = static_cast<HMDDevice*>(device);

            _vrSensor = *_vrDevice->GetSensor();
            if (_vrSensor != nullptr)
            {
                _vrSensorFusion.AttachToSensor(_vrSensor);
            }

            _vrDevice->GetDeviceInfo(&_hmdInfo);

            wchar_t message[100] = {};
            HRESULT hr = StringCchPrintf(message, _countof(message), L"Profile \"%S\" Loaded.\n", _vrDevice->GetProfileName());
            if (FAILED(hr))
            {
                throw std::exception();
            }
            OutputDebugString(message);

            if (_enableStereo)
            {
                EnableVROutput(true);
            }
        }
    }
}

_Use_decl_annotations_
void GraphicsSystem::HandleVRDeviceRemoved(OVR::DeviceBase* device)
{
    if (device == _vrDevice && device != nullptr)
    {
        OutputDebugString(L"Oculus disconnected.\n");
        _vrDevice = nullptr;

        if (_enableStereo)
        {
            EnableVROutput(false);
        }
    }
}

bool GraphicsSystem::VRAvailable() const
{
    return (_vrDevice != nullptr) && _vrSensorFusion.IsAttachedToSensor();
}

_Use_decl_annotations_
void GraphicsSystem::EnableVROutput(bool enable)
{
    if (enable)
    {
        assert(VRAvailable());

        std::wstring displayName = std::AnsiToWide(_hmdInfo.DisplayDeviceName);

        ComPtr<IDXGIFactory> factory;
        HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&factory));
        if (FAILED(hr))
        {
            throw std::exception();
        }

        uint32_t index = 0;
        ComPtr<IDXGIAdapter> adapter;
        while (factory->EnumAdapters(index++, &adapter) != DXGI_ERROR_NOT_FOUND)
        {
            uint32_t outputIndex = 0;
            ComPtr<IDXGIOutput> output;

            while (adapter->EnumOutputs(outputIndex++, &output) != DXGI_ERROR_NOT_FOUND)
            {
                DXGI_OUTPUT_DESC outDesc = {};
                output->GetDesc(&outDesc);
                if (wcsstr(displayName.c_str(), outDesc.DeviceName) == displayName.c_str())
                {
                    SetOutput(output);
                    break;
                }
            }
        }
    }
    else
    {
        SetOutput(nullptr);
    }
}

_Use_decl_annotations_
void GraphicsSystem::SetOutput(const ComPtr<IDXGIOutput>& output)
{
    _swapChain->SetFullscreenState(output != nullptr, output.Get());

    _frameBuffer = nullptr;
    _renderer2D = nullptr;
    _renderer3D = nullptr;

    HRESULT hr = _swapChain->ResizeBuffers(2, 1280, 800, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
    if (FAILED(hr))
    {
        throw std::exception();
    }

    ComPtr<ID3D11Resource> backBuffer;
    hr = _swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
    if (FAILED(hr))
    {
        throw std::exception();
    }

    hr = _device->CreateRenderTargetView(backBuffer.Get(), nullptr, &_frameBuffer);
    if (FAILED(hr))
    {
        throw std::exception();
    }

    _renderer2D = Renderer2D::Create(shared_from_this(), _context, _frameBuffer, _depthBuffer->GetDepthStencilView());
    _renderer3D = Renderer3D::Create(shared_from_this(), _context, _frameBuffer, _depthBuffer);
}

_Use_decl_annotations_
void GraphicsSystem::SetViewport(const D3D11_VIEWPORT& viewport)
{
    _context->RSSetViewports(1, &viewport);
}

const ComPtr<ID3D11SamplerState>& GraphicsSystem::GetPointClampSampler()
{
    if (_pointClampSampler == nullptr)
    {
        D3D11_SAMPLER_DESC sd = {};
        sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        sd.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
        sd.MaxLOD = D3D11_FLOAT32_MAX;

        HRESULT hr = _device->CreateSamplerState(&sd, &_pointClampSampler);
        if (FAILED(hr))
        {
            throw std::exception();
        }
    }

    return _pointClampSampler;
}

const ComPtr<ID3D11SamplerState>& GraphicsSystem::GetLinearClampSampler()
{
    if (_linearClampSampler == nullptr)
    {
        D3D11_SAMPLER_DESC sd = {};
        sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sd.MaxLOD = D3D11_FLOAT32_MAX;

        HRESULT hr = _device->CreateSamplerState(&sd, &_linearClampSampler);
        if (FAILED(hr))
        {
            throw std::exception();
        }
    }

    return _linearClampSampler;
}

const ComPtr<ID3D11SamplerState>& GraphicsSystem::GetPointWrapSampler()
{
    if (_pointWrapSampler == nullptr)
    {
        D3D11_SAMPLER_DESC sd = {};
        sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        sd.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
        sd.MaxLOD = D3D11_FLOAT32_MAX;

        HRESULT hr = _device->CreateSamplerState(&sd, &_pointWrapSampler);
        if (FAILED(hr))
        {
            throw std::exception();
        }
    }

    return _pointWrapSampler;
}

const ComPtr<ID3D11SamplerState>& GraphicsSystem::GetLinearWrapSampler()
{
    if (_linearWrapSampler == nullptr)
    {
        D3D11_SAMPLER_DESC sd = {};
        sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sd.MaxLOD = D3D11_FLOAT32_MAX;

        HRESULT hr = _device->CreateSamplerState(&sd, &_linearWrapSampler);
        if (FAILED(hr))
        {
            throw std::exception();
        }
    }

    return _linearWrapSampler;
}

const ComPtr<ID3D11BlendState>& GraphicsSystem::GetAlphaBlendState()
{
    if (_alphaBlendState == nullptr)
    {
        D3D11_BLEND_DESC bd = {};
        bd.RenderTarget[0].BlendEnable = TRUE;
        bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
        bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
        bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        HRESULT hr = _device->CreateBlendState(&bd, &_alphaBlendState);
        if (FAILED(hr))
        {
            throw std::exception();
        }
    }

    return _alphaBlendState;
}

const ComPtr<ID3D11BlendState>& GraphicsSystem::GetAdditiveBlendState()
{
    if (_additiveBlendState == nullptr)
    {
        D3D11_BLEND_DESC bd = {};
        bd.RenderTarget[0].BlendEnable = TRUE;
        bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        bd.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
        bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
        bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
        bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        HRESULT hr = _device->CreateBlendState(&bd, &_additiveBlendState);
        if (FAILED(hr))
        {
            throw std::exception();
        }
    }

    return _additiveBlendState;
}

const ComPtr<ID3D11BlendState>& GraphicsSystem::GetPremultipliedAlphaBlendState()
{
    if (_premulAlphaBlendState == nullptr)
    {
        D3D11_BLEND_DESC bd = {};
        bd.RenderTarget[0].BlendEnable = TRUE;
        bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
        bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
        bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        HRESULT hr = _device->CreateBlendState(&bd, &_premulAlphaBlendState);
        if (FAILED(hr))
        {
            throw std::exception();
        }
    }

    return _premulAlphaBlendState;
}

const ComPtr<ID3D11DepthStencilState>& GraphicsSystem::GetDepthReadNoWriteState()
{
    if (_depthReadNoWrite == nullptr)
    {
        D3D11_DEPTH_STENCIL_DESC dsd = {};
        dsd.DepthEnable = TRUE;
        dsd.DepthFunc = D3D11_COMPARISON_LESS;
        dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        dsd.StencilEnable = FALSE;

        HRESULT hr = _device->CreateDepthStencilState(&dsd, &_depthReadNoWrite);
        if (FAILED(hr))
        {
            throw std::exception();
        }
    }

    return _depthReadNoWrite;
}
