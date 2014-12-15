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
    if (_gdiplusToken != 0)
    {
        Gdiplus::GdiplusShutdown(_gdiplusToken);
    }
    UnloadAllComponentModules();
    return CWinAppEx::ExitInstance();
}


BOOL ContentStudioApp::InitInstance()
{
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    if (Gdiplus::GdiplusStartup( &_gdiplusToken, &gdiplusStartupInput, NULL ) != Gdiplus::Ok )
    {
        return FALSE;
    }

    // Enumerate components
    size_t numComponents = ComponentFinderGetNumComponents();
    _components.clear();
    _components.resize(numComponents);
    ComponentFinderGetComponents(_components.data(), _components.size(), &numComponents);

    // Zero base components
    _zeroBasedGraphicsComponents.clear();
    _zeroBasedGamesComponents.clear();
    _zeroBasedContentComponents.clear();

    // iterate components collection and zero-base them into separate collections
    // to be used for menu and selection purposes.
    for (size_t i = 0; i < theApp._components.size(); i++)
    {
        ComponentDescription desc = theApp._components.at(i);

        // Collect graphics factory descriptions
        if (desc.factoryId == __uuidof(GDK::Graphics::IGraphicsDeviceFactory))
        {
            _zeroBasedGraphicsComponents.push_back(i);
        }

        // Collect game factory descriptions
        if (desc.factoryId == __uuidof(GDK::IGameFactory))
        {
            _zeroBasedGamesComponents.push_back(i);
        }

        // Collect content factory descriptions
        if (desc.factoryId == __uuidof(GDK::Content::IContentFactory))
        {
            _zeroBasedContentComponents.push_back(i);
        }
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

    return TRUE;
}

BOOL ContentStudioApp::OnIdle(_In_ LONG lCount)
{
    UNREFERENCED_PARAMETER(lCount);

    Keyboard::Update();
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

HRESULT ContentStudioApp::CreateGameFactory(_In_ size_t index, _COM_Outptr_ IGameFactory** gameFactory)
{
    if (_zeroBasedGamesComponents.size() == 0)
    {
        return E_UNEXPECTED;
    }

    HRESULT hr = S_OK;
    ComPtr<IGameFactory> newGameFactory;
    ComponentDescription desc = _components.at(_zeroBasedGamesComponents[index]);
        
    IfFailedGotoExit(ComponentFinderCreateComponentFactory(desc, __uuidof(GDK::IGameFactory), &newGameFactory));    
    IfFailedGotoExit(newGameFactory.CopyTo(gameFactory));

Exit:

    if (FAILED(hr))
    {
        DebugOut(L"Failed to create IGameFactory, hr = 0x%lx\n" ,hr);
    }

    return hr;
}

HRESULT ContentStudioApp::CreateContentFactory(_In_ size_t index, _COM_Outptr_ GDK::Content::IContentFactory** contentFactory)
{
    if (_zeroBasedContentComponents.size() == 0)
    {
        return E_UNEXPECTED;
    }

    HRESULT hr = S_OK;
    ComPtr<GDK::Content::IContentFactory> newContentFactory;
    ComponentDescription desc = _components.at(_zeroBasedContentComponents[index]);
    
    IfFailedGotoExit(ComponentFinderCreateComponentFactory(desc, __uuidof(GDK::Content::IContentFactory), &newContentFactory));
    IfFailedGotoExit(newContentFactory.CopyTo(contentFactory));
    
Exit:

    if (FAILED(hr))
    {
        DebugOut(L"Failed to create IContentFactory, hr = 0x%lx\n", hr);
    }

    return hr;
}

HRESULT ContentStudioApp::CreateGraphicsDevice(_In_ size_t index, _In_ HWND hWnd, _COM_Outptr_ IGraphicsDevice** graphicsDevice)
{
    if (_zeroBasedGraphicsComponents.size() == 0)
    {
        return E_UNEXPECTED;
    }

    HRESULT hr = S_OK;
    ComponentDescription desc = _components.at(_zeroBasedGraphicsComponents[index]);
    ComPtr<IGraphicsDevice> newGraphics;    
    ComPtr<IGraphicsDeviceFactory> factory;

    GraphicsDeviceCreationParameters params;
    params.backBufferHeight = 600;
    params.backBufferWidth  = 800;
    params.vsyncEnabled     = true;
    params.windowIdentity   = hWnd;
    params.scaleMode        = GraphicsDeviceScaleMode::ResizeToFit;

    IfFailedGotoExit(ComponentFinderCreateComponentFactory(desc, __uuidof(GDK::Graphics::IGraphicsDeviceFactory), &factory));
    IfFailedGotoExit(factory->CreateGraphicsDevice(params, &newGraphics));
    IfFailedGotoExit(newGraphics.CopyTo(graphicsDevice));

Exit:

    if FAILED(hr)
    {
        DebugOut(L"Failed to create IGraphicsDevice, hr = 0x%lx\n", hr);
    }

    return hr;
}

class CAboutDlg : public CDialogEx
{
public:
    CAboutDlg();

    enum { IDD = IDD_ABOUTBOX };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(_In_ CDataExchange* pDX)
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
    bNameValid = strName.LoadString(IDS_EXPLORER);
    ASSERT(bNameValid);
    GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
}

void ContentStudioApp::LoadCustomState()
{
}

void ContentStudioApp::SaveCustomState()
{
}
