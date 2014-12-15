#pragma once

class ContentStudioProperties : IContentStudioProperties
{
public:
    ContentStudioProperties();
    virtual ~ContentStudioProperties();

public:
    // IUnknown
    HRESULT STDMETHODCALLTYPE QueryInterface(_In_ REFIID iid, _Deref_out_ void** ppvObject);
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // General get/set
    HRESULT Set(LPCSTR id, _variant_t& value);
    HRESULT Get(LPCSTR id, _variant_t& value);
    HRESULT GetTotal(size_t& numProps);
    HRESULT GetByIndex(size_t index, _variant_t& value);
    HRESULT GetIdByIndex(size_t index, LPCSTR* pid);
    HRESULT Contains(LPCSTR id, bool& hasId);
    HRESULT MarkPropertyChanged(LPCSTR id);

    // Type specific
    HRESULT SetInt32(LPCSTR id, int value);
    HRESULT GetInt32(LPCSTR id, int& value);
    HRESULT SetUInt32(LPCSTR id, unsigned int value);
    HRESULT GetUInt32(LPCSTR id, unsigned int& value);
    HRESULT SetInt64(LPCSTR id, int64& value);
    HRESULT GetInt64(LPCSTR id, int64& value);
    HRESULT SetUInt64(LPCSTR id, uint64& value);
    HRESULT GetUInt64(LPCSTR id, uint64& value);
    HRESULT SetFloat(LPCSTR id, float& value);
    HRESULT GetFloat(LPCSTR id, float& value);
    HRESULT SetDouble(LPCSTR id, double& value);
    HRESULT GetDouble(LPCSTR id, double& value);
    HRESULT SetString(LPCSTR id, LPSTR pvalue);
    HRESULT GetString(LPCSTR id, LPSTR pvalue, size_t cch);
    HRESULT SetStringW(LPCSTR id, LPWSTR pvalue);
    HRESULT GetStringW(LPCSTR id, LPWSTR pvalue, size_t cch);
    HRESULT SetInterface(LPCSTR id, IUnknown* pInterface);
    HRESULT GetInterface(LPCSTR id, IUnknown** ppInterface);

    // Events/Callbacks
    HRESULT RegisterPropertiesCallback(IContentStudioPropertiesCallback* pCallback, int64& eventCookie);
    HRESULT UnregisterPropertiesCallback(int64 eventCookie);
    HRESULT SignalPropertiesChangedEvent();

    void Clear();

public:
    static HRESULT Create(IContentStudioProperties** ppProperties);

private:
    void CopyChangedProperties(bool clearEntries);

private:
    std::map<std::string, _variant_t> _propMap;
    std::map<int64, stde::com_ptr<IContentStudioPropertiesCallback> > _callbackMap;
    std::vector<std::string> _propertiesThatChanged;
    PCSTR _propertiesChangedArray[255];
    size_t _numPropsChanged;
    long _refcount;
};

std::string ReadStringProperty(LPCSTR id, IContentStudioProperties* pProperties);
std::wstring ReadStringPropertyW(LPCSTR id, IContentStudioProperties* pProperties);