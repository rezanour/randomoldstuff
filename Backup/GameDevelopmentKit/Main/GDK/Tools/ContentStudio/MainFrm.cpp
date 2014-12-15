#include "stdafx.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// MainFrame

IMPLEMENT_DYNCREATE(MainFrame, CFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(MainFrame, CFrameWndEx)
    ON_WM_CREATE()
    ON_COMMAND(ID_VIEW_CUSTOMIZE, &MainFrame::OnViewCustomize)
    ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &MainFrame::OnToolbarCreateNew)
    ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &MainFrame::OnApplicationLook)
    ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &MainFrame::OnUpdateApplicationLook)
    ON_WM_SETTINGCHANGE()
    ON_WM_INITMENUPOPUP()
    ON_WM_MOVING()
    ON_COMMAND(ID_TEST_RUN, &MainFrame::OnTestRun)
END_MESSAGE_MAP()

static UINT indicators[] =
{
    ID_SEPARATOR,           // status line indicator
    ID_INDICATOR_CAPS,
    ID_INDICATOR_NUM,
    ID_INDICATOR_SCRL,
};

// MainFrame construction/destruction

MainFrame::MainFrame() :
    _runGameMode(false)
{
    // TODO: add member initialization code here
    theApp._applicationStyleSetting = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2008);
}

MainFrame::~MainFrame()
{
}

int MainFrame::OnCreate(_In_ LPCREATESTRUCT lpCreateStruct)
{
    if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
        return -1;

    BOOL bNameValid;

    if (!_menuBar.Create(this))
    {
        TRACE0("Failed to create menubar\n");
        return -1;      // fail to create
    }

    _menuBar.SetPaneStyle(_menuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

    // prevent the menu bar from taking the focus on activation
    CMFCPopupMenu::SetForceMenuFocus(FALSE);

    if (!_toolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
        !_toolBar.LoadToolBar(theApp._hicolorIcons ? IDR_MAINFRAME_256 : IDR_MAINFRAME))
    {
        TRACE0("Failed to create toolbar\n");
        return -1;      // fail to create
    }

    CString strToolBarName;
    bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
    ASSERT(bNameValid);
    _toolBar.SetWindowText(strToolBarName);

    CString strCustomize;
    bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
    ASSERT(bNameValid);
    _toolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);

    // Allow user-defined toolbars operations:
    InitUserToolbars(NULL, uiFirstUserToolBarId, uiLastUserToolBarId);

    if (!_statusBar.Create(this))
    {
        TRACE0("Failed to create status bar\n");
        return -1;      // fail to create
    }
    _statusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

    // TODO: Delete these five lines if you don't want the toolbar and menubar to be dockable
    _menuBar.EnableDocking(CBRS_ALIGN_ANY);
    _toolBar.EnableDocking(CBRS_ALIGN_ANY);
    EnableDocking(CBRS_ALIGN_ANY);
    DockPane(&_menuBar);
    DockPane(&_toolBar);

    CDockingManager::SetDockingMode(DT_SMART);
    EnableAutoHidePanes(CBRS_ALIGN_ANY);

    CMFCToolBar::AddToolBarForImageCollection(IDR_MENU_IMAGES, theApp._hicolorIcons ? IDB_MENU_IMAGES_24 : 0);

    if (!CreateDockingWindows())
    {
        TRACE0("Failed to create docking windows\n");
        return -1;
    }

    _archivePane.EnableDocking(CBRS_ALIGN_ANY);
    DockPane(&_archivePane);

    _archivePreviewRenderingPane.EnableDocking(CBRS_ALIGN_ANY);
    DockPane(&_archivePreviewRenderingPane);
    _archivePreviewRenderingPane.ShowPane(FALSE, FALSE, FALSE);

    _outputPane.EnableDocking(CBRS_ALIGN_ANY);
    DockPane(&_outputPane);
    _outputPane.ShowPane(FALSE, FALSE, FALSE);

    _propertiesPane.EnableDocking(CBRS_ALIGN_ANY);
    DockPane(&_propertiesPane);
    _propertiesPane.ShowPane(FALSE, FALSE, FALSE);

    OnApplicationLook(theApp._applicationStyleSetting);

    // Enable toolbar and docking window menu replacement
    EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

    // enable quick (Alt+drag) toolbar customization
    CMFCToolBar::EnableQuickCustomization();

    if (CMFCToolBar::GetUserImages() == NULL)
    {
        // load user-defined toolbar images
        if (_userImages.Load(_T(".\\UserImages.bmp")))
        {
            CMFCToolBar::SetUserImages(&_userImages);
        }
    }

    return 0;
}

BOOL MainFrame::PreCreateWindow(_In_ CREATESTRUCT& cs)
{
    if( !CFrameWndEx::PreCreateWindow(cs) )
        return FALSE;

    return TRUE;
}

