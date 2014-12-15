#pragma once

class CDirect3DView : public CView
{
public:
    CDirect3DView();
    DECLARE_DYNCREATE(CDirect3DView)

    virtual ~CDirect3DView();

public:
    virtual void OnInitialUpdate();
    virtual BOOL OnEraseBkgnd(CDC* pDC);
    virtual void OnDraw(CDC* pDC);

    virtual bool IsD3D11DeviceAcceptable( const CD3D11EnumAdapterInfo *AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo *DeviceInfo, DXGI_FORMAT BackBufferFormat, bool bWindowed);
    virtual bool ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings);
    virtual HRESULT OnD3D11CreateDevice( ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc);
    virtual HRESULT OnD3D11ResizedSwapChain( ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc);
    virtual void OnFrameMove( double fTime, float fElapsedTime);
    virtual void OnD3D11FrameRender( ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime, float fElapsedTime);
    virtual void OnD3D11ReleasingSwapChain();
    virtual void OnD3D11DestroyDevice();
    virtual LRESULT MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing);
    virtual void OnKeyboard( UINT nChar, bool bKeyDown, bool bAltDown);

    virtual void OnMouse( bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown,
                           bool bSideButton1Down, bool bSideButton2Down, int nMouseWheelDelta,
                           int xPos, int yPos);

    virtual bool OnDeviceRemoved();

protected:
    DECLARE_MESSAGE_MAP()
    virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
public:
    afx_msg void OnNcDestroy();
};

// DXUT callbacks
bool CALLBACK DXUTIsD3D11DeviceAcceptable( const CD3D11EnumAdapterInfo *AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo *DeviceInfo, DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext );
bool CALLBACK DXUTModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext );
HRESULT CALLBACK OnDXUTD3D11CreateDevice( ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
HRESULT CALLBACK OnDXUTD3D11ResizedSwapChain( ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
void CALLBACK OnDXUTFrameMove( double fTime, float fElapsedTime, void* pUserContext );
void CALLBACK OnDXUTD3D11FrameRender( ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime, float fElapsedTime, void* pUserContext );
void CALLBACK OnDXUTD3D11ReleasingSwapChain( void* pUserContext );
void CALLBACK OnDXUTD3D11DestroyDevice( void* pUserContext );
LRESULT CALLBACK DXUTMsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext );
void CALLBACK OnDXUTKeyboard( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext );

void CALLBACK OnDXUTMouse( bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown,
                       bool bSideButton1Down, bool bSideButton2Down, int nMouseWheelDelta,
                       int xPos, int yPos, void* pUserContext );

bool CALLBACK OnDXUTDeviceRemoved( void* pUserContext );
