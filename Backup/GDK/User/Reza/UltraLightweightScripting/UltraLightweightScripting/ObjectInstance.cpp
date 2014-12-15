#include "StdAfx.h"
#include "ObjectInstance.h"
#include "Var.h"
#include "Method.h"
#include "TypeInfo.h"

using GDK::ObjectInstance;
using GDK::Var;
using GDK::TypeInfo;
using GDK::Method;

uint64_t ObjectInstance::s_nextID = 0;

ObjectInstance::ObjectInstance()
    : _id(s_nextID++), _typeID(0), _numProperties(0), _numMethods(0), _properties(nullptr), _methods(nullptr)
{
}

ObjectInstance::~ObjectInstance()
{
    SafeDeleteArray(_properties);
    SafeDeleteArray(_methods);
}

uint64_t ObjectInstance::GetID() const
{
    return _id;
}

uint32_t ObjectInstance::GetTypeID() const
{
    return _typeID;
}

const Var* ObjectInstance::GetProperty(_In_ uint16_t id) const
{
    return (id < _numProperties) ? &_properties[id] : nullptr;
}

Var* ObjectInstance::GetProperty(_In_ uint16_t id)
{
    return (id < _numProperties) ? &_properties[id] : nullptr;
}

HRESULT ObjectInstance::InvokeMethod(_In_ uint16_t id, _In_ uint16_t numArgs, _In_count_(numArgs) Var** args, _Out_opt_ Var* returnValue)
{
    UNREFERENCED_PARAMETER(numArgs);
    UNREFERENCED_PARAMETER(args);
    UNREFERENCED_PARAMETER(returnValue);

    if (id > _numMethods)
        return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);

    return _methods[id]->Invoke(this, numArgs, args, returnValue);
}

HRESULT ObjectInstance::InitializeFromTypeInfo(_In_ const TypeInfo* typeInfo)
{
    if (!typeInfo)
        return E_INVALIDARG;

    HRESULT hr = S_OK;

    _numProperties = typeInfo->GetNumProperties();

    _properties = new Var[_numProperties];
    hr = _properties ? S_OK : E_OUTOFMEMORY;

    if (SUCCEEDED(hr))
    {
        for (uint16_t i = 0; i < _numProperties; ++i)
        {
            auto propInfo = typeInfo->GetPropertyInfo(i);
            if (!propInfo)
            {
                hr = E_UNEXPECTED;
                break;
            }

            _properties[i].SetType(propInfo->Type);
        }
    }

    if (SUCCEEDED(hr))
    {
        _numMethods = typeInfo->GetNumMethods();

        _methods = new Method*[_numMethods];
        hr = _methods ? S_OK : E_OUTOFMEMORY;
    }

    if (SUCCEEDED(hr))
    {
        for (uint16_t i = 0; i < _numMethods; ++i)
        {
            auto methodInfo = typeInfo->GetMethodInfo(i);
            if (!methodInfo)
            {
                hr = E_UNEXPECTED;
                break;
            }

            _methods[i] = typeInfo->GetMethodInfo(i)->MethodInstance;
        }
    }

    if (FAILED(hr))
    {
        SafeDeleteArray(_properties);
        SafeDeleteArray(_methods);

        _numProperties = 0;
        _numMethods = 0;
    }

    return hr;
}
