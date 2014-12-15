#include "StdAfx.h"
#include "TypeInfo.h"

using GDK::VarType;
using GDK::PropertyInfo;
using GDK::MethodInfo;
using GDK::Method;
using GDK::TypeInfo;

uint32_t TypeInfo::s_nextID = 0;

TypeInfo::TypeInfo(_In_ const char* name)
    : _name(name), _id(s_nextID++)
{
}

TypeInfo::~TypeInfo()
{
}

uint32_t TypeInfo::GetID() const
{
    return _id;
}

const char* TypeInfo::GetName() const
{
    return _name.c_str();
}

uint16_t TypeInfo::GetNumProperties() const
{
    return static_cast<uint16_t>(_properties.size());
}

const PropertyInfo* TypeInfo::GetPropertyInfo(_In_ uint16_t index) const
{
    return (index < _properties.size()) ? &_properties[index] : nullptr;
}

uint16_t TypeInfo::GetNumMethods() const
{
    return static_cast<uint16_t>(_methods.size());
}

const MethodInfo* TypeInfo::GetMethodInfo(_In_ uint16_t index) const
{
    return (index < _methods.size()) ? &_methods[index] : nullptr;
}

HRESULT TypeInfo::AddProperty(_In_ const char* name, _In_ VarType type)
{
    _properties.push_back(PropertyInfo(name, type));
    return S_OK;
}

HRESULT TypeInfo::AddMethod(_In_ const char* name, _In_ Method* method)
{
    _methods.push_back(MethodInfo(name, method));
    return S_OK;
}
