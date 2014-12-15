#include "stdafx.h"

ContentStudioProperties::ContentStudioProperties() :
    _refcount(1)
{
    CopyChangedProperties(true);
    _numPropsChanged = 0;
}

ContentStudioProperties::~ContentStudioProperties()
{

}

void ContentStudioProperties::Reset()
{
    _propMap.clear();
}

HRESULT STDMETHODCALLTYPE ContentStudioProperties::QueryInterface(_In_ REFIID iid, _Deref_out_ void** ppvObject)
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
    else if (iid == __uuidof(IContentStudioProperties))
    {
        *ppvObject = static_cast<IContentStudioProperties*>(this);
        AddRef();
        return S_OK;
    }

    return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE ContentStudioProperties::AddRef()
{
    return (ULONG)InterlockedIncrement(&_refcount);
}

ULONG STDMETHODCALLTYPE ContentStudioProperties::Release()
{
    ULONG res = (ULONG) InterlockedDecrement(&_refcount);
    if (res == 0) 
    {
        delete this;
    }

    return res;
}

HRESULT ContentStudioProperties::Set(LPCSTR id, _variant_t& value)
{
    _propMap[id] = value;
    MarkPropertyChanged(id);

    return S_OK;
}

HRESULT ContentStudioProperties::Get(LPCSTR id, _variant_t& value)
{
    value = _propMap[id];
    if (value.vt == VT_EMPTY)
    {
        return E_FAIL;
    }
    return S_OK;
}

HRESULT ContentStudioProperties::GetTotal(size_t& numProps)
{
    numProps = _propMap.size();
    return S_OK;
}

HRESULT ContentStudioProperties::GetByIndex(size_t index, _variant_t& value)
{
    size_t currIndex = 0;
    for ( std::map<std::string, _variant_t >::const_iterator iter = _propMap.begin(); iter != _propMap.end(); ++iter )
    {
        if (index == currIndex)
        {
            value = iter->second;
            return S_OK;
        }

        currIndex++;
    }

    return E_INVALIDARG;
}

HRESULT ContentStudioProperties::GetIdByIndex(size_t index, LPCSTR* pid)
{
    size_t currIndex = 0;
    for ( std::map<std::string, _variant_t >::const_iterator iter = _propMap.begin(); iter != _propMap.end(); ++iter )
    {
        if (index == currIndex)
        {
            *pid = iter->first.c_str();
            return S_OK;
        }

        currIndex++;
    }

    return E_INVALIDARG;
}

HRESULT ContentStudioProperties::Contains(LPCSTR id, bool& hasId)
{
    std::string s = id;
    for ( std::map<std::string, _variant_t >::const_iterator iter = _propMap.begin(); iter != _propMap.end(); ++iter )
    {
        if (s == iter->first.c_str())
        {
            hasId = true;
            return S_OK;
        }
    }

    return S_OK;
}

HRESULT ContentStudioProperties::SetInt32(LPCSTR id, int value)
{
    return Set(id, _variant_t(value));
}

HRESULT ContentStudioProperties::GetInt32(LPCSTR id, int& value)
{
    _variant_t v;
    HRESULT hr = S_OK;
    
    hr = Get(id, v);
    if (SUCCEEDED(hr))
    {
        value = v;
    }

    return hr;
}

HRESULT ContentStudioProperties::SetUInt32(LPCSTR id, unsigned int value)
{
    return Set(id, _variant_t(value));
}

HRESULT ContentStudioProperties::GetUInt32(LPCSTR id, unsigned int& value)
{
    _variant_t v;
    HRESULT hr = S_OK;
    
    hr = Get(id, v);
    if (SUCCEEDED(hr))
    {
        value = v;
    }

    return hr;
}

HRESULT ContentStudioProperties::SetInt64(LPCSTR id, int64& value)
{
    return Set(id, _variant_t(value));
}

HRESULT ContentStudioProperties::GetInt64(LPCSTR id, int64& value)
{
    _variant_t v;
    HRESULT hr = S_OK;
    
    hr = Get(id, v);
    if (SUCCEEDED(hr))
    {
        value = v;
    }

    return hr;
}

HRESULT ContentStudioProperties::SetUInt64(LPCSTR id, uint64& value)
{
    return Set(id, _variant_t(value));
}

HRESULT ContentStudioProperties::GetUInt64(LPCSTR id, uint64& value)
{
    _variant_t v;
    HRESULT hr = S_OK;
    
    hr = Get(id, v);
    if (SUCCEEDED(hr))
    {
        value = v;
    }

    return hr;
}

HRESULT ContentStudioProperties::SetFloat(LPCSTR id, float& value)
{
    return Set(id, _variant_t(value));
}

HRESULT ContentStudioProperties::GetFloat(LPCSTR id, float& value)
{
    _variant_t v;
    HRESULT hr = S_OK;
    
    hr = Get(id, v);
    if (SUCCEEDED(hr))
    {
        value = v;
    }

    return hr;
}

HRESULT ContentStudioProperties::SetInterface(LPCSTR id, IUnknown* pInterface)
{
    return Set(id, _variant_t(pInterface));
}

HRESULT ContentStudioProperties::GetInterface(LPCSTR id, IUnknown** ppInterface)
{
    _variant_t v;
    HRESULT hr = S_OK;
    
    hr = Get(id, v);
    if (SUCCEEDED(hr))
    {
        hr = v.punkVal->QueryInterface(IID_IUnknown, (void**)ppInterface);
    }

    return hr;
}

HRESULT ContentStudioProperties::SetDouble(LPCSTR id, double& value)
{
    return Set(id, _variant_t(value));
}

