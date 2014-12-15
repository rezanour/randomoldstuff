#pragma once

class MainFrame : public CFrameWndEx
{
protected:
    MainFrame();
    DECLARE_DYNCREATE(MainFrame)

public:
    virtual BOOL PreCreateWindow(_In_ CREATESTRUCT& cs);
    virtual BOOL LoadFrame(_In_ UINT nIDResource, _In_ DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, _In_ CWnd* pParentWnd = NULL, _In_ CCreateContext* pContext = NULL);
    virtual ~MainFrame();    
    ContentPreviewPane* GetContentPreviewPane() { return &_archivePreviewRenderingPane; }
    ContentPane* GetArchivePane() { return &_archivePane; }
    void EnterRunGameMode();
    void ExitRunGameMode();

#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

private:
    void TestRun();

protected:
    CMFCMenuBar         _menuBar;
    CMFCToolBar         _toolBar;
    CMFCStatusBar       _statusBar;
    CMFCToolBarImages   _userImages;
    ContentPane _archivePane;
    OutputPane          _outputPane;
    PropertiesPane      _propertiesPane;
    CMFCPopupMenu       _rendersMenu;
    ContentPreviewPane       _archivePreviewRenderingPane;
    RenderingPane       _renderingPane2;
    bool _runGameMode;
    std::vector<CBasePane*> _visiblePaneList;

protected:
    afx_msg int OnCreate(_In_ LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnViewCustomize();
    afx_msg LRESULT OnToolbarCreateNew(_In_ WPARAM wp, _In_ LPARAM lp);
    afx_msg void OnApplicationLook(_In_ UINT id);
    afx_msg void OnUpdateApplicationLook(_In_ CCmdUI* pCmdUI);
    afx_msg void OnSettingChange(_In_ UINT uFlags, _In_ LPCTSTR lpszSection);
    DECLARE_MESSAGE_MAP()

    BOOL CreateDockingWindows();
    void SetDockingWindowIcons();
public:
    afx_msg void OnInitMenuPopup(_In_ CMenu* pPopupMenu, _In_ UINT nIndex, _In_ BOOL bSysMenu);
    afx_msg void OnMoving(_In_ UINT fwSide, _In_ LPRECT pRect);
    afx_msg void OnTestRun();
};
