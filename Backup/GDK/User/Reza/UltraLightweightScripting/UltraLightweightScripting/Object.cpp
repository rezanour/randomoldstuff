#include "StdAfx.h"
#include "Object.h"
#include "Variable.h"
#include "Method.h"

using GDK::Object;
using GDK::Variable;
using GDK::Method;

uint32_t Object::s_nextID = 0;

HRESULT DudeTakeDamage(_In_opt_ Object* instance, _In_ size_t numParams, _In_ Variable** parameters, _Out_opt_ Variable* returnValue);

HRESULT Object::Create(const std::wstring& name, Object** object)
{
    if (!object)
        return E_POINTER;

    HRESULT hr = S_OK;
    *object = nullptr;

    Object* obj = new Object(name);
    hr = obj ? S_OK : E_OUTOFMEMORY;

    if (SUCCEEDED(hr))
    {
        obj->_properties[L"Health"] = std::make_shared<Variable>(100);
        obj->_properties[L"Damage"] = std::make_shared<Variable>(32.56);
        obj->_properties[L"Boss"] = std::make_shared<Variable>(true);
        obj->_properties[L"WeaponName"] = std::make_shared<Variable>(L"Chaingun");

        VariableType amount = VariableType_Real;
        obj->_methods[L"TakeDamage"].reset(new Method(VariableType_Empty, 1, &amount, DudeTakeDamage));
    }

    if (SUCCEEDED(hr))
    {
        *object = obj;
    }
    else if (obj)
    {
        delete obj;
    }

    return hr;
}

Object::Object(const std::wstring& name)
    : _id(s_nextID++), _name(name)
{
}

Object::~Object()
{
}

uint32_t Object::GetID() const
{
    return _id;
}

const wchar_t* Object::GetName() const
{
    return _name.c_str();
}

size_t Object::GetPropertyCount() const
{
    return _properties.size();
}

const wchar_t* Object::GetPropertyName(size_t index) const
{
    if (index < _properties.size())
    {
        PropertyMap::const_iterator it = _properties.cbegin();
        for (size_t i = 0; i < index; ++i, ++it);
        return it->first.c_str();
    }

    return nullptr;
}

std::shared_ptr<Variable> Object::GetProperty(const std::wstring& name)
{
    PropertyMap::iterator it = _properties.find(name);
    return (it != _properties.end() ? it->second : nullptr);
}

std::shared_ptr<Variable> Object::GetProperty(size_t index)
{
    if (index < _properties.size())
    {
        PropertyMap::const_iterator it = _properties.cbegin();
        for (size_t i = 0; i < index; ++i, ++it);
        return it->second;
    }

    return nullptr;
}

size_t Object::GetMethodCount() const
{
    return _methods.size();
}

const wchar_t* Object::GetMethodName(size_t index) const
{
    if (index < _methods.size())
    {
        MethodMap::const_iterator it = _methods.cbegin();
        for (size_t i = 0; i < index; ++i, ++it);
        return it->first.c_str();
    }

    return nullptr;
}

std::shared_ptr<Method> Object::GetMethod(const std::wstring& name)
{
    MethodMap::iterator it = _methods.find(name);
    return (it != _methods.end() ? it->second : nullptr);
}

std::shared_ptr<Method> Object::GetMethod(size_t index)
{
    if (index < _methods.size())
    {
        MethodMap::const_iterator it = _methods.cbegin();
        for (size_t i = 0; i < index; ++i, ++it);
        return it->second;
    }

    return nullptr;
}

HRESULT Object::InvokeMethod(_In_ const std::wstring& name, _In_ size_t numParams, _In_ Variable** parameters, _Out_opt_ Variable* returnValue)
{
    auto method = GetMethod(name);
    if (!method)
    {
        return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
    }

    return method->Invoke(this, numParams, parameters, returnValue);
}

HRESULT DudeTakeDamage(_In_opt_ Object* instance, _In_ size_t numParams, _In_ Variable** parameters, _Out_opt_ Variable* returnValue)
{
    if (!instance)
        return E_INVALIDARG;

    auto health = instance->GetProperty(L"Health");
    if (health)
    {
        health->SetValue(health->AsReal() - parameters[0]->AsReal());
    }

    UNREFERENCED_PARAMETER(numParams);
    UNREFERENCED_PARAMETER(returnValue);

    return S_OK;
}
