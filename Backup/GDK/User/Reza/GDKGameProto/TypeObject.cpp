#include "StdAfx.h"
#include "TypeObject.h"
#include "Executive.h"
#include "MemoryManager.h"

using GDK::TypeObject;

uint32_t TypeObject::s_nextID = 1;

TypeObject::TypeObject()
    : _name(nullptr), _id(s_nextID++)
{
}

TypeObject::~TypeObject()
{
    if (_name)
    {
        GDK::Executive::GetMemoryManager()->Free(_name);
        _name = nullptr;
    }
}

HRESULT TypeObject::Create(const char* name, TypeObject** ppType)
{
    if (!name)
        return E_INVALIDARG;

    if (!ppType)
        return E_POINTER;

    HRESULT hr = S_OK;

    *ppType = nullptr;
    TypeObject* pType = GDKNEW("TypeObject") TypeObject();
    hr = pType ? S_OK : E_OUTOFMEMORY;

    size_t len = strlen(name) + 1;

    if (SUCCEEDED(hr))
    {
#ifndef NDEBUG
        char tag[200] = {0};
        sprintf_s(tag, "TypeObject Name (%s)", name);
        pType->_name = static_cast<char*>(GDK::Executive::GetMemoryManager()->Alloc(len, tag));
#else
        pType->_name = static_cast<char*>(GDK::Executive::GetMemoryManager()->Alloc(len, name));
#endif
        hr = pType->_name ? S_OK : E_OUTOFMEMORY;
    }

    if (SUCCEEDED(hr))
    {
        hr = StringCchCopyA(pType->_name, len, name);
    }

    if (SUCCEEDED(hr))
    {
        *ppType = pType;
    }
    else if (pType)
    {
        pType->Release();
    }

    return hr;
}

const char* TypeObject::GetName() const
{
    return _name;
}

uint32_t TypeObject::GetID() const
{
    return _id;
}
