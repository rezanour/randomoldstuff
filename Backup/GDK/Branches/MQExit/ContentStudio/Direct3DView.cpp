#include "stdafx.h"

IMPLEMENT_DYNCREATE(CDirect3DView, CView)

BEGIN_MESSAGE_MAP(CDirect3DView, CView)
    ON_WM_ERASEBKGND()
    ON_WM_NCDESTROY()
END_MESSAGE_MAP()

CDirect3DView::CDirect3DView()
{

}

CDirect3DView::~CDirect3DView()
{

}

BOOL CDirect3DView::OnEraseBkgnd(CDC* pDC)
{
    return FALSE;
}

void CDirect3DView::OnDraw(CDC* /*pDC*/)
{
    DXUTRender3DEnvironment();
}

void CDirect3DView::OnInitialUpdate()
{
    __super::OnInitialUpdate();

    if (DXUTGetD3D11Device() != NULL)
    {
        return;
    }

    DXUTSetIsInGammaCorrectMode(false);

    // Set general DXUT callbacks
    DXUTSetCallbackFrameMove( OnDXUTFrameMove, this );
    DXUTSetCallbackKeyboard( OnDXUTKeyboard, this );
    DXUTSetCallbackMouse( OnDXUTMouse, true, this );
    DXUTSetCallbackMsgProc( DXUTMsgProc, this );
    DXUTSetCallbackDeviceChanging( DXUTModifyDeviceSettings, this );
    DXUTSetCallbackDeviceRemoved( OnDXUTDeviceRemoved, this );

    // Set the D3D11 DXUT callbacks. Remove these sets if the app doesn't need to support D3D11
    DXUTSetCallbackD3D11DeviceAcceptable( DXUTIsD3D11DeviceAcceptable, this );
    DXUTSetCallbackD3D11DeviceCreated( OnDXUTD3D11CreateDevice, this );
    DXUTSetCallbackD3D11SwapChainResized( OnDXUTD3D11ResizedSwapChain, this );
    DXUTSetCallbackD3D11FrameRender( OnDXUTD3D11FrameRender, this );
    DXUTSetCallbackD3D11SwapChainReleasing( OnDXUTD3D11ReleasingSwapChain, this );
    DXUTSetCallbackD3D11DeviceDestroyed( OnDXUTD3D11DestroyDevice, this );

    DXUTInit( true, true, NULL ); // Parse the command line, show msgboxes on error, no extra command line params
    DXUTSetCursorSettings( true, true ); // Show the cursor and clip it when in full screen

    // Create DXUT stuff
    DXUTSetWindow( this->GetSafeHwnd(), this->GetSafeHwnd(), this->GetSafeHwnd(), false );
    CRect rect;

    GetClientRect(&rect);

    // Only require 10-level hardware
    DXUTCreateDevice( D3D_FEATURE_LEVEL_10_1, true, rect.right, rect.bottom );
}

bool CDirect3DView::IsD3D11DeviceAcceptable( const CD3D11EnumAdapterInfo *AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo *DeviceInfo, DXGI_FORMAT BackBufferFormat, bool bWindowed)
{
    return true;
}

bool CDirect3DView::ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings)
{
    pDeviceSettings->d3d11.SyncInterval = 1; // limit to 60fps
    return true;
}

HRESULT CDirect3DView::OnD3D11CreateDevice( ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc)
{
    return S_OK;
}

HRESULT CDirect3DView::OnD3D11ResizedSwapChain( ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc)
{
    return S_OK;
}

void CDirect3DView::OnFrameMove( double fTime, float fElapsedTime) { }

void CDirect3DView::OnD3D11FrameRender( ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime, float fElapsedTime)
{
    // Clear render target and the depth stencil 
    //float ClearColor[4] = { 0.176f, 0.196f, 0.667f, 0.0f };
    float ClearColor[4] = {0.39f, 0.58f, 0.929f, 0.0f};

    ID3D11RenderTargetView* pRTV = DXUTGetD3D11RenderTargetView();
    ID3D11DepthStencilView* pDSV = DXUTGetD3D11DepthStencilView();
    pd3dImmediateContext->ClearRenderTargetView( pRTV, ClearColor );
    pd3dImmediateContext->ClearDepthStencilView( pDSV, D3D11_CLEAR_DEPTH| D3D11_CLEAR_STENCIL, 1.0, 0 );
}

void CDirect3DView::OnD3D11ReleasingSwapChain()
{

}

void CDirect3DView::OnD3D11DestroyDevice()
{

}

LRESULT CDirect3DView::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing)
{
    return 0;
}

void CDirect3DView::OnKeyboard( UINT nChar, bool bKeyDown, bool bAltDown) { }
void CDirect3DView::OnMouse( bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown, bool bSideButton1Down, bool bSideButton2Down, int nMouseWheelDelta, int xPos, int yPos) { }
bool CDirect3DView::OnDeviceRemoved() { return true; }

