#include "StdAfx.h"
#include "RendererView.h"

using namespace Lucid3D;
using namespace GDK;

HRESULT RendererView::Create(_In_ const RendererViewSettings& viewSettings, _In_ ContextPtr& spContext, _Out_ ViewPtr& spView)
{
    HRESULT hr = S_OK;

    ISNOTNULL(viewSettings.WindowIdentity, E_INVALIDARG);
    ISNOTNULL(spContext, E_INVALIDARG);

    spView.attach(new RendererView);
    ISNOTNULL(spView, E_OUTOFMEMORY);
    CHECKHR(spView->Initialize(viewSettings, spContext));

EXIT
    if (FAILED(hr))
    {
        spView.reset();
    }
    return hr;
}

RendererView::RendererView()
    : _hwnd(nullptr)
{
}

HRESULT RendererView::Initialize(_In_ const RendererViewSettings& viewSettings, _In_ ContextPtr& spContext)
{
    HRESULT hr = S_OK;

    DevicePtr spDevice;
    stde::com_ptr<IDXGIDevice> spDXGIDevice;
    stde::com_ptr<IDXGIAdapter> spAdapter;
    stde::com_ptr<IDXGIFactory> spFactory;
    Texture2DPtr spTexture;
    DXGI_SWAP_CHAIN_DESC desc = {0};

    _hwnd = reinterpret_cast<HWND>(const_cast<void*>(viewSettings.WindowIdentity));
    _spContext = spContext;
    _spContext->GetDevice(&spDevice);

    CHECKHR(spDXGIDevice.from<ID3D11Device>(spDevice));
    CHECKHR(spDXGIDevice->GetAdapter(&spAdapter));

    desc.OutputWindow = _hwnd;
    desc.Windowed = !viewSettings.IsFullScreen;
    desc.BufferCount = desc.Windowed ? 1 : 2;
    desc.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.BufferDesc.Width = static_cast<UINT>(viewSettings.ScreenWidth);
    desc.BufferDesc.Height = static_cast<UINT>(viewSettings.ScreenHeight);
    desc.BufferDesc.RefreshRate.Numerator = 60;
    desc.BufferDesc.RefreshRate.Denominator = 1;

    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;

    // TODO: For Win8+, we should get a DXGIFactory1 or 2, and use CreateSwapChainForHWND/Immersive
    CHECKHR(spAdapter->GetParent(IID_PPV_ARGS(&spFactory)));
    CHECKHR(spFactory->CreateSwapChain(spDevice, &desc, &_spSwapChain));

    CHECKHR(_spSwapChain->GetBuffer(0, IID_PPV_ARGS(&spTexture)));
    CHECKHR(spDevice->CreateRenderTargetView(spTexture, nullptr, &_spBackBuffer));

EXIT
    return hr;
}

RenderTargetPtr RendererView::GetBackBuffer() const
{
    return _spBackBuffer;
}

HRESULT RendererView::Present()
{
    return _spSwapChain->Present(1, 0);
}

GDK_IMETHODIMP_(const void*) RendererView::GetWindowIdentity() const
{
    return reinterpret_cast<const void*>(_hwnd);
}

GDK_IMETHODIMP_(bool) RendererView::IsFullScreen() const
{
    return _settings.IsFullScreen;
}

GDK_IMETHODIMP RendererView::SetFullScreen(_In_ bool fullScreen)
{
    return E_NOTIMPL;
}

GDK_IMETHODIMP RendererView::GetSize(_Out_ size_t* pWidth, _Out_ size_t* pHeight) const
{
    if (!pWidth || !pHeight)
        return E_INVALIDARG;

    *pWidth = _settings.ScreenWidth;
    *pHeight = _settings.ScreenHeight;

    return S_OK;
}

GDK_IMETHODIMP RendererView::SetSize(_In_ size_t width, _In_ size_t height)
{
    return E_NOTIMPL;
}
