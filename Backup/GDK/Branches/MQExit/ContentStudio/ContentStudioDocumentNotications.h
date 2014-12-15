#pragma once

class CContentStudioDocumentNotications : public IContentStudioPropertiesCallback
{
public:
    CContentStudioDocumentNotications(CContentStudioView* pView)
    {
        _refcount = 1;
        _pView = pView;
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(_In_ REFIID iid, _Deref_out_ void** ppvObject)
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

    ULONG STDMETHODCALLTYPE AddRef()
    {
        return (ULONG)InterlockedIncrement(&_refcount);
    }

    ULONG STDMETHODCALLTYPE Release()
    {
        ULONG res = (ULONG) InterlockedDecrement(&_refcount);
        if (res == 0) 
        {
            delete this;
        }

        return res;
    }

    // IContentStudioPropertiesCallback
    HRESULT OnPropertiesChanged(IContentStudioProperties* pProperties, LPCSTR* changedProperties, size_t numProps)
    {
        _pView->RefreshCurrentView();
        return S_OK;
    }

private:
    CContentStudioView* _pView;
    long _refcount;
};