#include "StdAfx.h"
#include "Value.h"
#include "IObject.h"

using GDK::ValueType;
using GDK::Value;
using GDK::IObject;

Value::Value()
    : _type(ValueType_Empty), _number(0)
{
}

Value::Value(_In_ ValueType type)
    : _type(type), _number(0)
{
}

Value::Value(_In_ const Value& other)
{
    *this = other;
}

Value::~Value()
{
    Clear();
}

Value& Value::operator=(const Value& other)
{
    _type = other._type;

    ClearValue();
    _number = other._number;
    if (_type == ValueType_Object && _object)
    {
        _object->AddRef();
    }

    return *this;
}

void Value::Clear()
{
    _type = ValueType_Empty;
    ClearValue();
}

void Value::ClearValue()
{
    if (_type == ValueType_Object)
    {
        SafeRelease(_object);
    }
    _number = 0;
}

bool Value::IsEmpty() const
{
    return _type == ValueType_Empty;
}

ValueType Value::GetType() const
{
    return _type;
}

bool Value::CanConvertToBoolean() const
{
    return (_type == ValueType_Number);
}

bool Value::CanConvertToNumber() const
{
    return (_type == ValueType_Number);
}

bool Value::AsBoolean(_Out_ bool* value)
{
    if (!value || !CanConvertToBoolean())
        return false;

    *value = (_number != 0.0);
    return true;
}

bool Value::AsNumber(_Out_ double* value)
{
    if (!value || !CanConvertToNumber())
        return false;

    *value = static_cast<double>(_number);
    return true;
}

bool Value::AsObject(_Out_ IObject** value)
{
    if (!value || _type != ValueType_Object)
        return false;

    *value = _object;
    if (_object)
    {
        _object->AddRef();
    }
    return true;
}
