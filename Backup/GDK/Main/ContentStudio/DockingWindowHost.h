#pragma once
#include <stde\com_ptr.h>
#include <vector>

class CDockingWindowHost : public CDockablePane
{
public:
    CDockingWindowHost(IContentStudioWindow* pWindow);
    virtual ~CDockingWindowHost();

    void OnChangeVisualStyle();

public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);

private:
    stde::com_ptr<IContentStudioWindow> _spWindow;

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
    afx_msg void OnPaint();
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg LRESULT OnChangeActiveTab(WPARAM, LPARAM);

    DECLARE_MESSAGE_MAP()
};

class DockingWindowManager
{
public:
    DockingWindowManager();
    virtual ~DockingWindowManager();

    void Create(CFrameWndEx* pFrame, UIExtensionManager* pExtensionsManager);
    void Destroy();
    void SetDockingWindowIcons(bool bHiColorIcons);

private:
    std::vector<CDockingWindowHost*> _dockableWindows;
};