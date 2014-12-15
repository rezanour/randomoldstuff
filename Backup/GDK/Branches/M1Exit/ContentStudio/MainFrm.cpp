#include "stdafx.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
    ON_WM_CREATE()
    ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
    ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
    ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
    ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
    ON_WM_SETTINGCHANGE()
    ON_WM_CLOSE()
    ON_WM_DESTROY()
    ON_COMMAND(ID_TOOLS_RESETUISETTINGS, &CMainFrame::OnToolsResetUISettings)
END_MESSAGE_MAP()

static UINT indicators[] =
{
    ID_SEPARATOR,           // status line indicator
    ID_INDICATOR_CAPS,
    ID_INDICATOR_NUM,
    ID_INDICATOR_SCRL,
};

CMainFrame::CMainFrame() :
    _extensionsInitialized(false)
{
    if (theApp.GetInt(_T("CleanState"), FALSE))
    {
        theApp.CleanState();
    }
    theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2008);
}

CMainFrame::~CMainFrame()
{
    
}

void CMainFrame::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
    CContentStudioDoc* pDoc = (CContentStudioDoc*)pHint;
    if (pDoc != NULL)
    {
        // TODO: Handle updates from document changes if needed
    }
}

#ifdef IMPLEMENT_MULTIPLE_VIEWS
BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)  
{ 
    VERIFY(m_wndSplitter.CreateStatic(this,2,2)); // two rows / two columns 
    VERIFY(m_wndSplitter.CreateView(0,0,RUNTIME_CLASS(CContentStudioView), CSize(100,100),pContext));
    VERIFY(m_wndSplitter.CreateView(0,1,RUNTIME_CLASS(CGridView), CSize(100,100),pContext));
    VERIFY(m_wndSplitter.CreateView(1,0,RUNTIME_CLASS(CGridView), CSize(100,100),pContext));
    VERIFY(m_wndSplitter.CreateView(1,1,RUNTIME_CLASS(CGridView), CSize(100,100),pContext));

    ReadSplitterSettings();

    return TRUE; 
} 
#endif

void CMainFrame::OnViewsCreated(CContentStudioDoc* pDoc)
{
    if (!_extensionsInitialized)
    {
        if (pDoc != nullptr)
        {
            stde::com_ptr<IContentStudioProperties> props;
            if (SUCCEEDED(pDoc->GetDocumentProperties(&props)))
            {
                m_uiExtensionsManager.SetDocumentProperties(props);
            }
        }
        _extensionsInitialized = true;
    }
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
        return -1;

    // load ui extensions
    m_uiExtensionsManager.Initialize("ContentStudio.ini", 25);

    // Add ContentStudio ui extensions
    ProjectBrowserExtension* pProjectBrowserExtension = new ProjectBrowserExtension(ProjectBrowseType::GameObjects);
    if (pProjectBrowserExtension)
    {
        m_uiExtensionsManager.AddExtension("Objects", pProjectBrowserExtension);
        pProjectBrowserExtension->Release();
    }

    pProjectBrowserExtension = new ProjectBrowserExtension(ProjectBrowseType::Assets);
    if (pProjectBrowserExtension)
    {
        m_uiExtensionsManager.AddExtension("Assets", pProjectBrowserExtension);
        pProjectBrowserExtension->Release();
    }

    pProjectBrowserExtension = new ProjectBrowserExtension(ProjectBrowseType::Resources);
    if (pProjectBrowserExtension)
    {
        m_uiExtensionsManager.AddExtension("Resources", pProjectBrowserExtension);
        pProjectBrowserExtension->Release();
    }

    pProjectBrowserExtension = new ProjectBrowserExtension(ProjectBrowseType::Components);
    if (pProjectBrowserExtension)
    {
        m_uiExtensionsManager.AddExtension("Components", pProjectBrowserExtension);
        pProjectBrowserExtension->Release();
    }

    pProjectBrowserExtension = new ProjectBrowserExtension(ProjectBrowseType::Screens);
    if (pProjectBrowserExtension)
    {
        m_uiExtensionsManager.AddExtension("Screens", pProjectBrowserExtension);
        pProjectBrowserExtension->Release();
    }

    ObjectPropertiesExtension* pObjectPropertiesExtension = new ObjectPropertiesExtension();
    if (pObjectPropertiesExtension)
    {
        m_uiExtensionsManager.AddExtension("Properties", pObjectPropertiesExtension);
        pObjectPropertiesExtension->Release();
    }

    BOOL bNameValid;
    // set the visual manager and style based on persisted value
    OnApplicationLook(theApp.m_nAppLook);

    if (!m_wndMenuBar.Create(this))
    {
        TRACE0("Failed to create menubar\n");
        return -1;      // fail to create
    }

    m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

    // prevent the menu bar from taking the focus on activation
    CMFCPopupMenu::SetForceMenuFocus(FALSE);

    if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
        !m_wndToolBar.LoadToolBar(theApp.m_bHiColorIcons ? IDR_MAINFRAME_256 : IDR_MAINFRAME))
    {
        TRACE0("Failed to create toolbar\n");
        return -1;      // fail to create
    }

    CString strToolBarName;
    bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
    ASSERT(bNameValid);
    m_wndToolBar.SetWindowText(strToolBarName);

    CString strCustomize;
    bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
    ASSERT(bNameValid);
    m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);

    // Allow user-defined toolbars operations:
    InitUserToolbars(NULL, uiFirstUserToolBarId, uiLastUserToolBarId);

    if (!m_wndStatusBar.Create(this))
    {
        TRACE0("Failed to create status bar\n");
        return -1;      // fail to create
    }
    m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

    // TODO: Delete these five lines if you don't want the toolbar and menubar to be dockable
    m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
    m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
    EnableDocking(CBRS_ALIGN_ANY);
    DockPane(&m_wndMenuBar);
    DockPane(&m_wndToolBar);

    // hide the default toolbar
    //m_wndToolBar.ShowPane(FALSE,FALSE,FALSE);

    // enable Visual Studio 2005 style docking window behavior
    CDockingManager::SetDockingMode(DT_SMART);
    // enable Visual Studio 2005 style docking window auto-hide behavior
    EnableAutoHidePanes(CBRS_ALIGN_ANY);

    // Load menu item image (not placed on any standard toolbars):
    CMFCToolBar::AddToolBarForImageCollection(IDR_MENU_IMAGES, theApp.m_bHiColorIcons ? IDB_MENU_IMAGES_24 : 0);

