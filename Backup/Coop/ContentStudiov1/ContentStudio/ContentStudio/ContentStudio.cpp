#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CContentStudioApp, CWinAppEx)
    ON_COMMAND(ID_APP_ABOUT, &CContentStudioApp::OnAppAbout)
    // Standard file based document commands
    ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
    ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
END_MESSAGE_MAP()

CContentStudioApp::CContentStudioApp()
{
    m_bHiColorIcons = TRUE;
    SetAppID(_T("ContentStudio.AppID.NoVersion"));
}

CContentStudioApp theApp;

BOOL CContentStudioApp::InitInstance()
{
    // Initialize GDI+ for textures viewer
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    if (Gdiplus::GdiplusStartup( &m_gdiplusToken, &gdiplusStartupInput, NULL ) != Gdiplus::Ok )
    {
        DEBUG_PRINT("Failed to initialize GdiPlus");
        return FALSE;
    }
    else
    {
        DEBUG_PRINT("GdiPlus initialized with token '%d'", m_gdiplusToken);
    }

    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    CWinAppEx::InitInstance();

    EnableTaskbarInteraction(FALSE);

    // AfxInitRichEdit2() is required to use RichEdit control
    // AfxInitRichEdit2();

    SetRegistryKey(_T("Local AppWizard-Generated Applications"));
    LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)

    InitContextMenuManager();
    InitKeyboardManager();
    InitTooltipManager();

    CMFCToolTipInfo ttParams;
    ttParams.m_bVislManagerTheme = TRUE;
    theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
        RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

    // Register the application's document templates.  Document templates
    //  serve as the connection between documents, frame windows and views
    CSingleDocTemplate* pDocTemplate;
    pDocTemplate = new CSingleDocTemplate(
        IDR_MAINFRAME,
        RUNTIME_CLASS(CContentStudioDoc),
        RUNTIME_CLASS(CMainFrame),       // main SDI frame window
        RUNTIME_CLASS(CContentStudioView));
    if (!pDocTemplate)
        return FALSE;
    AddDocTemplate(pDocTemplate);

    // Parse command line for standard shell commands, DDE, file open
    CCommandLineInfo cmdInfo;
    ParseCommandLine(cmdInfo);

    // Dispatch commands specified on the command line.  Will return FALSE if
    // app was launched with /RegServer, /Register, /Unregserver or /Unregister.
    if (!ProcessShellCommand(cmdInfo))
        return FALSE;

    // The one and only window has been initialized, so show and update it
    m_pMainWnd->ShowWindow(SW_SHOW);
    m_pMainWnd->UpdateWindow();
    // call DragAcceptFiles only if there's a suffix
    //  In an SDI app, this should occur after ProcessShellCommand
    return TRUE;
}

int CContentStudioApp::ExitInstance()
{
    if (m_gdiplusToken != 0)
    {
        DEBUG_PRINT("Shutting down GdiPlus with token '%d'", m_gdiplusToken);
        Gdiplus::GdiplusShutdown(m_gdiplusToken);
    }
    else
    {
        DEBUG_PRINT("Shutting down GdiPlus has been skipped because it was not initialized");
    }

    AfxOleTerm(FALSE);

    return CWinAppEx::ExitInstance();
}

BOOL CContentStudioApp::OnIdle(LONG lCount)
{
    DXUTRender3DEnvironment();
    return TRUE;
}

class CAboutDlg : public CDialogEx
{
public:
    CAboutDlg();

    enum { IDD = IDD_ABOUTBOX };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// App command to run the dialog
void CContentStudioApp::OnAppAbout()
{
    CAboutDlg aboutDlg;
    aboutDlg.DoModal();
}

void CContentStudioApp::PreLoadState()
{
    BOOL bNameValid;
    CString strName;
    bNameValid = strName.LoadString(IDS_EDIT_MENU);
    ASSERT(bNameValid);
    GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
    bNameValid = strName.LoadString(IDS_EXPLORER);
    ASSERT(bNameValid);
    GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
}

void CContentStudioApp::LoadCustomState()
{
}

void CContentStudioApp::SaveCustomState()
{
}