BOOL MainFrame::CreateDockingWindows()
{
    BOOL bNameValid;

    // Create archive pane
    CString strArchivePane;
    bNameValid = strArchivePane.LoadString(IDS_ARCHIVE);
    ASSERT(bNameValid);
    if (!_archivePane.Create(strArchivePane, this, CRect(0, 0, 200, 200), TRUE, ID_ARCHIVE_PANE, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT| CBRS_FLOAT_MULTI))
    {
        TRACE0("Failed to create archive pane\n");
        return FALSE;
    }

    // Create output pane
    CString strOutputPane;
    bNameValid = strOutputPane.LoadString(IDS_OUTPUT);
    ASSERT(bNameValid);
    if (!_outputPane.Create(strOutputPane, this, CRect(0, 0, 100, 100), TRUE, ID_OUTPUT_PANE, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
    {
        TRACE0("Failed to create output pane\n");
        return FALSE;
    }

    // Create properties pane
    CString strPropertiesPane;
    bNameValid = strPropertiesPane.LoadString(IDS_PROPERTIES);
    ASSERT(bNameValid);
    if (!_propertiesPane.Create(strPropertiesPane, this, CRect(0, 0, 200, 200), TRUE, ID_PROPERTIES_PANE, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
    {
        TRACE0("Failed to create properties pane\n");
        return FALSE;
    }

    // Create rendering pane
    CString strRenderingPane;
    bNameValid = strRenderingPane.LoadString(IDS_ARCHIVE_CONTENT_PREVIEW__RENDERING);
    ASSERT(bNameValid);
    if (!_archivePreviewRenderingPane.Create(strRenderingPane, this, CRect(0, 0, 200, 200), TRUE, ID_ARCHIVE_CONTENT_PREVIEW_RENDERING_PANE, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
    {
        TRACE0("Failed to create archive content preview rendering pane\n");
        return FALSE;
    }

    SetDockingWindowIcons();
    return TRUE;
}

void MainFrame::SetDockingWindowIcons()
{
    HICON hFileViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_ARCHIVE_HC), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
    _archivePane.SetIcon(hFileViewIcon, FALSE);

    HICON hOutputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_OUTPUT_HC), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
    _outputPane.SetIcon(hOutputBarIcon, FALSE);

    HICON hPropertiesBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_PROPERTIES_HC), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
    _propertiesPane.SetIcon(hPropertiesBarIcon, FALSE);

}

// MainFrame diagnostics

#ifdef _DEBUG
void MainFrame::AssertValid() const
{
    CFrameWndEx::AssertValid();
}

void MainFrame::Dump(CDumpContext& dc) const
{
    CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// MainFrame message handlers

void MainFrame::OnViewCustomize()
{
    CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* scan menus */);
    pDlgCust->EnableUserDefinedToolbars();
    pDlgCust->Create();
}

LRESULT MainFrame::OnToolbarCreateNew(_In_ WPARAM wp, _In_ LPARAM lp)
{
    LRESULT lres = CFrameWndEx::OnToolbarCreateNew(wp,lp);
    if (lres == 0)
    {
        return 0;
    }

    CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
    ASSERT_VALID(pUserToolbar);

    BOOL bNameValid;
    CString strCustomize;
    bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
    ASSERT(bNameValid);

    pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
    return lres;
}

void MainFrame::OnApplicationLook(_In_ UINT id)
{
    CWaitCursor wait;

    theApp._applicationStyleSetting = id;

    switch (theApp._applicationStyleSetting)
    {
    case ID_VIEW_APPLOOK_WIN_2000:
        CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
        break;

    case ID_VIEW_APPLOOK_OFF_XP:
        CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
        break;

    case ID_VIEW_APPLOOK_WIN_XP:
        CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
        CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
        break;

    case ID_VIEW_APPLOOK_OFF_2003:
        CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
        CDockingManager::SetDockingMode(DT_SMART);
        break;

    case ID_VIEW_APPLOOK_VS_2005:
        CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
        CDockingManager::SetDockingMode(DT_SMART);
        break;

    case ID_VIEW_APPLOOK_VS_2008:
        CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
        CDockingManager::SetDockingMode(DT_SMART);
        break;

    case ID_VIEW_APPLOOK_WINDOWS_7:
        CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
        CDockingManager::SetDockingMode(DT_SMART);
        break;

    default:
        switch (theApp._applicationStyleSetting)
        {
        case ID_VIEW_APPLOOK_OFF_2007_BLUE:
            CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
            break;

        case ID_VIEW_APPLOOK_OFF_2007_BLACK:
            CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
            break;

        case ID_VIEW_APPLOOK_OFF_2007_SILVER:
            CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
            break;

        case ID_VIEW_APPLOOK_OFF_2007_AQUA:
            CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
            break;
        }

        CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
        CDockingManager::SetDockingMode(DT_SMART);
    }

    _outputPane.UpdateFonts();
    RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

    theApp.WriteInt(_T("ApplicationLook"), theApp._applicationStyleSetting);
}

void MainFrame::OnUpdateApplicationLook(_In_ CCmdUI* pCmdUI)
{
    pCmdUI->SetRadio(theApp._applicationStyleSetting == pCmdUI->m_nID);
}


BOOL MainFrame::LoadFrame(_In_ UINT nIDResource, _In_ DWORD dwDefaultStyle, _In_ CWnd* pParentWnd, _In_ CCreateContext* pContext)
{
    // base class does the real work

    if (!CFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
    {
        return FALSE;
    }


    // enable customization button for all user toolbars
    BOOL bNameValid;
    CString strCustomize;
    bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
    ASSERT(bNameValid);

    for (int i = 0; i < iMaxUserToolbars; i ++)
    {
        CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
        if (pUserToolbar != NULL)
        {
            pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
        }
    }
    
    return TRUE;
}


void MainFrame::OnSettingChange(_In_ UINT uFlags, _In_ LPCTSTR lpszSection)
{
    CFrameWndEx::OnSettingChange(uFlags, lpszSection);
    _outputPane.UpdateFonts();
}


void MainFrame::OnInitMenuPopup(_In_ CMenu* pPopupMenu, _In_ UINT nIndex, _In_ BOOL bSysMenu)
{
    CFrameWndEx::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);

    // There is one master components list that contains mixed components of
    // all supported types.

    //
    // The compoments dropdown menu is dynamically built up from that cached list.
    // Separate smaller arrays are also created that become groups of known
    // components.  These arrays contain the index into the master compoment array.
    // 

    /////////////////////////////////////////////////////////////////////////////
    // Master List      //  Graphics Zero-base List  //  Audio Zero-based List //
    // [0] - graphics   //  [0] - 0                  //  [0] - 2               //
    // [1] - graphics   //  [1] - 1                  //                        //
    // [2] - audio      //  [2] - 3                  //                        //
    // [3] - graphics   //                           //                        //
    //                  //                           //                        //
    /////////////////////////////////////////////////////////////////////////////
    
    UINT menuId = pPopupMenu->GetMenuItemID(0);
    switch(menuId)
    {
    case ID_GRAPHICS_:
        {
            //int g = 0;
            pPopupMenu->RemoveMenu(0, MF_BYPOSITION);
            // Loop components, filtering for graphics renderers
            /*
            for (size_t i = 0; i < theApp._components.size(); i++)
            {
                if (theApp._components[i].factoryId == __uuidof(GDK::Graphics::IGraphicsDeviceFactory))
                {
                    pPopupMenu->AppendMenuW(MF_BYPOSITION, (UINT_PTR)g + ID_GRAPHICS_, theApp._components[i].displayName);
                    g++;
                }
            }
            */
        }
        break;
    case ID_GAMES_:
        {
            //int g = 0;
            pPopupMenu->RemoveMenu(0, MF_BYPOSITION);
            // Loop components, filtering for games
            /*
            for (size_t i = 0; i < theApp._components.size(); i++)
            {
                if (theApp._components[i].factoryId == __uuidof(GDK::IGameFactory))
                {
                    pPopupMenu->AppendMenuW(MF_BYPOSITION, (UINT_PTR)g + ID_GAMES_, theApp._components[i].displayName);
                    g++;
                }
            }
            */
        }
        break;
    default:
        break;
    }
}

void MainFrame::OnMoving(_In_ UINT fwSide, _In_ LPRECT pRect)
{
    CFrameWndEx::OnMoving(fwSide, pRect);
    theApp.InvokeRenderCallback();
}

void MainFrame::EnterRunGameMode()
{
    if (!_runGameMode)
    {
        _runGameMode = true;
        _visiblePaneList.clear();
        
        if (_menuBar.IsVisible()) { _visiblePaneList.push_back(&_menuBar); }
        if (_toolBar.IsVisible()) { _visiblePaneList.push_back(&_toolBar); }
        if (_statusBar.IsVisible()) { _visiblePaneList.push_back(&_statusBar); }
        if (_archivePane.IsVisible()) { _visiblePaneList.push_back(&_archivePane); }
        if (_outputPane.IsVisible()) { _visiblePaneList.push_back(&_outputPane); }
        if (_propertiesPane.IsVisible()) { _visiblePaneList.push_back(&_propertiesPane); }
        if (_archivePreviewRenderingPane.IsVisible()) { _visiblePaneList.push_back(&_archivePreviewRenderingPane); }

        for (size_t i = 0; i < _visiblePaneList.size(); i++)
        {
            _visiblePaneList[i]->ShowPane(FALSE, FALSE, FALSE);
        }
    }
}

void MainFrame::ExitRunGameMode()
{
    if (_runGameMode)
    {
        _runGameMode = false;
        for (size_t i = 0; i < _visiblePaneList.size(); i++)
        {
            _visiblePaneList[i]->ShowPane(TRUE, FALSE, TRUE);
        }
    }
}

void MainFrame::OnTestRun()
{
    TestRun();
}

void MainFrame::TestRun()
{

}
