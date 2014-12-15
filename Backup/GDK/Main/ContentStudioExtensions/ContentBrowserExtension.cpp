#include "stdafx.h"

ContentBrowserExtension::ContentBrowserExtension() :
    _refcount(1)
{

}

ContentBrowserExtension::~ContentBrowserExtension()
{

}

// IUnknown
HRESULT STDMETHODCALLTYPE ContentBrowserExtension::QueryInterface(_In_ REFIID iid, _Deref_out_ void** ppvObject)
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

ULONG STDMETHODCALLTYPE ContentBrowserExtension::AddRef()
{
    return (ULONG)InterlockedIncrement(&_refcount);
}

ULONG STDMETHODCALLTYPE ContentBrowserExtension::Release()
{
    ULONG res = (ULONG) InterlockedDecrement(&_refcount);
    if (res == 0) 
    {
        delete this;
    }

    return res;
}

HRESULT ContentBrowserExtension::GetWindow(_Deref_out_ IContentStudioWindow** ppWindow)
{
    *ppWindow = new ContentBrowserWindow();
    
    if (!*ppWindow)
        return E_OUTOFMEMORY;

    return S_OK;
}

HRESULT ContentBrowserExtension::GetToolbar(_Deref_out_ IContentStudioToolbar** ppToolbar)
{
    return E_NOTIMPL;
}

HRESULT ContentBrowserExtension::GetMenu(_Deref_out_ IContentStudioMenu** ppMenu)
{
    return E_NOTIMPL;
}

HRESULT ContentBrowserExtension::SetDocumentProperties(IContentStudioProperties* pProperties)
{
    return S_OK;
}