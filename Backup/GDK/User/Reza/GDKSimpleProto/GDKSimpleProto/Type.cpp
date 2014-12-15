#include "StdAfx.h"
#include "Type.h"

using GDK::Type;

Type* Type::s_rootType = nullptr;

HRESULT Type::Create(const char* name, Type** type)
{
    if (!name)
        return E_INVALIDARG;

    if (!type)
        return E_POINTER;

    HRESULT hr = S_OK;

    *type = new Type(name);
    hr = (*type) ? S_OK : E_OUTOFMEMORY;

    if (SUCCEEDED(hr))
    {
        (*type)->SetType(GetRootType());
    }

    return hr;
}

GDK::IType* Type::GetRootType()
{
    if (!s_rootType)
    {
        s_rootType = new Type("Type");
        if (s_rootType)
        {
            s_rootType->SetType(s_rootType);
        }
    }

    return s_rootType;
}

Type::Type(const char* name)
{
    SetName(name);
}

Type::~Type()
{
}
