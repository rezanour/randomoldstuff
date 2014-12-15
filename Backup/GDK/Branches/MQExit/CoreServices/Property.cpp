#include "StdAfx.h"
#include "Property.h"
#include "Debug.h"

using namespace CoreServices;
using namespace GDK;

Property::Property(_In_ const std::string& name, _In_ PropertyType type)
    : _name(name), _type(type), _floatValue(0.0f)
{
}

Property::~Property()
{
}

// IProperty
GDK_METHODIMP_(const char* const) Property::GetName() const
{
    return _name.c_str();
}

GDK_METHODIMP_(GDK::PropertyType) Property::GetType() const
{
    return _type;
}

GDK_METHODIMP Property::AsFloat(_Out_ float* pValue) const
{
    HRESULT hr = S_OK;

    ISNOTNULL(pValue, E_POINTER);
    ISTRUE(_type == PropertyType::Float, HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED));

    *pValue = _floatValue;

EXIT
    return hr;
}

GDK_METHODIMP Property::AsInt(_Out_ int* pValue) const
{
    HRESULT hr = S_OK;

    ISNOTNULL(pValue, E_POINTER);
    ISTRUE(_type == PropertyType::Integer, HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED));

    *pValue = _intValue;

EXIT
    return hr;
}

GDK_METHODIMP Property::AsUint64(_Out_ uint64* pValue) const
{
    HRESULT hr = S_OK;

    ISNOTNULL(pValue, E_POINTER);
    ISTRUE(_type == PropertyType::Uint64, HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED));

    *pValue = _uint64Value;

EXIT
    return hr;
}

GDK_METHODIMP Property::AsBool(_Out_ bool* pValue) const
{
    HRESULT hr = S_OK;

    ISNOTNULL(pValue, E_POINTER);
    ISTRUE(_type == PropertyType::Boolean, HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED));

    *pValue = _boolValue;

EXIT
    return hr;
}

GDK_METHODIMP Property::AsString(_Out_cap_(cchMax) char* pszValue, _In_ size_t cchMax) const
{
    HRESULT hr = S_OK;

    ISNOTNULL(pszValue, E_INVALIDARG);
    ISTRUE(_type == PropertyType::String, HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED));

    CHECKHR(StringCchCopyA(pszValue, cchMax, _stringValue.c_str()));

EXIT
    return hr;
}

GDK_METHODIMP Property::SetFloat(_In_ float value)
{
    HRESULT hr = S_OK;

    ISTRUE(_type == PropertyType::Float, HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED));
    _floatValue = value;

EXIT
    return hr;
}

GDK_METHODIMP Property::SetInt(_In_ int value)
{
    HRESULT hr = S_OK;

    ISTRUE(_type == PropertyType::Integer, HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED));
    _intValue = value;

EXIT
    return hr;
}

GDK_METHODIMP Property::SetUint64(_In_ uint64 value)
{
    HRESULT hr = S_OK;

    ISTRUE(_type == PropertyType::Uint64, HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED));
    _uint64Value = value;

EXIT
    return hr;
}

GDK_METHODIMP Property::SetBool(_In_ bool value)
{
    HRESULT hr = S_OK;

    ISTRUE(_type == PropertyType::Boolean, HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED));
    _boolValue = value;

EXIT
    return hr;
}

GDK_METHODIMP Property::SetString(_In_ const char* value)
{
    HRESULT hr = S_OK;

    ISTRUE(_type == PropertyType::String, HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED));
    _stringValue = value;

EXIT
    return hr;
}

