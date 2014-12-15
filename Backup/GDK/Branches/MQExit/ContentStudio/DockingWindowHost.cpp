#include "stdafx.h"

CDockingWindowHost::CDockingWindowHost(IContentStudioWindow* pWindow)
{
    _spWindow = pWindow;
}

CDockingWindowHost::~CDockingWindowHost()
{

}

BEGIN_MESSAGE_MAP(CDockingWindowHost, CDockablePane)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_CONTEXTMENU()
    ON_WM_PAINT()
    ON_WM_SETFOCUS()
END_MESSAGE_MAP()

int CDockingWindowHost::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    // fail creation if no content studio window interface is found
    if (_spWindow == nullptr)
        return -1;

    if (CDockablePane::OnCreate(lpCreateStruct) == -1)
        return -1;

    CREATESTRUCT createStruct = *lpCreateStruct;
    createStruct.hwndParent = m_hWnd; // adjust the parent to be this host window
    createStruct.lpCreateParams = this;

    if (FAILED(_spWindow->Create(&createStruct)))
        return -1;

    return 0;
}

void CDockingWindowHost::OnSize(UINT nType, int cx, int cy)
{
    CDockablePane::OnSize(nType, cx, cy);

    if (GetSafeHwnd() == nullptr)
    {
        return;
    }

    _spWindow->OnSize(nType, cx, cy);
}

void CDockingWindowHost::OnContextMenu(CWnd* pWnd, CPoint point)
{
    if (!_spWindow->IsOwnedWindow(pWnd->m_hWnd))
    {
        CDockablePane::OnContextMenu(pWnd, point);
        return;
    }

    if (point != CPoint(-1, -1))
    {
        _spWindow->OnContextMenu(pWnd->m_hWnd, &point);
    }
}

void CDockingWindowHost::OnPaint()
{
    CPaintDC dc(this);

    _spWindow->OnPaint(m_hWnd, dc.m_hDC);

    CDockablePane::OnPaint();
}

BOOL CDockingWindowHost::PreTranslateMessage(MSG* pMsg)
{
    return CDockablePane::PreTranslateMessage(pMsg);
}

void CDockingWindowHost::OnSetFocus(CWnd* pOldWnd)
{
    CDockablePane::OnSetFocus(pOldWnd);
    _spWindow->OnSetFocus(pOldWnd->m_hWnd);
}

// Docking Window Manager object
DockingWindowManager::DockingWindowManager()
{

}

DockingWindowManager::~DockingWindowManager()
{
    Destroy();
}

void DockingWindowManager::Create(CFrameWndEx* pFrame, UIExtensionManager* pExtensionsManager)
{
    if (pExtensionsManager != nullptr)
    {
        size_t numExtensions = pExtensionsManager->GetExtensionsCount();

        // create docking plugin windows
        for (size_t i = 0; i < numExtensions; i++)
        {
            stde::com_ptr<IContentStudioWindow> spWindow;
            HRESULT hr = pExtensionsManager->GetWindow(i, &spWindow);
            if (SUCCEEDED(hr))
            {
                // We have an instance of an IContentStudioWindow from a UI extension.
                // Create a docking window host and pass that instance the IContentStudioWindow
                // to delegate to.
                CDockingWindowHost* pHost = new CDockingWindowHost(spWindow);
                if (pHost)
                {
                    UINT controlId = (UINT)(i + 1000);
                    std::wstring hostName;
                    WCHAR caption[128] = {0};
                    if (SUCCEEDED(spWindow->GetCaption(caption, (ARRAYSIZE(caption) - 1))))
                    {
                        hostName = caption;
                    }
                    else
                    {
                        CString strHostName;
                        strHostName.Format(L"Window 0x%lx",controlId);
                        hostName = strHostName;
                    }

                    if (pHost->Create(hostName.c_str(), pFrame, CRect(0, 0, 200, 200), TRUE,  controlId, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
                    {
                        _dockableWindows.push_back(pHost);
                        pHost->EnableDocking(CBRS_ALIGN_ANY);
                        pFrame->DockPane(pHost);
                        if (hostName != L"Objects")
                        {
                            pHost->ShowPane(FALSE, FALSE, FALSE);
                        }
                    }
                }
            }
        }
    }
}

void DockingWindowManager::Destroy()
{
    for (size_t i = 0; i < _dockableWindows.size(); i++)
    {
        delete _dockableWindows[i];
    }

    _dockableWindows.clear();
}

void DockingWindowManager::SetDockingWindowIcons(bool bHiColorIcons)
{
    // set docking window icons
    for (size_t i = 0; i < _dockableWindows.size(); i++)
    {
        //HICON hHostIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_FILE_VIEW_HC : IDI_FILE_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
        //_dockableWindows[i]->SetIcon(hHostIcon, FALSE);
    }
}