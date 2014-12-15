#pragma once

#include <windows.h>

struct IContentStudioPropertiesCallback;

[uuid("959611df-a6ae-429a-bbd6-732099da5f5e")]
struct IContentStudioProperties : public IUnknown
{
    virtual HRESULT Set(LPCSTR id, _variant_t& value) = 0;
    virtual HRESULT Get(LPCSTR id, _variant_t& value) = 0;
    virtual HRESULT GetTotal(size_t& numProps) = 0;
    virtual HRESULT GetByIndex(size_t index, _variant_t& value) = 0;
    virtual HRESULT GetIdByIndex(size_t index, LPCSTR* pid) = 0;
    virtual HRESULT SetInt32(LPCSTR id, int value) = 0;
    virtual HRESULT GetInt32(LPCSTR id, int& value) = 0;
    virtual HRESULT SetUInt32(LPCSTR id, unsigned int value) = 0;
    virtual HRESULT GetUInt32(LPCSTR id, unsigned int& value) = 0;
    virtual HRESULT SetInt64(LPCSTR id, __int64& value) = 0;
    virtual HRESULT GetInt64(LPCSTR id, __int64& value) = 0;
    virtual HRESULT SetUInt64(LPCSTR id, unsigned __int64& value) = 0;
    virtual HRESULT GetUInt64(LPCSTR id, unsigned __int64& value) = 0;
    virtual HRESULT SetFloat(LPCSTR id, float& value) = 0;
    virtual HRESULT GetFloat(LPCSTR id, float& value) = 0;
    virtual HRESULT SetDouble(LPCSTR id, double& value) = 0;
    virtual HRESULT GetDouble(LPCSTR id, double& value) = 0;
    virtual HRESULT SetString(LPCSTR id, LPSTR pvalue) = 0;
    virtual HRESULT GetString(LPCSTR id, LPSTR pvalue, size_t cch) = 0;
    virtual HRESULT SetStringW(LPCSTR id, LPWSTR pvalue) = 0;
    virtual HRESULT GetStringW(LPCSTR id, LPWSTR pvalue, size_t cch) = 0;
    virtual HRESULT SetInterface(LPCSTR id, IUnknown* pInterface) = 0;
    virtual HRESULT GetInterface(LPCSTR id, IUnknown** ppInterface) = 0;

    virtual HRESULT MarkPropertyChanged(LPCSTR id) = 0;

    virtual HRESULT RegisterPropertiesCallback(IContentStudioPropertiesCallback* pCallback, __int64& eventCookie) = 0;
    virtual HRESULT UnregisterPropertiesCallback(__int64 eventCookie) = 0;

    virtual void Reset() = 0;

    virtual HRESULT SignalPropertiesChangedEvent() = 0;
};

[uuid("94402dc5-2504-4fab-b8a8-fc109f190a9a")]
struct IContentStudioPropertiesCallback : public IUnknown
{
    virtual HRESULT OnPropertiesChanged(IContentStudioProperties* pProperties, LPCSTR* changedProperties, size_t numProps) = 0;
};

struct IContentStudioPropertiesCollectionCallback;

[uuid("c635f900-d004-4d14-b5ad-fe65b086298b")]
struct IContentStudioPropertiesCollection : public IUnknown
{
    virtual HRESULT Add(IContentStudioProperties* pProperties) = 0;
    virtual HRESULT Get(size_t index, IContentStudioProperties** ppProperties) = 0;
    virtual HRESULT GetTotal(size_t& numProps) = 0;

    virtual void Reset() = 0;

    virtual HRESULT RegisterPropertiesCollectionCallback(IContentStudioPropertiesCollectionCallback* pCallback, __int64& eventCookie) = 0;
    virtual HRESULT UnregisterPropertiesCollectionCallback(__int64 eventCookie) = 0;

    virtual HRESULT SignalCollectionChangedEvent() = 0;
};

[uuid("7b4a2b6d-616f-49be-837e-5e18d25028d4")]
struct IContentStudioPropertiesCollectionCallback : public IUnknown
{
    virtual HRESULT OnCollecionChanged(IContentStudioPropertiesCollection* pCollection) = 0;
};
