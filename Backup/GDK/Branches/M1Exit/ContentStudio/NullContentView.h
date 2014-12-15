#pragma once

class CNullContentView : public IContentView
{
public:
    CNullContentView();
    virtual ~CNullContentView();

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
    CContentStudioDoc* m_pDoc;
};