#include "stdafx.h"

HWND g_hWnd = nullptr;
ID3D11Device* g_device;
ID3D11DeviceContext* g_context;
IDXGISwapChain* g_swapChain;
ID3D11Texture2D* g_backBufferTexture;
ID3D11RenderTargetView* g_backBuffer;
ID3D11Texture2D* g_depthStencilTexture;
ID3D11DepthStencilView* g_depthStencil;
DXGI_SURFACE_DESC g_backbufferSurfaceDesc = {0};
DXGI_SWAP_CHAIN_DESC g_swapChainDesc = { 0 };

void __cdecl DXUTRender3DEnvironment(void)
{

}

struct ID3D11DeviceContext * __cdecl DXUTGetD3D11DeviceContext(void)
{
    return g_context;
}

struct ID3D11Device * __cdecl DXUTGetD3D11Device(void)
{
    return g_device;
}

struct DXGI_SURFACE_DESC const * __cdecl DXUTGetDXGIBackBufferSurfaceDesc(void)
{
    return &g_backbufferSurfaceDesc;
}

long __cdecl DXUTCreateDevice(enum D3D_FEATURE_LEVEL,bool,int,int)
{
    HRESULT hr = S_OK;
    RECT client;
    GetClientRect(g_hWnd, &client);

    g_swapChainDesc.BufferCount = 1;
    g_swapChainDesc.BufferDesc.Width = client.right;
    g_swapChainDesc.BufferDesc.Height = client.bottom;
    g_swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    g_swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_BACK_BUFFER;
    g_swapChainDesc.OutputWindow = g_hWnd;
    g_swapChainDesc.SampleDesc.Count = 1;
    g_swapChainDesc.Windowed = TRUE;

    g_backbufferSurfaceDesc.Format = g_swapChainDesc.BufferDesc.Format;
    g_backbufferSurfaceDesc.Height = g_swapChainDesc.BufferDesc.Height;
    g_backbufferSurfaceDesc.Width = g_swapChainDesc.BufferDesc.Width;
    g_backbufferSurfaceDesc.SampleDesc = g_swapChainDesc.SampleDesc;

    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_10_0;
    
    hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, &featureLevel, 1, D3D11_SDK_VERSION, &g_swapChainDesc, &g_swapChain, &g_device, NULL, &g_context);

    if (SUCCEEDED(hr))
    {
        hr = g_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&g_backBufferTexture);
    }

    D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc = { DXGI_FORMAT_UNKNOWN, D3D11_RTV_DIMENSION_TEXTURE2D };

    if (SUCCEEDED(hr))
    {
        hr = g_device->CreateRenderTargetView(g_backBufferTexture, &renderTargetViewDesc, &g_backBuffer);
    }

    D3D11_TEXTURE2D_DESC depthStencilDesc = { 0 };

    depthStencilDesc.Width = client.right;
    depthStencilDesc.Height = client.bottom;
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;

    if (SUCCEEDED(hr))
    {
        hr = g_device->CreateTexture2D(&depthStencilDesc, NULL, &g_depthStencilTexture);
    }

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
    depthStencilViewDesc.Format = DXGI_FORMAT_UNKNOWN;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

    if (SUCCEEDED(hr))
    {
        hr = g_device->CreateDepthStencilView(g_depthStencilTexture, &depthStencilViewDesc, &g_depthStencil);
    }

    if (FAILED(hr))
    {
        DXUTShutdown(0,false);
    }

    return hr;
}

long __cdecl DXUTSetWindow(HWND h ,struct HWND__ *,struct HWND__ *,bool,bool)
{
    g_hWnd = h;
    return S_OK;
}

void __cdecl DXUTSetCursorSettings(bool,bool)
{
    
}

long __cdecl DXUTInit(bool,bool,wchar_t *,bool)
{
    return S_OK;
}

void __cdecl DXUTSetCallbackD3D11DeviceDestroyed(void (__cdecl*)(void *),void *)
{
}

void __cdecl DXUTSetCallbackD3D11SwapChainReleasing(void (__cdecl*)(void *),void *)
{

}

void __cdecl DXUTSetCallbackD3D11FrameRender(void (__cdecl*)(struct ID3D11Device *,struct ID3D11DeviceContext *,double,float,void *),void *)
{

}

void __cdecl DXUTSetCallbackD3D11SwapChainResized(long (__cdecl*)(struct ID3D11Device *,struct IDXGISwapChain *,struct DXGI_SURFACE_DESC const *,void *),void *)
{

}

void __cdecl DXUTSetCallbackD3D11DeviceCreated(long (__cdecl*)(struct ID3D11Device *,struct DXGI_SURFACE_DESC const *,void *),void *)
{

}

void __cdecl DXUTSetCallbackD3D11DeviceAcceptable(bool (__cdecl*)(class CD3D11EnumAdapterInfo const *,unsigned int,class CD3D11EnumDeviceInfo const *,enum DXGI_FORMAT,bool,void *),void *)
{

}

void __cdecl DXUTSetCallbackDeviceRemoved(bool (__cdecl*)(void *),void *)
{

}

void __cdecl DXUTSetCallbackDeviceChanging(bool (__cdecl*)(struct DXUTDeviceSettings *,void *),void *)
{

}

void __cdecl DXUTSetCallbackMsgProc(__int64 (__cdecl*)(struct HWND__ *,unsigned int,unsigned __int64,__int64,bool *,void *),void *)
{

}

void __cdecl DXUTSetCallbackMouse(void (__cdecl*)(bool,bool,bool,bool,bool,int,int,int,void *),bool,void *)
{

}

void __cdecl DXUTSetCallbackKeyboard(void (__cdecl*)(unsigned int,bool,bool,void *),void *)
{

}

void __cdecl DXUTSetCallbackFrameMove(void (__cdecl*)(double,float,void *),void *)
{

}

void __cdecl DXUTSetIsInGammaCorrectMode(bool)
{

}

struct ID3D11DepthStencilView * __cdecl DXUTGetD3D11DepthStencilView(void)
{
    return nullptr;
}

struct ID3D11RenderTargetView * __cdecl DXUTGetD3D11RenderTargetView(void)
{
    return nullptr;
}

void __cdecl DXUTShutdown(int,bool)
{
    SAFE_RELEASE(g_depthStencilTexture);
    SAFE_RELEASE(g_depthStencil);
    SAFE_RELEASE(g_backBuffer);
    SAFE_RELEASE(g_backBufferTexture);
    SAFE_RELEASE(g_swapChain);
    SAFE_RELEASE(g_context);
    SAFE_RELEASE(g_device);
}

__int64 __cdecl DXUTStaticWndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64)
{
    return 0;
}

void __cdecl DXUTSetWindowSettings(bool)
{

}