#include "StdAfx.h"
#include "Property.h"

using GDK::ValueType;
using GDK::Value;
using GDK::Property;
using GDK::MethodInfo;

Property::Property(_In_ ValueType type, _In_ const char* name)
    : _variable(name, type), _readonly(false), _getter(nullptr), _setter(nullptr)
{
}

Property::~Property()
{
}

bool Property::GetValue(_Out_ Value* value) const
{
    if (!value)
    {
        return false;
    }

    if (_getter)
    {
        return SUCCEEDED(_getter->InvokeMethod(0, nullptr, value));
    }
    else
    {
        *value = _variable.GetValue();
        return true;
    }
}

bool Property::SetValue(_In_ Value& value)
{
    if (_readonly)
        return false;

    if (_setter)
    {
        return SUCCEEDED(_setter->InvokeMethod(1, &value, nullptr));
    }
    else
    {
        _variable.SetValue(value);
        return true;
    }
}
