#include "stdafx.h"

CModelContentView::CModelContentView()
{

}

CModelContentView::~CModelContentView()
{

}

void CModelContentView::OnRefresh(CContentStudioDoc* pDoc)
{
    m_pDoc = pDoc;

    // Perform any updates that depend on the document's current state
    if (pDoc->m_bContentUpdated)
    {
        m_renderer.Clear();

        if (pDoc->m_currentContentIndex != -1)
        {
            std::vector<std::string> meshNames;
            pDoc->m_currentModelContent->GetMeshNames(meshNames);
            PrepareModelContentForRendering(pDoc->m_currentModelContent, (meshNames[pDoc->m_currentContentIndex]).c_str());
        }
        else
        {
            PrepareModelContentForRendering(pDoc->m_currentModelContent, NULL); // add all meshes
        }
        pDoc->m_bContentUpdated = false;
    }
}

void CModelContentView::SetDocument(CContentStudioDoc* pDoc)
{
    m_pDoc = pDoc;

    m_renderer.Clear();

    // Handle first init?

    // FirstPersonCamera
    D3DXVECTOR3 vMin = D3DXVECTOR3( -1000.0f, -1000.0f, -1000.0f );
    D3DXVECTOR3 vMax = D3DXVECTOR3( 1000.0f, 1000.0f, 1000.0f );
    D3DXVECTOR3 vecEye( 100.0f, 5.0f, 5.0f );
    D3DXVECTOR3 vecAt( 0.0f,0.0f,0.0f );
    m_FirstPersonCamera.SetViewParams( &vecEye, &vecAt );
    m_FirstPersonCamera.SetRotateButtons(TRUE, FALSE, FALSE);
    m_FirstPersonCamera.SetScalers( 0.01f, 30/*10*/ );
    m_FirstPersonCamera.SetDrag( true );
    m_FirstPersonCamera.SetEnableYAxisMovement( true );
    m_FirstPersonCamera.SetClipToBoundary( TRUE, &vMin, &vMax );
    m_FirstPersonCamera.FrameMove( 0 );
}

void CModelContentView::OnUpdate(double fTime, float fElapsedTime)
{
    m_FirstPersonCamera.FrameMove( fElapsedTime );
}

void CModelContentView::OnDraw(GraphicsDevicePtr pGraphicsDevice, DeviceContextPtr pDeviceContext, double fTime, float fElapsedTime)
{
    m_cachedGraphicsDevice = pGraphicsDevice;

    // Get the projection & view matrix from the camera class
    D3DXMATRIX mWorld;
    D3DXMatrixIdentity(&mWorld);

    XMMATRIX world = DXUTD3DXMatrixToXMMatrix(mWorld);
    XMMATRIX view = m_FirstPersonCamera.GetViewXMMatrix();
    XMMATRIX proj = m_FirstPersonCamera.GetProjXMMatrix();

    m_basicEffect->SetWorldMatrix(world);
    m_basicEffect->SetViewMatrix(view);
    m_basicEffect->SetProjectionMatrix(proj);

    m_renderer.Draw(pDeviceContext, m_basicEffect.get());
}

void CModelContentView::OnLoadContent(GraphicsDevicePtr pGraphicsDevice, DeviceContextPtr pDeviceContext)
{
    HRESULT hr = S_OK;

    m_cachedGraphicsDevice = pGraphicsDevice;

    hr = Renderer::CreateDiffuseOnlyEffect(pGraphicsDevice, pDeviceContext, &m_basicEffect);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Can't create basic effect");

Exit:
    return;
}

void CModelContentView::OnUnloadContent()
{
    m_renderer.Clear();
    m_basicEffect.reset();
}

void CModelContentView::OnResize(UINT width, UINT height)
{
    float fAspectRatio = (FLOAT)width / (FLOAT)height;
    m_FirstPersonCamera.SetProjParams( D3DX_PI / 4, fAspectRatio, 0.1f, 10000.0f );
}

void CModelContentView::OnMouse(bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown,
                bool bSideButton1Down, bool bSideButton2Down, int nMouseWheelDelta,
                int xPos, int yPos)
{

}

void CModelContentView::OnKeyboard(UINT nChar, bool bKeyDown, bool bAltDown)
{

}

HRESULT CModelContentView::PrepareModelContentForRendering(IModelContentPtr pModelContent, LPCSTR meshName)
{
    return m_renderer.AddContent(m_cachedGraphicsDevice, m_pDoc->GetContentFilesPath(), pModelContent, meshName);
}

LRESULT CModelContentView::OnMsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing)
{
    return m_FirstPersonCamera.HandleMessages( hWnd, uMsg, wParam, lParam );
}