void CDirect3DView::OnNcDestroy()
{
    DXUTShutdown(0, false);
    CView::OnNcDestroy();
}

BOOL CDirect3DView::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
    switch(message)
    {
    case WM_ERASEBKGND: // avoid flicker by not allowing DXUTStaticWndProc to handle this message
        break;
    case WM_KEYDOWN:    // avoid allowing DXUT to process certian key presses
        {
            if (wParam == VK_ESCAPE) break;
        }
    default:
        DXUTSetWindowSettings(false);
        DXUTStaticWndProc( this->GetSafeHwnd(), message, wParam, lParam);
        break;
    }

    return CView::OnWndMsg(message, wParam, lParam, pResult);
}

// DXUT callbacks
bool CALLBACK DXUTIsD3D11DeviceAcceptable( const CD3D11EnumAdapterInfo *AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo *DeviceInfo,
                                       DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
    CDirect3DView* pView = (CDirect3DView*)pUserContext;
    if (pView != NULL)
    {
        return pView->IsD3D11DeviceAcceptable(AdapterInfo, Output, DeviceInfo, BackBufferFormat, bWindowed);
    }
    return true;
}

bool CALLBACK DXUTModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext )
{
    CDirect3DView* pView = (CDirect3DView*)pUserContext;
    if (pView != NULL)
    {
        return pView->ModifyDeviceSettings(pDeviceSettings);
    }
    return true;
}

HRESULT CALLBACK OnDXUTD3D11CreateDevice( ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,
                                      void* pUserContext )
{
    CDirect3DView* pView = (CDirect3DView*)pUserContext;
    if (pView != NULL)
    {
        return pView->OnD3D11CreateDevice(pd3dDevice, pBackBufferSurfaceDesc);
    }
    return S_OK;
}

HRESULT CALLBACK OnDXUTD3D11ResizedSwapChain( ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
                                          const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
    CDirect3DView* pView = (CDirect3DView*)pUserContext;
    if (pView != NULL)
    {
        return pView->OnD3D11ResizedSwapChain(pd3dDevice, pSwapChain, pBackBufferSurfaceDesc);
    }
    return S_OK;
}

void CALLBACK OnDXUTFrameMove( double fTime, float fElapsedTime, void* pUserContext )
{
    CDirect3DView* pView = (CDirect3DView*)pUserContext;
    if (pView != NULL)
    {
        pView->OnFrameMove(fTime, fElapsedTime);
    }
}

void CALLBACK OnDXUTD3D11FrameRender( ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext,
                                  double fTime, float fElapsedTime, void* pUserContext )
{
    CDirect3DView* pView = (CDirect3DView*)pUserContext;
    if (pView != NULL)
    {
        pView->OnD3D11FrameRender(pd3dDevice, pd3dImmediateContext, fTime, fElapsedTime);
    }
}

void CALLBACK OnDXUTD3D11ReleasingSwapChain( void* pUserContext )
{
    CDirect3DView* pView = (CDirect3DView*)pUserContext;
    if (pView != NULL)
    {
        pView->OnD3D11ReleasingSwapChain();
    }
}

void CALLBACK OnDXUTD3D11DestroyDevice( void* pUserContext )
{
    CDirect3DView* pView = (CDirect3DView*)pUserContext;
    if (pView != NULL)
    {
        pView->OnD3D11DestroyDevice();
    }
}

LRESULT CALLBACK DXUTMsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
                          bool* pbNoFurtherProcessing, void* pUserContext )
{
    CDirect3DView* pView = (CDirect3DView*)pUserContext;
    if (pView != NULL)
    {
        return pView->MsgProc(hWnd, uMsg, wParam, lParam, pbNoFurtherProcessing);
    }
    return 0;
}

void CALLBACK OnDXUTKeyboard( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext )
{
    CDirect3DView* pView = (CDirect3DView*)pUserContext;
    if (pView != NULL)
    {
        pView->OnKeyboard(nChar, bKeyDown, bAltDown);
    }
}

void CALLBACK OnDXUTMouse( bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown,
                       bool bSideButton1Down, bool bSideButton2Down, int nMouseWheelDelta,
                       int xPos, int yPos, void* pUserContext )
{
    CDirect3DView* pView = (CDirect3DView*)pUserContext;
    if (pView != NULL)
    {
        pView->OnMouse(bLeftButtonDown, bRightButtonDown, bMiddleButtonDown,
                       bSideButton1Down, bSideButton2Down, nMouseWheelDelta,
                       xPos, yPos);
    }
}

bool CALLBACK OnDXUTDeviceRemoved( void* pUserContext )
{
    CDirect3DView* pView = (CDirect3DView*)pUserContext;
    if (pView != NULL)
    {
        return pView->OnDeviceRemoved();
    }
    return true;
}
