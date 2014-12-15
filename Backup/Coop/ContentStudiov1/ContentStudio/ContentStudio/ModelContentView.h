#pragma once

class CModelContentView : public IContentView
{
public:
    CModelContentView();
    virtual ~CModelContentView();

    void OnRefresh(CContentStudioDoc* pDoc);
    void SetDocument(CContentStudioDoc* pDoc);
    void OnUpdate(double fTime, float fElapsedTime);
    void OnDraw(GraphicsDevicePtr pGraphicsDevice, DeviceContextPtr pDeviceContext, double fTime, float fElapsedTime);
    void OnLoadContent(GraphicsDevicePtr pGraphicsDevice, DeviceContextPtr pDeviceContext);
    void OnUnloadContent();
    void OnResize(UINT width, UINT height);
    void OnMouse(bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown,
                 bool bSideButton1Down, bool bSideButton2Down, int nMouseWheelDelta,
                 int xPos, int yPos);
    void OnKeyboard(UINT nChar, bool bKeyDown, bool bAltDown);
    LRESULT OnMsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing);

private:
    HRESULT PrepareModelContentForRendering(IModelContentPtr pModelContent, LPCSTR meshName);

private:
    CContentStudioDoc* m_pDoc;
    DiffuseOnlyEffectPtr  m_basicEffect;
    CLucid3DRenderer   m_renderer;
    GraphicsDevicePtr  m_cachedGraphicsDevice;
    CFirstPersonCamera m_FirstPersonCamera;
};