#if defined(_MSC_VER)
#pragma once
#endif

#ifndef _GDK_VAR_INL_
#define _GDK_VAR_INL_

namespace GDK
{
    inline value_t Var::FromBool(bool value)
    {
        return *(value_t*)&value;
    }

    inline value_t Var::FromInt(int_t value)
    {
        return *(value_t*)&value;
    }

    inline value_t Var::FromReal(real_t value)
    {
        return *(value_t*)&value;
    }

    inline value_t Var::FromString(const StringID& value)
    {
        return *(value_t*)&value._value;
    }

    inline bool Var::ToBool(value_t value)
    {
        return *(bool*)&value;
    }

    inline int_t Var::ToInt(value_t value)
    {
        return *(int_t*)&value;
    }

    inline real_t Var::ToReal(value_t value)
    {
        return *(real_t*)&value;
    }

    inline StringID Var::ToString(value_t value)
    {
        return *(StringID*)&value;
    }

    inline Var::Var()
        : _type(VarType_Empty), _value(0)
    {
    }

    inline Var::Var(const Var& other)
        : _type(other._type), _value(other._value)
    {
    }

    inline Var::Var(bool value)
        : _type(VarType_Bool), _value(FromBool(value))
    {
    }

    inline Var::Var(int_t value)
        : _type(VarType_Int), _value(FromInt(value))
    {
    }

    inline Var::Var(real_t value)
        : _type(VarType_Real), _value(FromReal(value))
    {
    }

    inline Var::Var(const StringID& value)
        : _type(VarType_StringID), _value(FromString(value))
    {
    }

    inline Var& Var::operator= (const Var& other)
    {
        _type = other._type;
        _value = other._value;
        return *this;
    }

    inline void Var::SetType(VarType type)
    {
        _type = type;
    }

    inline VarType Var::GetType() const
    {
        return _type;
    }

    inline bool Var::IsEmpty() const
    {
        return _type == VarType_Empty;
    }

    inline bool Var::IsBool() const
    {
        return _type == VarType_Bool;
    }

    inline bool Var::IsInt() const
    {
        return _type == VarType_Int;
    }

    inline bool Var::IsReal() const
    {
        return _type == VarType_Real;
    }

    inline bool Var::IsString() const
    {
        return _type == VarType_StringID;
    }

    inline bool Var::ToBool() const
    {
        return ToBool(_value);
    }

    inline int_t Var::ToInt() const
    {
        return ToInt(_value);
    }

    inline real_t Var::ToReal() const
    {
        return ToReal(_value);
    }

    inline StringID Var::ToString() const
    {
        return ToString(_value);
    }

    inline void Var::SetBool(bool value)
    {
        _value = FromBool(value);
    }

    inline void Var::SetInt(int_t value)
    {
        _value = FromInt(value);
    }

    inline void Var::SetReal(real_t value)
    {
        _value = FromReal(value);
    }

    inline void Var::SetString(const StringID& value)
    {
        _value = FromString(value);
    }
} // GDK

#endif // _GDK_VAR_INL_
