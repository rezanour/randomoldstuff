#include "precomp.h"
#include "graphics.h"

_Use_decl_annotations_
std::unique_ptr<Graphics> Graphics::Create(HWND window)
{
    std::unique_ptr<Graphics> graphics(new Graphics(window));

    // Set the global entry for others to use
    Globals::Graphics = graphics.get();

    return graphics;
}

_Use_decl_annotations_
Graphics::Graphics(HWND window) :
    _window(window)
{
    DXGI_SWAP_CHAIN_DESC swd = {};
    swd.BufferCount = 3;
    swd.BufferDesc.Width = Globals::ScreenWidth;
    swd.BufferDesc.Height = Globals::ScreenHeight;
    swd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swd.BufferDesc.RefreshRate.Numerator = 60;
    swd.BufferDesc.RefreshRate.Denominator = 1;
    swd.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swd.OutputWindow = window;
    swd.SampleDesc.Count = 1;
    swd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    swd.Windowed = TRUE;

    D3D_FEATURE_LEVEL requestedLevel = D3D_FEATURE_LEVEL_11_0;
    D3D_FEATURE_LEVEL featureLevel;

    UINT flags = 0;
#if _DEBUG
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    //
    // Find a DXGI Adapter that supports hardware D3D11
    //
    ComPtr<IDXGIFactory1> factory;
    CHECKHR(CreateDXGIFactory1(IID_PPV_ARGS(&factory)));

#if 1
    ComPtr<IDXGIAdapter1> adapter;
    uint32_t adapterIndex = 0;
    while (SUCCEEDED(factory->EnumAdapters1(adapterIndex++, &adapter)))
    {
        DXGI_ADAPTER_DESC1 desc = {};
        if (SUCCEEDED(adapter->GetDesc1(&desc)))
        {
            if ((desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0)
            {
                if (SUCCEEDED(D3D11CreateDeviceAndSwapChain(adapter.Get(), D3D_DRIVER_TYPE_UNKNOWN, nullptr, flags,
                    &requestedLevel, 1, D3D11_SDK_VERSION, &swd, &_swapChain, &_device, &featureLevel, &_context)))
                {
                    // Success!
                    DebugOut("Using Adapter: %ls.\n"
                             "Video Memory: %u.\n",
                             desc.Description,
                             (uint32_t)desc.DedicatedVideoMemory);
                    break;
                }
            }
        }
    }
#else
    CHECKHR(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, flags,
        &requestedLevel, 1, D3D11_SDK_VERSION, &swd, &_swapChain, &_device, &featureLevel, &_context));
#endif

    CHECKNOTNULL(_device);

    ComPtr<ID3D11Resource> resource;
    CHECKHR(_swapChain->GetBuffer(0, IID_PPV_ARGS(&resource)));
    CHECKHR(_device->CreateRenderTargetView(resource.Get(), nullptr, &_backBuffer));
    _context->OMSetRenderTargets(1, _backBuffer.GetAddressOf(), nullptr);

    D3D11_VIEWPORT vp = {};
    vp.Width = (float)Globals::ScreenWidth;
    vp.Height = (float)Globals::ScreenHeight;
    vp.MaxDepth = 1.0f;
    _context->RSSetViewports(1, &vp);
}

Graphics::~Graphics()
{
    // Remove global entry
    Globals::Graphics = nullptr;
}

_Use_decl_annotations_
void Graphics::Clear(const XMFLOAT4& color)
{
    _context->ClearRenderTargetView(_backBuffer.Get(), &color.x);
}

void Graphics::Present()
{
    _swapChain->Present(0, 0);
    _context->OMSetRenderTargets(1, _backBuffer.GetAddressOf(), nullptr);
}
