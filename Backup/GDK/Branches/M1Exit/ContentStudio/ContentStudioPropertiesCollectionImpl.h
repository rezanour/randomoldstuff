#pragma once

class ContentStudioPropertiesCollection : IContentStudioPropertiesCollection
{
public:
    ContentStudioPropertiesCollection();
    virtual ~ContentStudioPropertiesCollection();

public:
    // IUnknown
    HRESULT STDMETHODCALLTYPE QueryInterface(_In_ REFIID iid, _Deref_out_ void** ppvObject);
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // General get/set
    HRESULT Add(IContentStudioProperties* pProperties);
    HRESULT Get(size_t index, IContentStudioProperties** ppProperties);
    HRESULT GetTotal(size_t& numProps);

    // Events/Callbacks
    HRESULT RegisterPropertiesCollectionCallback(IContentStudioPropertiesCollectionCallback* pCallback, int64& eventCookie);
    HRESULT UnregisterPropertiesCollectionCallback(int64 eventCookie);
    HRESULT SignalCollectionChangedEvent();

    void Clear();

private:
    std::vector<stde::com_ptr<IContentStudioProperties> > _propCollection;
    std::map<int64, stde::com_ptr<IContentStudioPropertiesCollectionCallback> > _callbackMap;
    long _refcount;
};