HRESULT ContentStudioProperties::GetDouble(LPCSTR id, double& value)
{
    _variant_t v;
    HRESULT hr = S_OK;
    
    hr = Get(id, v);
    if (SUCCEEDED(hr))
    {
        value = v;
    }

    return hr;
}

HRESULT ContentStudioProperties::SetString(LPCSTR id, LPSTR pvalue)
{
    return Set(id, _variant_t(pvalue));
}

HRESULT ContentStudioProperties::GetString(LPCSTR id, LPSTR pvalue, size_t cch)
{
    HRESULT hr = S_OK;
    _variant_t v;
    _bstr_t bstr;

    hr = Get(id, v);
    if (SUCCEEDED(hr))
    {
        bstr = v;
        std::string s = stde::to_string(bstr);
        hr = StringCchCopyA(pvalue, cch, s.c_str());
    }

    return hr;
}

HRESULT ContentStudioProperties::SetStringW(LPCSTR id, LPWSTR pvalue)
{
    return Set(id, _variant_t(pvalue));
}

HRESULT ContentStudioProperties::GetStringW(LPCSTR id, LPWSTR pvalue, size_t cch)
{
    HRESULT hr = S_OK;
    _variant_t v;
    _bstr_t bstr;

    hr = Get(id, v);
    if (SUCCEEDED(hr))
    {
        bstr = v;
        std::wstring s = stde::to_wstring(bstr);
        hr = StringCchCopyW(pvalue, cch, s.c_str());
    }

    return hr;
}

HRESULT ContentStudioProperties::RegisterPropertiesCallback(IContentStudioPropertiesCallback* pCallback, int64& eventCookie)
{
    if (!pCallback)
    {
        return E_INVALIDARG;
    }

    HRESULT hr = S_OK;

    GUID guid = {0};
    hr = CoCreateGuid(&guid);
    if (SUCCEEDED(hr))
    {
        eventCookie = (int64)guid.Data1 << 32;
        eventCookie |= (int64)guid.Data2 << 16;
        eventCookie |= guid.Data3;

        _callbackMap[eventCookie] = pCallback;
    }

    return hr;
}

HRESULT ContentStudioProperties::UnregisterPropertiesCallback(int64 eventCookie)
{
    HRESULT hr = S_OK;

    _callbackMap[eventCookie] = nullptr;

    return hr;
}

HRESULT ContentStudioProperties::SignalPropertiesChangedEvent()
{
    CopyChangedProperties(false);

    for ( std::map<int64, stde::com_ptr<IContentStudioPropertiesCallback> >::const_iterator iter = _callbackMap.begin(); iter != _callbackMap.end(); ++iter )
    {
        if (iter->second != nullptr)
        {
            stde::com_ptr<IContentStudioPropertiesCallback> spCallback = iter->second;
            if (spCallback != nullptr)
            {
                spCallback->OnPropertiesChanged(this, _propertiesChangedArray, _numPropsChanged);
            }

            // iter->first  (key)
            // iter->second (value)
        }
    }

    // clear changed properties after notifcation has been sent
    _propertiesThatChanged.clear();

    return S_OK;
}

void ContentStudioProperties::CopyChangedProperties(bool clearEntries)
{
    _numPropsChanged = 0;

    for (size_t i = 0; i < ARRAYSIZE(_propertiesChangedArray); i++)
    {
        _propertiesChangedArray[i] = nullptr;
    }

    if (!clearEntries)
    {
        for (size_t i = 0; i < _propertiesThatChanged.size(); i++)
        {
            if (_numPropsChanged < ARRAYSIZE(_propertiesChangedArray))
            {
                _numPropsChanged++;
                _propertiesChangedArray[i] = _propertiesThatChanged[i].c_str();
            }
            else
            {
                break;
            }
        }
    }
}

HRESULT ContentStudioProperties::MarkPropertyChanged(LPCSTR id)
{
    bool propertyChanged = false;
    for (size_t i = 0; i < _propertiesThatChanged.size(); i++)
    {
        if (id == _propertiesThatChanged[i])
        {
            propertyChanged = true;
            break;
        }
    }

    // If the property has not been changed, add it to the changed list
    if (!propertyChanged)
    {
        _propertiesThatChanged.push_back(id);
    }

    return S_OK;
}

HRESULT ContentStudioProperties::Create(IContentStudioProperties** ppProperties)
{
    if (ppProperties == nullptr)
    {
        return E_INVALIDARG;
    }

    HRESULT hr = S_OK;

    ContentStudioProperties* pProperties = new ContentStudioProperties();
    if (pProperties != nullptr)
    {
        *ppProperties = pProperties;
    }
    else
    {
        hr = E_OUTOFMEMORY;
    }

    return hr;
}

std::string ReadStringProperty(LPCSTR id, IContentStudioProperties* pProperties)
{
    std::string value;
    char szValue[MAX_PATH] = {0};
    if (pProperties != nullptr)
    {
        if (SUCCEEDED(pProperties->GetString(id, szValue, ARRAYSIZE(szValue) - 1)))
        {
            value = szValue;
        }
    }

    return value;
}

std::wstring ReadStringPropertyW(LPCSTR id, IContentStudioProperties* pProperties)
{
    std::wstring value;
    WCHAR szValue[MAX_PATH] = {0};
    if (pProperties != nullptr)
    {
        if (SUCCEEDED(pProperties->GetStringW(id, szValue, ARRAYSIZE(szValue) - 1)))
        {
            value = szValue;
        }
    }

    return value;
}