#ifdef USE_BUILTIN_DOCKINGWINDOWS

    // create docking windows
    if (!CreateDockingWindows())
    {
        TRACE0("Failed to create docking windows\n");
        return -1;
    }

    m_wndSolutionPane.EnableDocking(CBRS_ALIGN_ANY);
    DockPane(&m_wndSolutionPane);
    CDockablePane* pTabbedBar = NULL;
    m_wndSolutionPane.ToggleAutoHide();

    m_wndOutputPane.EnableDocking(CBRS_ALIGN_ANY);
    DockPane(&m_wndOutputPane);
    m_wndOutputPane.ToggleAutoHide();

    m_wndPropertiesPane.EnableDocking(CBRS_ALIGN_ANY);
    DockPane(&m_wndPropertiesPane);
    m_wndPropertiesPane.ShowPane(FALSE, FALSE, FALSE);
#endif

    // Create docking windows
    m_dockingWindowManager.Create(this, &m_uiExtensionsManager);

    // Enable toolbar and docking window menu replacement
    EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

    // enable quick (Alt+drag) toolbar customization
    CMFCToolBar::EnableQuickCustomization();

    if (CMFCToolBar::GetUserImages() == NULL)
    {
        // load user-defined toolbar images
        if (m_UserImages.Load(_T(".\\UserImages.bmp")))
        {
            CMFCToolBar::SetUserImages(&m_UserImages);
        }
    }

/*
    // enable menu personalization (most-recently used commands)
    // TODO: define your own basic commands, ensuring that each pulldown menu has at least one basic command.
    CList<UINT, UINT> lstBasicCommands;

    lstBasicCommands.AddTail(ID_FILE_NEW);
    lstBasicCommands.AddTail(ID_FILE_OPEN);
    lstBasicCommands.AddTail(ID_FILE_SAVE);
    lstBasicCommands.AddTail(ID_FILE_PRINT);
    lstBasicCommands.AddTail(ID_APP_EXIT);
    lstBasicCommands.AddTail(ID_EDIT_CUT);
    lstBasicCommands.AddTail(ID_EDIT_PASTE);
    lstBasicCommands.AddTail(ID_EDIT_UNDO);
    lstBasicCommands.AddTail(ID_APP_ABOUT);
    lstBasicCommands.AddTail(ID_VIEW_STATUS_BAR);
    lstBasicCommands.AddTail(ID_VIEW_TOOLBAR);
    lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2003);
    lstBasicCommands.AddTail(ID_VIEW_APPLOOK_VS_2005);
    lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLUE);
    lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_SILVER);
    lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLACK);
    lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_AQUA);
    lstBasicCommands.AddTail(ID_VIEW_APPLOOK_WINDOWS_7);
    lstBasicCommands.AddTail(ID_SORTING_SORTALPHABETIC);
    lstBasicCommands.AddTail(ID_SORTING_SORTBYTYPE);
    lstBasicCommands.AddTail(ID_SORTING_SORTBYACCESS);
    lstBasicCommands.AddTail(ID_SORTING_GROUPBYTYPE);

    CMFCToolBar::SetBasicCommands(lstBasicCommands);
*/
    //m_wndStatusBar.ShowPane(FALSE,FALSE,FALSE);

    return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    if( !CFrameWndEx::PreCreateWindow(cs) )
        return FALSE;

    return TRUE;
}

