#include "precomp.h"
#include "renderer.h"
#include "gbuffer.h"
#include "texture2d.h"
#include "object.h"
#include "camera.h"
#include "scene.h"
#include "lighting.h"
#include "shadowmaps.h"
#include "highpassfilter.h"
#include "luminance.h"
#include "tonemapping.h"
#include "combine.h"

using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace DirectX;

#define ENABLE_HDR
#define SCENE_1

//==============================================================================

_Use_decl_annotations_
Renderer::Renderer(HWND target, uint32_t backBufferWidth, uint32_t backBufferHeight)
{
    assert(target != nullptr);
    CreateDeviceResources(target, backBufferWidth, backBufferHeight);

    _diffuse.reset(new Texture2D(_device.Get(), backBufferWidth, backBufferHeight, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, DXGI_FORMAT_R8G8B8A8_UNORM));
    _normals.reset(new Texture2D(_device.Get(), backBufferWidth, backBufferHeight, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, DXGI_FORMAT_R8G8B8A8_UNORM));
    _depth.reset(new Texture2D(_device.Get(), backBufferWidth, backBufferHeight, D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE, DXGI_FORMAT_R32_TYPELESS, DXGI_FORMAT_R32_TYPELESS, DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_D32_FLOAT));
#if defined(ENABLE_HDR)
    _light.reset(new Texture2D(_device.Get(), backBufferWidth, backBufferHeight, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, DXGI_FORMAT_R16G16B16A16_FLOAT));
    _lightHighPass.reset(new Texture2D(_device.Get(), backBufferWidth / 2, backBufferHeight / 2, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, DXGI_FORMAT_R16G16B16A16_FLOAT));
    _lightCombined.reset(new Texture2D(_device.Get(), backBufferWidth, backBufferHeight, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, DXGI_FORMAT_R16G16B16A16_FLOAT));
    _lightLuminance.reset(new Texture2D(_device.Get(), backBufferWidth, backBufferHeight, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, DXGI_FORMAT_R32_FLOAT));
    _tonemapped.reset(new Texture2D(_device.Get(), backBufferWidth, backBufferHeight, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, DXGI_FORMAT_R8G8B8A8_UNORM));
#else
    _light.reset(new Texture2D(_device.Get(), backBufferWidth, backBufferHeight, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, DXGI_FORMAT_R8G8B8A8_UNORM));
#endif
    _combined.reset(new Texture2D(_device.Get(), backBufferWidth, backBufferHeight, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, DXGI_FORMAT_R8G8B8A8_UNORM));

    _gbuffer = std::make_shared<GBuffer>(_context.Get(), _diffuse, _normals, _depth);
    _directionalShadows = std::make_shared<DirectionalLightShadows>(_context.Get());
    _pointShadows = std::make_shared<PointLightShadows>(_context.Get());
    _lighting = std::make_shared<DirectionalLighting>(_context.Get(), _depth, _normals, _light);
    _pointLighting = std::make_shared<PointLighting>(_context.Get(), _depth, _normals, _light);
#if defined(ENABLE_HDR)
    _highPass = std::make_shared<HighPassFilter>(_context.Get(), _light, _lightHighPass);
    _lightingCombine = std::make_shared<Combine>(_context.Get(), _light, _lightHighPass, _lightCombined, CombineType::Add);
    _luminance = std::make_shared<Luminance>(_context.Get(), _lightCombined, _lightLuminance);
    _tonemapping = std::make_shared<ToneMapping>(_context.Get(), _lightCombined, _lightLuminance, _tonemapped);
    _combine = std::make_shared<Combine>(_context.Get(), _diffuse, _tonemapped, _combined, CombineType::Multiply);
#else
    _combine = std::make_shared<Combine>(_context.Get(), _diffuse, _light, _combined, CombineType::Multiply);
#endif

    _camera = std::make_shared<Camera>();
    _camera->SetView(XMMatrixLookAtLH(XMVectorSet(1, 3, -5, 1), XMVectorSet(0, 0, 0, 1), XMVectorSet(0, 1, 0, 0)));
    _camera->SetProjection(XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), 16.0f/9.0f, 0.1f, 50.0f));

    _scene.reset(new Scene);

    // plane
    _scene->AddObject(Object::CreateCube(_device.Get(), 8, 0.25f, 8));

