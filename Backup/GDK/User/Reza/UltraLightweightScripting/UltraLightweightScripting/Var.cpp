#include "StdAfx.h"
#include "Var.h"
#include "ObjectInstance.h"

using GDK::VarType;
using GDK::Var;
using GDK::ObjectInstance;

Var::Var()
    : _type(VarType_Empty), _integer(0)
{
}

Var::~Var()
{
    if (_type == VarType_Object)
    {
        SafeRelease(_object);
    }
}

VarType Var::GetType() const
{
    return _type;
}

void Var::SetType(_In_ VarType type)
{
    _type = type;
}

bool Var::IsEmpty() const
{
    return _type == VarType_Empty;
}

bool Var::IsCompatible(_In_ VarType type) const
{
    switch (_type)
    {
    case VarType_Empty:
        return true;
    case VarType_Boolean:
        return IsBoolean(type);
    case VarType_Integer:
    case VarType_Real:
        return IsNumber(type);
    case VarType_StringID:
        return IsString(type);
    case VarType_Object:
        return IsObject(type);
    }

    return false;
}

bool Var::IsBoolean() const
{
    return IsBoolean(_type);
}

bool Var::IsInteger() const
{
    return IsInteger(_type);
}

bool Var::IsReal() const
{
    return IsReal(_type);
}

bool Var::IsNumber() const
{
    return IsNumber(_type);
}

bool Var::IsString() const
{
    return IsString(_type);
}

bool Var::IsObject() const
{
    return IsObject(_type);
}

bool Var::IsBoolean(_In_ VarType type)
{
    return type == VarType_Boolean;
}

bool Var::IsInteger(_In_ VarType type)
{
    return type == VarType_Integer;
}

bool Var::IsReal(_In_ VarType type)
{
    return type == VarType_Real;
}

bool Var::IsNumber(_In_ VarType type)
{
    return type == VarType_Integer || type == VarType_Real;
}

bool Var::IsString(_In_ VarType type)
{
    return type == VarType_StringID;
}

bool Var::IsObject(_In_ VarType type)
{
    return type == VarType_Object;
}

bool Var::GetBoolean() const
{
    switch (_type)
    {
    case VarType_Boolean:   return _boolean;
    default:                return 0;
    }
}

int64_t Var::GetInteger() const
{
    switch (_type)
    {
    case VarType_Integer:   return _integer;
    case VarType_Real:      return static_cast<int64_t>(_real);
    default:                return 0;
    }
}

double Var::GetReal() const
{
    switch (_type)
    {
    case VarType_Integer:   return static_cast<double>(_integer);
    case VarType_Real:      return _real;
    default:                return 0.0;
    }
}

uint64_t Var::GetString() const
{
    switch (_type)
    {
    case VarType_StringID:  return _stringID;
    default:                return 0;
    }
}

ObjectInstance* Var::GetObject() const
{
    switch (_type)
    {
    case VarType_Object:    return _object;
    default:                return nullptr;
    }
}

void Var::SetString(_In_ uint64_t value)
{
    if (IsCompatible(VarType_StringID))
    {
        _stringID = value;
    }
}
