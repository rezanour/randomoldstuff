#include "stdafx.h"

ContentBrowserWindow::ContentBrowserWindow() :
    _refcount(1),
    _parentWnd(nullptr)
{

}

ContentBrowserWindow::~ContentBrowserWindow()
{

}

// IUnknown
HRESULT STDMETHODCALLTYPE ContentBrowserWindow::QueryInterface(_In_ REFIID iid, _Deref_out_ void** ppvObject)
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

ULONG STDMETHODCALLTYPE ContentBrowserWindow::AddRef()
{
    return (ULONG)InterlockedIncrement(&_refcount);
}

ULONG STDMETHODCALLTYPE ContentBrowserWindow::Release()
{
    ULONG res = (ULONG) InterlockedDecrement(&_refcount);
    if (res == 0) 
    {
        delete this;
    }

    return res;
}

HRESULT ContentBrowserWindow::GetCaption(WCHAR* pCaption, size_t cchCaption)
{
    return StringCchCopyW( pCaption, cchCaption, L"Content Browser" );
}

HRESULT ContentBrowserWindow::Create(LPCREATESTRUCT lpCreateStruct)
{
    _parentWnd = lpCreateStruct->hwndParent;

    // If you are using MFC, then the CWnd* of the parent is stored in 
    // the createParams member of the struct.
    CWnd* pParentWnd = (CWnd*)lpCreateStruct->lpCreateParams;

    _staticWnd.Create(L"STATIC", L"Hello Browser Window", WS_CHILD | WS_VISIBLE, CRect(lpCreateStruct->x, lpCreateStruct->y, lpCreateStruct->cx, lpCreateStruct->cy), pParentWnd, 1234);

    return S_OK;
}

void ContentBrowserWindow::OnSize(UINT nType, int cx, int cy)
{
    CRect rectClient;
    GetClientRect(_parentWnd, rectClient);

    _staticWnd.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
    
    if (_parentWnd != nullptr)
    {
        //RedrawWindow(_parentWnd, nullptr, nullptr, RDW_INTERNALPAINT);
    }
}

bool ContentBrowserWindow::IsOwnedWindow(HWND hWnd)
{
    return false;
}

void ContentBrowserWindow::OnContextMenu(HWND hWnd, LPPOINT pPoint)
{

}

void ContentBrowserWindow::OnPaint(HWND hWnd, HDC hDC)
{
    //CDC dc;
    //dc.Attach(hDC);

    //CMemoryDC memDC(&dc);
    //RECT rect = {0};
    //GetClientRect(hWnd, &rect);
    //memDC.Rectangle(&rect);

    //dc.Detach();
}

void ContentBrowserWindow::OnSetFocus(HWND hOldWnd)
{

}

void ContentBrowserWindow::OnSetDocumentProperties(IContentStudioProperties* pProperties)
{

}
