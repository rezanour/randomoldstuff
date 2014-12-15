#include "stdafx.h"

ObjectPropertiesWindow::ObjectPropertiesWindow() :
    _refcount(1),
    _parentWnd(nullptr)
{

}

ObjectPropertiesWindow::~ObjectPropertiesWindow()
{

}

// IUnknown
HRESULT STDMETHODCALLTYPE ObjectPropertiesWindow::QueryInterface(_In_ REFIID iid, _Deref_out_ void** ppvObject)
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

ULONG STDMETHODCALLTYPE ObjectPropertiesWindow::AddRef()
{
    return (ULONG)InterlockedIncrement(&_refcount);
}

ULONG STDMETHODCALLTYPE ObjectPropertiesWindow::Release()
{
    ULONG res = (ULONG) InterlockedDecrement(&_refcount);
    if (res == 0) 
    {
        delete this;
    }

    return res;
}

HRESULT ObjectPropertiesWindow::GetCaption(WCHAR* pCaption, size_t cchCaption)
{
    return StringCchCopyW( pCaption, cchCaption, L"Properties" );
}

HRESULT ObjectPropertiesWindow::Create(LPCREATESTRUCT lpCreateStruct)
{
    _parentWnd = lpCreateStruct->hwndParent;

    // If you are using MFC, then the CWnd* of the parent is stored in 
    // the createParams member of the struct.
    CWnd* pParentWnd = (CWnd*)lpCreateStruct->lpCreateParams;

    _staticWnd.Create(L"STATIC", L"Hello Properties Window", WS_CHILD | WS_VISIBLE, CRect(lpCreateStruct->x, lpCreateStruct->y, lpCreateStruct->cx, lpCreateStruct->cy), pParentWnd, 1234);

    return S_OK;
}

void ObjectPropertiesWindow::OnSize(UINT nType, int cx, int cy)
{
    CRect rectClient;
    GetClientRect(_parentWnd, rectClient);

    _staticWnd.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
    
    if (_parentWnd != nullptr)
    {
        //RedrawWindow(_parentWnd, nullptr, nullptr, RDW_INTERNALPAINT);
    }
}

bool ObjectPropertiesWindow::IsOwnedWindow(HWND hWnd)
{
    return false;
}

void ObjectPropertiesWindow::OnContextMenu(HWND hWnd, LPPOINT pPoint)
{

}

void ObjectPropertiesWindow::OnPaint(HWND hWnd, HDC hDC)
{

}

void ObjectPropertiesWindow::OnSetFocus(HWND hOldWnd)
{

}

void ObjectPropertiesWindow::OnSetDocumentProperties(IContentStudioProperties* pProperties)
{
    _documentProperties = pProperties;
    _staticWnd.SetDocumentProperties(pProperties);
}
