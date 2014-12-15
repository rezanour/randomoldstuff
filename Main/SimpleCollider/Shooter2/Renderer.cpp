#include "Precomp.h"
#include <dxgi1_3.h>

Renderer::Renderer(const RendererConfig& config)
{
    InitializeGraphics(config);
}

void Renderer::Clear(const float color[4], float depth)
{
    _context->ClearRenderTargetView(_renderTarget.Get(), color);
    _context->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH, depth, 0);
    _context->OMSetRenderTargets(1, _renderTarget.GetAddressOf(), _depthStencilView.Get());
}

void Renderer::Present(bool waitForVSync)
{
    ResetEvent(_frameEvent);

    HRESULT hr = _swapChain->Present(0, 0);
    if (FAILED(hr)) throw std::exception();

    if (waitForVSync)
    {
        WaitForSingleObject(_frameEvent, INFINITE);
    }
}

void Renderer::InitializeGraphics(const RendererConfig& config)
{
    DXGI_SWAP_CHAIN_DESC scd = {};
    scd.BufferCount = 2;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferDesc.Width = config.Width;
    scd.BufferDesc.Height = config.Height;
    scd.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = config.Window;
    scd.SampleDesc.Count = 1;
    scd.Windowed = TRUE;
    scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    scd.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;

    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

    uint32_t flags = 0;
    if (config.DebugDevice)
    {
        flags |= D3D11_CREATE_DEVICE_DEBUG;
    }

    //
    // Some systems (like my laptop), have multiple GPUs,
    // and the "default" one isn't always the one we want.
    // Try creating the device with the parameters we want
    // on each adapter until we find one that works.
    //
    ComPtr<IDXGIFactory> factory;
    HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&factory));
    if (FAILED(hr)) throw std::exception();

    uint32_t adapterIndex = 0;
    ComPtr<IDXGIAdapter> adapter;
    while (SUCCEEDED(hr = factory->EnumAdapters(adapterIndex++, &adapter)))
    {
        DXGI_ADAPTER_DESC desc;
        hr = adapter->GetDesc(&desc);
        if (FAILED(hr)) throw std::exception();

        hr = D3D11CreateDeviceAndSwapChain(adapter.Get(), D3D_DRIVER_TYPE_UNKNOWN, nullptr,
            flags, &featureLevel, 1, D3D11_SDK_VERSION, &scd, &_swapChain, &_device, nullptr,
            &_context);
        if (SUCCEEDED(hr))
        {
            DebugOut(L"Using device: %s\n\tVRAM: %d MB\n", desc.Description, (desc.DedicatedVideoMemory / 1024 / 1024));
            break;
        }
    }

    // No suitable adapter found
    if (FAILED(hr)) throw std::exception();

    ComPtr<IDXGISwapChain2> swapChain2;
    hr = _swapChain.As(&swapChain2);
    if (FAILED(hr)) throw std::exception();

    _frameEvent = swapChain2->GetFrameLatencyWaitableObject();

    ComPtr<ID3D11Texture2D> texture;
    hr = _swapChain->GetBuffer(0, IID_PPV_ARGS(&texture));
    if (FAILED(hr)) throw std::exception();

    hr = _device->CreateRenderTargetView(texture.Get(), nullptr, &_renderTarget);
    if (FAILED(hr)) throw std::exception();

    D3D11_TEXTURE2D_DESC td = {};
    texture->GetDesc(&td);

    td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    td.Format = DXGI_FORMAT_D32_FLOAT;

    hr = _device->CreateTexture2D(&td, nullptr, &texture);
    if (FAILED(hr)) throw std::exception();

    hr = _device->CreateDepthStencilView(texture.Get(), nullptr, &_depthStencilView);
    if (FAILED(hr)) throw std::exception();

    D3D11_VIEWPORT vp = {};
    vp.Width = (float)config.Width;
    vp.Height = (float)config.Height;
    vp.MaxDepth = 1.0f;

    _context->RSSetViewports(1, &vp);
}
