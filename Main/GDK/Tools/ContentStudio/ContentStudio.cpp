#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(ContentStudioApp, CWinAppEx)
    ON_COMMAND(ID_APP_ABOUT, &ContentStudioApp::OnAppAbout)
    ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
    ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
END_MESSAGE_MAP()

ContentStudioApp::ContentStudioApp() :
    _renderingMode(RenderSolid),
    _hicolorIcons(TRUE),
    _renderCallback(nullptr),
    _renderCallbackContext(nullptr),
    _gdiplusToken(0),
    _totalElapsedTime(1.0f),
    _elapsedTime(1.0f),
    _firstUpdate(true)
{
    SetAppID(_T("ContentStudio.AppID.NoVersion"));
}

ContentStudioApp theApp;

int ContentStudioApp::ExitInstance()
{
    GDK::Shutdown();

    if (_gdiplusToken != 0)
    {
        Gdiplus::GdiplusShutdown(_gdiplusToken);
    }

    return CWinAppEx::ExitInstance();
}


BOOL ContentStudioApp::InitInstance()
{
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    if (Gdiplus::GdiplusStartup( &_gdiplusToken, &gdiplusStartupInput, NULL ) != Gdiplus::Ok )
    {
        return FALSE;
    }

    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    CWinAppEx::InitInstance();

    EnableTaskbarInteraction(FALSE);

    SetRegistryKey(_T("Game Development Kit"));
    LoadStdProfileSettings(4);

    InitContextMenuManager();
    InitKeyboardManager();
    InitTooltipManager();

    CMFCToolTipInfo ttParams;
    ttParams.m_bVislManagerTheme = TRUE;
    theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
        RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

    CSingleDocTemplate* pDocTemplate;
    pDocTemplate = new CSingleDocTemplate(
        IDR_MAINFRAME,
        RUNTIME_CLASS(ContentStudioDoc),
        RUNTIME_CLASS(MainFrame),       // main SDI frame window
        RUNTIME_CLASS(ContentStudioView));
    if (!pDocTemplate)
        return FALSE;
    AddDocTemplate(pDocTemplate);

    CCommandLineInfo cmdInfo;
    ParseCommandLine(cmdInfo);

    if (!ProcessShellCommand(cmdInfo))
        return FALSE;

    m_pMainWnd->ShowWindow(SW_SHOW);
    m_pMainWnd->UpdateWindow();

    GDK::Startup();

    return TRUE;
}

_Use_decl_annotations_
BOOL ContentStudioApp::OnIdle(LONG lCount)
{
    UNREFERENCED_PARAMETER(lCount);

    Keyboard::Update();
    Mouse::Update();
    InvokeRenderCallback();

    __super::OnIdle(lCount);
    return TRUE;
}

void ContentStudioApp::InvokeRenderCallback()
{
    UpdateGameTime();

    if (_renderCallback)
    {
        _renderCallback(_renderCallbackContext);
    }
}

void ContentStudioApp::UpdateGameTime()
{
    if (_firstUpdate)
    {
        _firstUpdate = FALSE;

        QueryPerformanceFrequency(&_performanceFrequency);
        QueryPerformanceCounter(&_startCounter);
        QueryPerformanceCounter(&_lastCounter);
    }

    QueryPerformanceCounter(&_currentCounter);
    _totalElapsedTime = (float)(_currentCounter.QuadPart - _startCounter.QuadPart) / (float)_performanceFrequency.QuadPart;
    _elapsedTime = (float)(_currentCounter.QuadPart - _lastCounter.QuadPart) / (float)_performanceFrequency.QuadPart;
    _lastCounter.QuadPart = _currentCounter.QuadPart;
}

class CAboutDlg : public CDialogEx
{
public:
    CAboutDlg();

    enum { IDD = IDD_ABOUTBOX };

protected:
    virtual void DoDataExchange(_In_ CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

_Use_decl_annotations_
void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

void ContentStudioApp::OnAppAbout()
{
    CAboutDlg aboutDlg;
    aboutDlg.DoModal();
}

void ContentStudioApp::PreLoadState()
{
    BOOL bNameValid;
    CString strName;

    bNameValid = strName.LoadString(IDS_EDIT_MENU);
    ASSERT(bNameValid);
    GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);

    bNameValid = strName.LoadString(IDS_WORLD_MENU);
    ASSERT(bNameValid);
    GetContextMenuManager()->AddMenu(strName, IDR_POPUP_WORLD);
}

void ContentStudioApp::LoadCustomState()
{
}

void ContentStudioApp::SaveCustomState()
{
}
