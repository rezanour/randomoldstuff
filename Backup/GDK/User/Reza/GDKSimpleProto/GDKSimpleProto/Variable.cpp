#include "StdAfx.h"
#include "Variable.h"

using GDK::ValueType;
using GDK::Value;
using GDK::Variable;

Variable::Variable(_In_opt_ const char* name, _In_ const Value& value)
    : _name(_strdup(name)), _value(value)
{
}

Variable::Variable(_In_opt_ const char* name, _In_ ValueType type)
    : _name(_strdup(name)), _value(type)
{
}

Variable::Variable(_In_ const Value& value)
    : _name(nullptr), _value(value)
{
}

Variable::Variable(_In_ ValueType type)
    : _name(nullptr), _value(type)
{
}

Variable::~Variable()
{
    if (_name)
    {
        free(_name);
    }
}

const char* Variable::GetName() const
{
    return _name;
}

const Value& Variable::GetValue() const
{
    return _value;
}

bool Variable::SetValue(_In_ const Value& value)
{
    return _value.Set(value);
}

bool Variable::CopyValueTo(Variable& other)
{
    return other.SetValue(_value);
}
