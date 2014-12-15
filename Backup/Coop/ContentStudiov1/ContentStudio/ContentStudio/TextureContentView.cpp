#include "stdafx.h"

CTextureContentView::CTextureContentView()
{

}

CTextureContentView::~CTextureContentView()
{

}

void CTextureContentView::OnRefresh(CContentStudioDoc* pDoc)
{
    m_pDoc = pDoc;

    // Perform any updates that depend on the document's current state
    if (pDoc->m_bContentUpdated)
    {
        m_renderer.Clear();
        PrepareTextureContentForRendering(pDoc->m_currentTextureContent);
        pDoc->m_bContentUpdated = false;
    }
}

void CTextureContentView::SetDocument(CContentStudioDoc* pDoc)
{
    m_pDoc = pDoc;

    m_renderer.Clear();

    // Handle first init?

    // FirstPersonCamera
    D3DXVECTOR3 vMin = D3DXVECTOR3( -1000.0f, -1000.0f, -1000.0f );
    D3DXVECTOR3 vMax = D3DXVECTOR3( 1000.0f, 1000.0f, 1000.0f );
    D3DXVECTOR3 vecEye( 2.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vecAt( 0.0f,0.0f,0.0f );
    m_FirstPersonCamera.SetViewParams( &vecEye, &vecAt );
    m_FirstPersonCamera.SetRotateButtons(TRUE, FALSE, FALSE);
    m_FirstPersonCamera.SetScalers( 0.01f, 1 );
    m_FirstPersonCamera.SetDrag( true );
    m_FirstPersonCamera.SetEnableYAxisMovement( true );
    m_FirstPersonCamera.SetClipToBoundary( TRUE, &vMin, &vMax );
    m_FirstPersonCamera.FrameMove( 0 );
}

void CTextureContentView::OnUpdate(double fTime, float fElapsedTime)
{
    m_FirstPersonCamera.FrameMove( fElapsedTime );
}

void CTextureContentView::OnDraw(GraphicsDevicePtr pGraphicsDevice, DeviceContextPtr pDeviceContext, double fTime, float fElapsedTime)
{
    m_cachedGraphicsDevice = pGraphicsDevice;

    m_renderer.Draw(pDeviceContext, m_textureEffect.get());
}

void CTextureContentView::OnLoadContent(GraphicsDevicePtr pGraphicsDevice, DeviceContextPtr pDeviceContext)
{
    HRESULT hr = S_OK;

    m_cachedGraphicsDevice = pGraphicsDevice;

    hr = Renderer::CreateFitTextureQuadEffect(pGraphicsDevice, pDeviceContext, &m_textureEffect);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Can't create texture effect");

Exit:
    return;
}

void CTextureContentView::OnUnloadContent()
{
    m_renderer.Clear();
    m_textureEffect.reset();
}

void CTextureContentView::OnResize(UINT width, UINT height)
{
    float fAspectRatio = (FLOAT)width / (FLOAT)height;
    m_FirstPersonCamera.SetProjParams( D3DX_PI / 4, fAspectRatio, 0.1f, 10000.0f );
    m_textureEffect->SetAspectRatio(fAspectRatio);
}

void CTextureContentView::OnMouse(bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown,
                bool bSideButton1Down, bool bSideButton2Down, int nMouseWheelDelta,
                int xPos, int yPos)
{

}

void CTextureContentView::OnKeyboard(UINT nChar, bool bKeyDown, bool bAltDown)
{

}

HRESULT CTextureContentView::PrepareTextureContentForRendering(ITextureContentPtr pTextureContent)
{
    return m_renderer.AddContent(m_cachedGraphicsDevice, m_pDoc->GetContentFilesPath(), pTextureContent);
}

LRESULT CTextureContentView::OnMsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing)
{
    return m_FirstPersonCamera.HandleMessages( hWnd, uMsg, wParam, lParam );
}