#include "stdafx.h"

ObjectPropertiesExtension::ObjectPropertiesExtension() :
    _refcount(1)
{

}

ObjectPropertiesExtension::~ObjectPropertiesExtension()
{

}

// IUnknown
HRESULT STDMETHODCALLTYPE ObjectPropertiesExtension::QueryInterface(_In_ REFIID iid, _Deref_out_ void** ppvObject)
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

ULONG STDMETHODCALLTYPE ObjectPropertiesExtension::AddRef()
{
    return (ULONG)InterlockedIncrement(&_refcount);
}

ULONG STDMETHODCALLTYPE ObjectPropertiesExtension::Release()
{
    ULONG res = (ULONG) InterlockedDecrement(&_refcount);
    if (res == 0) 
    {
        delete this;
    }

    return res;
}

HRESULT ObjectPropertiesExtension::GetWindow(_Deref_out_ IContentStudioWindow** ppWindow)
{
    if (!ppWindow)
        return E_INVALIDARG;

    if (_window == nullptr)
    {
        *ppWindow = new ObjectPropertiesWindow();
        if (!*ppWindow)
            return E_OUTOFMEMORY;

        _window = *ppWindow;
    }
    else
    {
        _window->QueryInterface(IID_IUnknown, (void**)ppWindow);
    }

    return S_OK;
}

HRESULT ObjectPropertiesExtension::GetToolbar(_Deref_out_ IContentStudioToolbar** ppToolbar)
{
    return E_NOTIMPL;
}

HRESULT ObjectPropertiesExtension::GetMenu(_Deref_out_ IContentStudioMenu** ppMenu)
{
    return E_NOTIMPL;
}

HRESULT ObjectPropertiesExtension::SetDocumentProperties(IContentStudioProperties* pProperties)
{
    if (_window != nullptr)
    {
        _window->OnSetDocumentProperties(pProperties);
    }
    return S_OK;
}