#ifdef SCENE_1
    // block
    auto block = Object::CreateCube(_device.Get(), 1, 3, 1);
    block->SetPosition(XMFLOAT3(0, 1.625f, 0));
    _scene->AddObject(block);

    block = Object::CreateCube(_device.Get(), 3, 0.125f, 0.25f);
    block->SetPosition(XMFLOAT3(3, 1.625f, -2));
    _scene->AddObject(block);
    _blade1 = block;

    block = Object::CreateCube(_device.Get(), 0.25f, 0.125f, 3.0f);
    block->SetPosition(XMFLOAT3(3, 1.625f, -2));
    _scene->AddObject(block);
    _blade2 = block;

    // lights
    //_scene->AddLight(std::make_shared<DirectionalLight>(XMFLOAT3(0, 1, 1), XMFLOAT3(1, 0, 0)));
    _scene->AddLight(std::make_shared<DirectionalLight>(XMFLOAT3(1, 1, -1), XMFLOAT3(0, 0, 1)));

    _scene->AddLight(std::make_shared<PointLight>(XMFLOAT3(1, 1, 0), 3.0f, XMFLOAT3(1, 1, 1)));
    _scene->AddLight(std::make_shared<PointLight>(XMFLOAT3(-1, 1, 0), 3.0f, XMFLOAT3(0, 1, 0)));
    //_scene->AddLight(std::make_shared<PointLight>(XMFLOAT3(0, 1, 1), 3.0f, XMFLOAT3(1, 0, 0)));
    //_scene->AddLight(std::make_shared<PointLight>(XMFLOAT3(0, 1, -1), 3.0f, XMFLOAT3(1, 1, 0)));
#else

    // block
    auto block = Object::CreateCube(_device.Get(), 1, 3, 1);
    block->SetPosition(XMFLOAT3(-2, 1.625f, -2));
    _scene->AddObject(block);

    block = Object::CreateCube(_device.Get(), 1, 3, 1);
    block->SetPosition(XMFLOAT3(-2, 1.625f, 2));
    _scene->AddObject(block);

    block = Object::CreateCube(_device.Get(), 1, 3, 1);
    block->SetPosition(XMFLOAT3(2, 1.625f, 2));
    _scene->AddObject(block);

    block = Object::CreateCube(_device.Get(), 1, 3, 1);
    block->SetPosition(XMFLOAT3(2, 1.625f, -2));
    _scene->AddObject(block);

    // lights
    //_scene->AddLight(std::make_shared<DirectionalLight>(XMFLOAT3(1, 1, -1), XMFLOAT3(1, 0, 0)));
    //_scene->AddLight(std::make_shared<DirectionalLight>(XMFLOAT3(0, 1, 1), XMFLOAT3(0, 0, 1)));

    _scene->AddLight(std::make_shared<PointLight>(XMFLOAT3(0, 1, 0), 6.0f, XMFLOAT3(2, 1, 0)));
    //_scene->AddLight(std::make_shared<PointLight>(XMFLOAT3(-1, 1, 0), 3.0f, XMFLOAT3(0, 0, 1)));
    //_scene->AddLight(std::make_shared<PointLight>(XMFLOAT3(0, 1, 1), 3.0f, XMFLOAT3(1, 0, 0)));
    //_scene->AddLight(std::make_shared<PointLight>(XMFLOAT3(0, 1, -1), 3.0f, XMFLOAT3(1, 1, 0)));
#endif
}

//==============================================================================

void Renderer::RenderFrame()
{
    Clear();

    if (_blade1 != nullptr)
    {
        // move the fan blades
        XMVECTOR orientation = XMLoadFloat4(&_blade1->GetOrientation());
        orientation = XMQuaternionNormalize(XMQuaternionMultiply(orientation, XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), 0.05f)));
        _blade1->SetOrientation(orientation);
        _blade2->SetOrientation(orientation);
    }

#if defined(SCENE_1)
    // move the camera
    XMVECTOR det;
    XMMATRIX invView = XMMatrixInverse(&det, XMLoadFloat4x4(&_camera->GetView()));
    XMVECTOR cameraPos = invView.r[3];
    cameraPos = XMVector3Transform(cameraPos, XMMatrixRotationAxis(XMVectorSet(0, 1, 0, 0), 0.01f));
    _camera->SetView(XMMatrixLookAtLH(cameraPos, XMVectorSet(0, 0, 0, 1), XMVectorSet(0, 1, 0, 0)));
#endif

#if 0
#if defined(SCENE_1) && !defined(ENABLE_HDR)
    // rotate point lights
    static std::vector<std::shared_ptr<Light>> lights;
    _scene->GetLightsInSphere(XMFLOAT3(), 1, lights);

    XMMATRIX rot = XMMatrixRotationAxis(XMVectorSet(0, 1, 0, 0), 0.08f);
    for (auto light : lights)
    {
        if (light->Type == LightType::Point)
        {
            auto pointLight = reinterpret_cast<PointLight*>(light.get());
            XMVECTOR pos = XMLoadFloat3(&pointLight->Position);
            pos = XMVector3Transform(pos, rot);
            XMStoreFloat3(&pointLight->Position, pos);
        }
    }
    lights.clear();
