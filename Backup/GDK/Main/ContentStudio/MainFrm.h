#pragma once

class DockingWindowManager;
class CContentStudioView;
class CContentStudioDoc;

class CMainFrame : public CFrameWndEx
{

protected: // create from serialization only
    CMainFrame();
    DECLARE_DYNCREATE(CMainFrame)

// Overrides
public:
#ifdef IMPLEMENT_MULTIPLE_VIEWS
    virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
#endif
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
    CContentStudioView* GetView();

// Implementation
public:
    virtual ~CMainFrame();
    void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
    void OnViewsCreated(CContentStudioDoc* pDoc);

#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
    CMFCMenuBar        m_wndMenuBar;
    CMFCToolBar        m_wndToolBar;
    CMFCStatusBar      m_wndStatusBar;
    CMFCToolBarImages  m_UserImages;
    DockingWindowManager m_dockingWindowManager;
    UIExtensionManager m_uiExtensionsManager;
    bool               _extensionsInitialized;

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnViewCustomize();
    afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
    afx_msg void OnApplicationLook(UINT id);
    afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
    afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
    DECLARE_MESSAGE_MAP()

    BOOL CreateDockingWindows();
    void SetDockingWindowIcons(BOOL bHiColorIcons);
public:
    afx_msg void OnClose();

private:

#ifdef IMPLEMENT_MULTIPLE_VIEWS
    void WriteSplitterSettings();
    void ReadSplitterSettings();
#endif
public:
    afx_msg void OnDestroy();
    afx_msg void OnToolsResetUISettings();
};
