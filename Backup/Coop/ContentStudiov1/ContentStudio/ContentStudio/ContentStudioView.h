#pragma once

#define ID_REFRESH_MAIN_VIEW 87634

#include <Engine\Renderer.h>

#define NULL_VIEW_ID               0
#define MODEL_CONTENT_VIEW_ID      1
#define TEXTURE_CONTENT_VIEW_ID    2
#define SPRITEFONT_CONTENT_VIEW_ID 3

class CContentStudioView : public CDirect3DView
{
protected:
    CContentStudioView();
    DECLARE_DYNCREATE(CContentStudioView)

public:
    CContentStudioDoc* GetDocument() const;
    CMainFrame* GetMainFrame() const;

    //virtual void OnDraw(CDC* pDC);  // overridden to draw this view
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual ~CContentStudioView();

    virtual void OnInitialUpdate();
    virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
    virtual void OnFrameMove( double fTime, float fElapsedTime);
    virtual void OnD3D11FrameRender( ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime, float fElapsedTime);
    virtual HRESULT OnD3D11CreateDevice( ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc);
    virtual void OnD3D11DestroyDevice();
    virtual LRESULT MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing);
    virtual HRESULT OnD3D11ResizedSwapChain( ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc);
    virtual void OnMouse( bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown,
                           bool bSideButton1Down, bool bSideButton2Down, int nMouseWheelDelta,
                           int xPos, int yPos);
    virtual void OnKeyboard( UINT nChar, bool bKeyDown, bool bAltDown);

#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

private:
    

protected:
    CMainFrame* m_pMainFrame;

    CDXUTTextHelper* m_pTxtHelper;
    ID3D11SamplerState* m_pSamLinear;

    ID3D11RasterizerState*  m_pRasterizerStateSolid;
    ID3D11RasterizerState*  m_pRasterizerStateWireframe;
    ID3D11RasterizerState*  m_pRasterizerStateWireframeNoCull;

    GraphicsDevicePtr   m_graphicsDevice;
    DeviceContextPtr    m_deviceContext;

    bool m_bShowFrameRateCounter;

    void HandleDropFiles(HDROP hDropInfo);

    UINT m_currentViewId;

    std::vector<IContentViewPtr> m_views;

protected:
    afx_msg void OnFilePrintPreview();
    afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
    afx_msg void OnViewWireframe();
    afx_msg void OnUpdateViewWireframe(CCmdUI *pCmdUI);
    afx_msg void OnViewWireframeNoBackfaceCulling();
    afx_msg void OnUpdateViewWireframeNoBackfaceCulling(CCmdUI *pCmdUI);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnViewFrameratecounter();
    afx_msg void OnUpdateViewFrameratecounter(CCmdUI *pCmdUI);
    afx_msg void OnDropFiles(HDROP hDropInfo);
#ifdef ENABLE_UNITTESTS
    afx_msg void OnUnittestModelcontentserialization();
    afx_msg void OnUnittestModelcontentrawfileoffsets();
#endif
    afx_msg void OnFileNewSpriteFont();
};

#ifndef _DEBUG  // debug version in ContentStudioView.cpp
inline CContentStudioDoc* CContentStudioView::GetDocument() const
   { return reinterpret_cast<CContentStudioDoc*>(m_pDocument); }

inline CMainFrame* CContentStudioView::GetMainFrame() const
   { return reinterpret_cast<CMainFrame*>(m_pMainFrame); }
#endif
