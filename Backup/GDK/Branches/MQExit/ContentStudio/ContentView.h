#pragma once

class IContentView
{
public:
    virtual void OnRefresh(CContentStudioDoc* pDoc) = 0;
    virtual void SetDocument(CContentStudioDoc* pDoc) = 0;
    virtual void OnUpdate(double fTime, float fElapsedTime) = 0;
    virtual void OnDraw(double fTime, float fElapsedTime) = 0;
    virtual void OnLoadContent() = 0;
    virtual void OnUnloadContent() = 0;
    virtual void OnResize(UINT width, UINT height) = 0;
    virtual void OnMouse(bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown,
                         bool bSideButton1Down, bool bSideButton2Down, int nMouseWheelDelta,
                         int xPos, int yPos) = 0;
    virtual void OnKeyboard(UINT nChar, bool bKeyDown, bool bAltDown) = 0;
    virtual LRESULT OnMsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing) = 0;
};

typedef std::shared_ptr<IContentView> IContentViewPtr;