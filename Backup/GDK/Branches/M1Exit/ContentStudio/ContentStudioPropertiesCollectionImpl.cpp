#include "stdafx.h"

ContentStudioPropertiesCollection::ContentStudioPropertiesCollection() :
    _refcount(1)
{

}

ContentStudioPropertiesCollection::~ContentStudioPropertiesCollection()
{

}

void ContentStudioPropertiesCollection::Clear()
{
    this->_propCollection.clear();
}

HRESULT STDMETHODCALLTYPE ContentStudioPropertiesCollection::QueryInterface(_In_ REFIID iid, _Deref_out_ void** ppvObject)
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
    else if (iid == __uuidof(IContentStudioPropertiesCollection))
    {
        *ppvObject = static_cast<IContentStudioPropertiesCollection*>(this);
        AddRef();
        return S_OK;
    }

    return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE ContentStudioPropertiesCollection::AddRef()
{
    return (ULONG)InterlockedIncrement(&_refcount);
}

ULONG STDMETHODCALLTYPE ContentStudioPropertiesCollection::Release()
{
    ULONG res = (ULONG) InterlockedDecrement(&_refcount);
    if (res == 0) 
    {
        delete this;
    }

    return res;
}

HRESULT ContentStudioPropertiesCollection::Add(IContentStudioProperties* pProperties)
{
    if (pProperties == nullptr)
    {
        return E_INVALIDARG;
    }

    HRESULT hr = S_OK;
    stde::com_ptr<IContentStudioProperties> spProps;

    hr = pProperties->QueryInterface(IID_IUnknown, (void**)&spProps);
    if (SUCCEEDED(hr))
    {
        _propCollection.push_back(spProps);
    }

    return hr;
}

HRESULT ContentStudioPropertiesCollection::Get(size_t index, IContentStudioProperties** ppProperties)
{
    if (index >= _propCollection.size())
    {
        return E_INVALIDARG;
    }

    if (ppProperties == nullptr)
    {
        return E_INVALIDARG;
    }

    *ppProperties = nullptr;

    if (_propCollection[index] != nullptr)
    {
        return _propCollection[index]->QueryInterface(IID_IUnknown, (void**)ppProperties);
    }

    return E_NOINTERFACE;
}

HRESULT ContentStudioPropertiesCollection::GetTotal(size_t& numProps)
{
    numProps = _propCollection.size();
    return S_OK;
}

HRESULT ContentStudioPropertiesCollection::RegisterPropertiesCollectionCallback(IContentStudioPropertiesCollectionCallback* pCallback, int64& eventCookie)
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

HRESULT ContentStudioPropertiesCollection::UnregisterPropertiesCollectionCallback(int64 eventCookie)
{
    HRESULT hr = S_OK;

    _callbackMap[eventCookie] = nullptr;

    return hr;
}

HRESULT ContentStudioPropertiesCollection::SignalCollectionChangedEvent()
{
    for ( std::map<int64, stde::com_ptr<IContentStudioPropertiesCollectionCallback> >::const_iterator iter = _callbackMap.begin(); iter != _callbackMap.end(); ++iter )
    {
        if (iter->second != nullptr)
        {
            stde::com_ptr<IContentStudioPropertiesCollectionCallback> spCallback = iter->second;
            if (spCallback != nullptr)
            {
                spCallback->OnCollecionChanged(this);
            }

            // iter->first  (key)
            // iter->second (value)
        }
    }

    return S_OK;
}
