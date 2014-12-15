#pragma once

void __cdecl DXUTRender3DEnvironment(void);
struct ID3D11DeviceContext * __cdecl DXUTGetD3D11DeviceContext(void);
struct ID3D11Device * __cdecl DXUTGetD3D11Device(void);
struct DXGI_SURFACE_DESC const * __cdecl DXUTGetDXGIBackBufferSurfaceDesc(void);
long __cdecl DXUTCreateDevice(enum D3D_FEATURE_LEVEL,bool,int,int);
long __cdecl DXUTSetWindow(struct HWND__ *,struct HWND__ *,struct HWND__ *,bool,bool);
void __cdecl DXUTSetCursorSettings(bool,bool);
long __cdecl DXUTInit(bool,bool,wchar_t *,bool);
void __cdecl DXUTSetCallbackD3D11DeviceDestroyed(void (__cdecl*)(void *),void *);
void __cdecl DXUTSetCallbackD3D11SwapChainReleasing(void (__cdecl*)(void *),void *);
void __cdecl DXUTSetCallbackD3D11FrameRender(void (__cdecl*)(struct ID3D11Device *,struct ID3D11DeviceContext *,double,float,void *),void *);
void __cdecl DXUTSetCallbackD3D11SwapChainResized(long (__cdecl*)(struct ID3D11Device *,struct IDXGISwapChain *,struct DXGI_SURFACE_DESC const *,void *),void *);
void __cdecl DXUTSetCallbackD3D11DeviceCreated(long (__cdecl*)(struct ID3D11Device *,struct DXGI_SURFACE_DESC const *,void *),void *);
void __cdecl DXUTSetCallbackD3D11DeviceAcceptable(bool (__cdecl*)(class CD3D11EnumAdapterInfo const *,unsigned int,class CD3D11EnumDeviceInfo const *,enum DXGI_FORMAT,bool,void *),void *);
void __cdecl DXUTSetCallbackDeviceRemoved(bool (__cdecl*)(void *),void *);
void __cdecl DXUTSetCallbackDeviceChanging(bool (__cdecl*)(struct DXUTDeviceSettings *,void *),void *);
void __cdecl DXUTSetCallbackMsgProc(__int64 (__cdecl*)(struct HWND__ *,unsigned int,unsigned __int64,__int64,bool *,void *),void *);
void __cdecl DXUTSetCallbackMouse(void (__cdecl*)(bool,bool,bool,bool,bool,int,int,int,void *),bool,void *);
void __cdecl DXUTSetCallbackKeyboard(void (__cdecl*)(unsigned int,bool,bool,void *),void *);
void __cdecl DXUTSetCallbackFrameMove(void (__cdecl*)(double,float,void *),void *);
void __cdecl DXUTSetIsInGammaCorrectMode(bool);
struct ID3D11DepthStencilView * __cdecl DXUTGetD3D11DepthStencilView(void);
struct ID3D11RenderTargetView * __cdecl DXUTGetD3D11RenderTargetView(void);
void __cdecl DXUTShutdown(int,bool);
__int64 __cdecl DXUTStaticWndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64);
void __cdecl DXUTSetWindowSettings(bool);
