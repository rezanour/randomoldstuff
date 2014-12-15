#include "stdafx.h"

ProjectBrowserWindow::ProjectBrowserWindow(ProjectBrowseType browseType) :
    _refcount(1),
    _parentWnd(nullptr),
    _browseType(browseType)
{
    
}

ProjectBrowserWindow::~ProjectBrowserWindow()
{

}

// IUnknown
HRESULT STDMETHODCALLTYPE ProjectBrowserWindow::QueryInterface(_In_ REFIID iid, _Deref_out_ void** ppvObject)
{
    if (ppvObject == nullptr)
    {
        return E_INVALIDARG;
    }

    *ppvObject = nullptr;

    if (iid == __uuidof(IUnknown))
    {
        *ppvObject = static_cast<IUnknown*>(this);
        AddRef();
        return S_OK;
    }

    return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE ProjectBrowserWindow::AddRef()
{
    return (ULONG)InterlockedIncrement(&_refcount);
}

ULONG STDMETHODCALLTYPE ProjectBrowserWindow::Release()
{
    ULONG res = (ULONG) InterlockedDecrement(&_refcount);
    if (res == 0) 
    {
        delete this;
    }

    return res;
}

HRESULT ProjectBrowserWindow::GetCaption(WCHAR* pCaption, size_t cchCaption)
{
    LPCWSTR caption = L"Browser";

    switch (_browseType)
    {
    case ProjectBrowseType::Screens:
        caption = L"Screens";
        break;
    case ProjectBrowseType::GameObjects:
        caption = L"Objects";
        break;
    case ProjectBrowseType::Assets:
        caption = L"Assets";
        break;
    case ProjectBrowseType::Resources:
        caption = L"Resources";
        break;
    case ProjectBrowseType::Components:
        caption = L"Components";
        break;
    default:
        break;
    }

    return StringCchCopyW( pCaption, cchCaption, caption );
}

HRESULT ProjectBrowserWindow::Create(LPCREATESTRUCT lpCreateStruct)
{
    _parentWnd = lpCreateStruct->hwndParent;

    // If you are using MFC, then the CWnd* of the parent is stored in 
    // the createParams member of the struct.
    CWnd* pParentWnd = (CWnd*)lpCreateStruct->lpCreateParams;

    _staticWnd.Initialize(_browseType);
    _staticWnd.Create(L"STATIC", L"Hello Browser Window", WS_CHILD | WS_VISIBLE, CRect(lpCreateStruct->x, lpCreateStruct->y, lpCreateStruct->cx, lpCreateStruct->cy), pParentWnd, 1234);

    return S_OK;
}

void ProjectBrowserWindow::OnSize(UINT nType, int cx, int cy)
{
    CRect rectClient;
    GetClientRect(_parentWnd, rectClient);

    _staticWnd.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
    
    if (_parentWnd != nullptr)
    {
        //RedrawWindow(_parentWnd, nullptr, nullptr, RDW_INTERNALPAINT);
    }
}

bool ProjectBrowserWindow::IsOwnedWindow(HWND hWnd)
{
    return false;
}

void ProjectBrowserWindow::OnContextMenu(HWND hWnd, LPPOINT pPoint)
{

}

void ProjectBrowserWindow::OnPaint(HWND hWnd, HDC hDC)
{
    //CDC dc;
    //dc.Attach(hDC);

    //CMemoryDC memDC(&dc);
    //RECT rect = {0};
    //GetClientRect(hWnd, &rect);
    //memDC.Rectangle(&rect);

    //dc.Detach();
}

void ProjectBrowserWindow::OnSetFocus(HWND hOldWnd)
{

}

void ProjectBrowserWindow::OnSetDocumentProperties(IContentStudioProperties* pProperties)
{
    _documentProperties = pProperties;
    _staticWnd.SetDocumentProperties(pProperties);
}