BOOL CMainFrame::CreateDockingWindows()
{
    SetDockingWindowIcons(theApp.m_bHiColorIcons);
    return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
    m_dockingWindowManager.SetDockingWindowIcons(bHiColorIcons ? true : false);
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
    CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
    CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnViewCustomize()
{
    CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* scan menus */);
    pDlgCust->EnableUserDefinedToolbars();
    pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
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

void CMainFrame::OnApplicationLook(UINT id)
{
    CWaitCursor wait;

    theApp.m_nAppLook = id;

    switch (theApp.m_nAppLook)
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
        switch (theApp.m_nAppLook)
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

    RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

    theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
    pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext)
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


void CMainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
    CFrameWndEx::OnSettingChange(uFlags, lpszSection);
}

CContentStudioView* CMainFrame::GetView()
{
    CContentStudioView* pView = (CContentStudioView*)GetActiveView();
    if(pView->IsKindOf(RUNTIME_CLASS(CContentStudioView)))
    {
        return pView;
    }
    if(pView->IsKindOf(RUNTIME_CLASS(CMainFrame)))
    {
        return NULL;
    }
    return NULL;
}

void CMainFrame::OnClose()
{
#ifdef IMPLEMENT_MULTIPLE_VIEWS
    WriteSplitterSettings();
#endif

    m_uiExtensionsManager.SetDocumentProperties(nullptr);

    CFrameWndEx::OnClose();
}

#ifdef IMPLEMENT_MULTIPLE_VIEWS
void CMainFrame::WriteSplitterSettings()
{
    CRect cr;
    GetWindowRect(&cr);

    int splitterRow0Height = theApp.GetInt(L"SplitterRow0Height", cr.Height() / 2);
    int splitterRow1Height = theApp.GetInt(L"SplitterRow1Height", cr.Height() / 2);
    int splitterColumn0Width = theApp.GetInt(L"SplitterColumn0Width", cr.Width() / 2);
    int splitterColumn1Width = theApp.GetInt(L"SplitterColumn1Width", cr.Width() / 2);

    int minValue = 0;

    m_wndSplitter.GetRowInfo( 0, splitterRow0Height, minValue );
    m_wndSplitter.GetRowInfo( 1, splitterRow1Height, minValue );
    m_wndSplitter.GetColumnInfo( 0, splitterColumn0Width, minValue);
    m_wndSplitter.GetColumnInfo( 1, splitterColumn1Width, minValue);

    theApp.WriteInt(L"SplitterRow0Height", splitterRow0Height);
    theApp.WriteInt(L"SplitterRow1Height", splitterRow1Height);
    theApp.WriteInt(L"SplitterColumn0Width", splitterColumn0Width);
    theApp.WriteInt(L"SplitterColumn1Width", splitterColumn1Width);
}

void CMainFrame::ReadSplitterSettings()
{
    CRect cr;
    GetWindowRect(&cr);

    int splitterRow0Height = theApp.GetInt(L"SplitterRow0Height", cr.Height() / 2);
    int splitterRow1Height = theApp.GetInt(L"SplitterRow1Height", cr.Height() / 2);
    int splitterColumn0Width = theApp.GetInt(L"SplitterColumn0Width", cr.Width() / 2);
    int splitterColumn1Width = theApp.GetInt(L"SplitterColumn1Width", cr.Width() / 2);

    m_wndSplitter.SetRowInfo( 0, splitterRow0Height, 50 );
    m_wndSplitter.SetRowInfo( 1, splitterRow1Height, 50 );
    m_wndSplitter.SetColumnInfo( 0, splitterColumn0Width, 50);
    m_wndSplitter.SetColumnInfo( 1, splitterColumn1Width, 50);

    m_wndSplitter.RecalcLayout();
}
#endif

void CMainFrame::OnDestroy()
{
    CFrameWndEx::OnDestroy();

    m_dockingWindowManager.Destroy();
}


void CMainFrame::OnToolsResetUISettings()
{
    if (AfxMessageBox(L"Are you sure you want to reset all UI settings?\nThis will include window positions, theming and other settings.", MB_ICONQUESTION|MB_YESNO) == IDYES)
    {
        theApp.WriteInt(L"CleanState", TRUE);
        AfxMessageBox(L"You must restart the application to complete the UI settings reset", MB_ICONWARNING|MB_OK);
    }
}
