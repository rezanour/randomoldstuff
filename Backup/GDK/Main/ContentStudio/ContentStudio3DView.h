#pragma once

class CContentStudio3DView : public IContentView
{
public:
    CContentStudio3DView();
    virtual ~CContentStudio3DView();

    void OnRefresh(CContentStudioDoc* pDoc);
    void SetDocument(CContentStudioDoc* pDoc);
    void OnUpdate(double fTime, float fElapsedTime);
    void OnDraw(double fTime, float fElapsedTime);
    void OnLoadContent();
    void OnUnloadContent();
    void OnResize(UINT width, UINT height);
    void OnMouse(bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown,
                 bool bSideButton1Down, bool bSideButton2Down, int nMouseWheelDelta,
                 int xPos, int yPos);
    void OnKeyboard(UINT nChar, bool bKeyDown, bool bAltDown);
    LRESULT OnMsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing);

private:
    CSpriteFontTextRenderer m_FPStextRenderer;
    CContentStudioDoc* m_pDoc;
    CFirstPersonCamera m_ViewCameras[4];
    int m_activeCamera;
    bool m_bLastLeftButtonDownPressed;

    std::unique_ptr<DirectX::GeometricPrimitive> majorGrid;
    std::unique_ptr<DirectX::GeometricPrimitive> minorGrid;

    std::unique_ptr<DirectX::GeometricPrimitive> geometryResource;
    stde::com_ptr<ID3D11ShaderResourceView> geometryResourceTexture;

    stde::com_ptr<ID3D11ShaderResourceView> sprite;
    stde::com_ptr<ID3D11ShaderResourceView> selectionSprite;
    D3D11_VIEWPORT vports[4];
};