#elif !defined(ENABLE_HDR)
    // move point lights
    static std::vector<std::shared_ptr<Light>> lights;
    _scene->GetLightsInSphere(XMFLOAT3(), 1, lights);

    static XMFLOAT3 lightPos = XMFLOAT3(3, 4, 0);
    static float dir = 1.0f;
    lightPos.x += dir * 0.05f;
    if (fabsf(lightPos.x) > 5)
    {
        dir *= -1;
    }

    for (auto light : lights)
    {
        if (light->Type == LightType::Point)
        {
            auto pointLight = reinterpret_cast<PointLight*>(light.get());
            pointLight->Position = lightPos;
        }
    }
    lights.clear();
#endif
#endif

    _gbuffer->RenderFrame(_camera, _scene);
    _directionalShadows->BuildShadowmaps(_camera, _scene);
    _pointShadows->BuildShadowmaps(_camera, _scene);
    _lighting->RenderFrame(_camera, _scene);
    _pointLighting->RenderFrame(_camera, _scene);
#if defined(ENABLE_HDR)
    _highPass->RenderFrame(_camera, _scene);
    _lightingCombine->RenderFrame(_camera, _scene);
    _luminance->RenderFrame(_camera, _scene);
    _tonemapping->RenderFrame(_camera, _scene);
#endif
    _combine->RenderFrame(_camera, _scene);

    ID3D11RenderTargetView* empty[] = { nullptr };
    _context->OMSetRenderTargets(0, empty, nullptr);

    ComPtr<ID3D11Resource> resource;
    _backBuffer->GetResource(&resource);
    D3D11_BOX box = {};
    box.left = 0;
    box.right = 1280;
    box.top = 0;
    box.bottom = 720;
    box.front = 0;
    box.back = 1;
    _context->CopySubresourceRegion(resource.Get(), 0, 0, 0, 0, _combined->GetTexture().Get(), 0, &box);
    //_context->CopyResource(resource.Get(), _combined->GetTexture().Get());

    Present();
}

//==============================================================================

_Use_decl_annotations_
void Renderer::CreateDeviceResources(HWND target, uint32_t backBufferWidth, uint32_t backBufferHeight)
{
    DXGI_SWAP_CHAIN_DESC scd = {};
    scd.BufferCount = 2;
    scd.BufferDesc.Width = backBufferWidth;
    scd.BufferDesc.Height = backBufferHeight;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferDesc.RefreshRate.Numerator = 1;
    scd.BufferDesc.RefreshRate.Denominator = 60;
    scd.BufferDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
    scd.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = target;
    scd.SampleDesc.Count = 1;
    scd.SampleDesc.Quality = 0;
    scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    scd.Windowed = TRUE;

    UINT deviceFlags = 0;

#ifdef _DEBUG
    deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL requestedFeatureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, deviceFlags,
        requestedFeatureLevels, _countof(requestedFeatureLevels), D3D11_SDK_VERSION, &scd, &_swapChain,
        &_device, &featureLevel, &_context);
    if (FAILED(hr))
    {
        throw std::exception("D3D11CreateDeviceAndSwapChain failed!");
    }

    ComPtr<ID3D11Texture2D> texture;
    hr = _swapChain->GetBuffer(0, IID_PPV_ARGS(&texture));
    if (FAILED(hr))
    {
        throw std::exception("SwapChain::GetBuffer failed!");
    }

    hr = _device->CreateRenderTargetView(texture.Get(), nullptr, &_backBuffer);
    if (FAILED(hr))
    {
        throw std::exception("CreateRenderTargetView failed!");
    }

    D3D11_TEXTURE2D_DESC td = {};
    td.ArraySize = 1;
    td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    td.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    td.Width = backBufferWidth;
    td.Height = backBufferHeight;
    td.MipLevels = 1;
    td.SampleDesc.Count = 1;
    td.SampleDesc.Quality = 0;
    td.Usage = D3D11_USAGE_DEFAULT;

    hr = _device->CreateTexture2D(&td, nullptr, &texture);
    if (FAILED(hr))
    {
        throw std::exception("CreateRenderTargetView failed!");
    }

    hr = _device->CreateDepthStencilView(texture.Get(), nullptr, &_depthStencil);
    if (FAILED(hr))
    {
        throw std::exception("CreateRenderTargetView failed!");
    }

    D3D11_VIEWPORT vp = {};
    vp.Width = static_cast<float>(backBufferWidth);
    vp.Height = static_cast<float>(backBufferHeight);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;

    _context->RSSetViewports(1, &vp);
}

void Renderer::Clear()
{
    static const float clearColor[] = { 0.0f, 0.0f, 0.5f, 1.0f };
    _context->ClearRenderTargetView(_backBuffer.Get(), clearColor);
    _context->ClearDepthStencilView(_depthStencil.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    _context->OMSetRenderTargets(1, _backBuffer.GetAddressOf(), _depthStencil.Get());
}

void Renderer::Present()
{
    _swapChain->Present(1, 0);
}
