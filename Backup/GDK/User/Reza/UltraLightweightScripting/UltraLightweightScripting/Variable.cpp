#include "StdAfx.h"
#include "Variable.h"
#include "Object.h"

using GDK::VariableType;
using GDK::Variable;
using GDK::Object;

Variable::Variable()
    : _type(VariableType_Empty), _integer(0)
{
}

Variable::Variable(VariableType type)
    : _type(type), _integer(0)
{
}

Variable::Variable(bool value)
    : _type(VariableType_Empty), _integer(0)
{
    SetValue(value);
}

Variable::Variable(int8_t value)
    : _type(VariableType_Empty), _integer(0)
{
    SetValue(value);
}

Variable::Variable(int16_t value)
    : _type(VariableType_Empty), _integer(0)
{
    SetValue(value);
}

Variable::Variable(int32_t value)
    : _type(VariableType_Empty), _integer(0)
{
    SetValue(value);
}

Variable::Variable(int64_t value)
    : _type(VariableType_Empty), _integer(0)
{
    SetValue(value);
}

Variable::Variable(double value)
    : _type(VariableType_Empty), _integer(0)
{
    SetValue(value);
}

Variable::Variable(wchar_t value)
    : _type(VariableType_Empty), _integer(0)
{
    SetValue(value);
}

Variable::Variable(const wchar_t* value)
    : _type(VariableType_Empty), _integer(0)
{
    SetValue(value);
}

Variable::Variable(Object* value)
    : _type(VariableType_Empty), _integer(0)
{
    SetValue(value);
}

Variable::~Variable()
{
    ClearValue();
}

void Variable::Clear()
{
    _type = VariableType_Empty;
    ClearValue();
}

VariableType Variable::GetType() const
{
    return _type;
}

bool Variable::IsEmpty() const
{
    return _type == VariableType_Empty;
}

bool Variable::IsBoolean() const
{
    return _type == VariableType_Boolean;
}

bool Variable::IsInteger() const
{
    return _type == VariableType_Integer;
}

bool Variable::IsReal() const
{
    return _type == VariableType_Real;
}

bool Variable::IsNumber() const
{
    return IsInteger() || IsReal();
}

bool Variable::IsChar() const
{
    return _type == VariableType_Char;
}

bool Variable::IsString() const
{
    return _type == VariableType_String;
}

bool Variable::IsObject() const
{
    return _type == VariableType_Object;
}

bool Variable::AsBoolean() const
{
    return (IsBoolean() ? _bool : false);
}

int64_t Variable::AsInteger() const
{
    return (IsInteger() ? _integer :
            (IsReal() ? (int64_t)_real : 0));
}

double Variable::AsReal() const
{
    return (IsReal() ? _real :
            (IsInteger() ? (double)_integer : 0.0));
}

wchar_t Variable::AsChar() const
{
    return (IsChar() ? _char : 0);
}

const wchar_t* Variable::AsString() const
{
    return (IsString() ? _string : nullptr);
}

Object* Variable::AsObject() const
{
    return (IsObject() ? _object : nullptr);
}

void Variable::ChangeType(VariableType type)
{
    _type = type;
}

void Variable::SetValue(bool value)
{
    switch (_type)
    {
    case VariableType_Empty:
        _type = VariableType_Boolean;
    case VariableType_Boolean:
        ClearValue();
        _bool = value;
        break;
    }
}

void Variable::SetValue(int8_t value)
{
    switch (_type)
    {
    case VariableType_Empty:
        _type = VariableType_Integer;
    case VariableType_Integer:
        ClearValue();
        _integer = value;
        break;

    case VariableType_Real:
        ClearValue();
        _real = (double)value;
        break;
    }
}

void Variable::SetValue(int16_t value)
{
    switch (_type)
    {
    case VariableType_Empty:
        _type = VariableType_Integer;
    case VariableType_Integer:
        ClearValue();
        _integer = value;
        break;

    case VariableType_Real:
        ClearValue();
        _real = (double)value;
        break;
    }
}

void Variable::SetValue(int32_t value)
{
    switch (_type)
    {
    case VariableType_Empty:
        _type = VariableType_Integer;
    case VariableType_Integer:
        ClearValue();
        _integer = value;
        break;

    case VariableType_Real:
        ClearValue();
        _real = (double)value;
        break;
    }
}

void Variable::SetValue(int64_t value)
{
    switch (_type)
    {
    case VariableType_Empty:
        _type = VariableType_Integer;
    case VariableType_Integer:
        ClearValue();
        _integer = value;
        break;

    case VariableType_Real:
        ClearValue();
        _real = (double)value;
        break;
    }
}

void Variable::SetValue(double value)
{
    switch (_type)
    {
    case VariableType_Empty:
        _type = VariableType_Real;
    case VariableType_Real:
        ClearValue();
        _real = value;
        break;

    case VariableType_Integer:
        ClearValue();
        _integer = (int64_t)value;
        break;
    }
}

void Variable::SetValue(wchar_t value)
{
    switch (_type)
    {
    case VariableType_Empty:
        _type = VariableType_Char;
    case VariableType_Char:
        ClearValue();
        _char = value;
        break;
    }
}

void Variable::SetValue(const wchar_t* value)
{
    switch (_type)
    {
    case VariableType_Empty:
        _type = VariableType_String;
    case VariableType_String:
        ClearValue();
        _string = _wcsdup(value);
        break;
    }
}

void Variable::SetValue(Object* value)
{
    switch (_type)
    {
    case VariableType_Empty:
        _type = VariableType_Object;
    case VariableType_Object:
        ClearValue();
        _object = value;
        if (_object)
        {
            //_object->AddRef();
        }
        break;
    }
}

void Variable::ClearValue()
{
    // if data is all 0s, then there's nothing to do
    if (_integer != 0)
    {
        if (_type == VariableType_String)
        {
            free(_string);
        }
        else if (_type == VariableType_Object)
        {
            //SafeRelease(_object);
        }

        _integer = 0;
    }
}
