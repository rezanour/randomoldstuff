#pragma once

#include "IObject.h"

namespace GDK
{
    //
    // GetTypeOf specializations
    //

    // specialization for Value
    template<>
    inline ValueType Value::GetTypeOf<Value>(_In_ const Value& value)
    {
        return value.GetType();
    }

#define GETTYPEOF_SPECIALIZATION(type, valueType) \
    template <> \
    inline ValueType Value::GetTypeOf(_In_ const type&) \
    { \
        return valueType; \
    }

#define GETTYPEOF_SPECIALIZATION_PTR(type, valueType) \
    template <> \
    inline ValueType Value::GetTypeOf(_In_ const type*) \
    { \
        return valueType; \
    }

    GETTYPEOF_SPECIALIZATION(bool, ValueType_Boolean);
    GETTYPEOF_SPECIALIZATION(float, ValueType_Number);
    GETTYPEOF_SPECIALIZATION(double, ValueType_Number);
    GETTYPEOF_SPECIALIZATION(int16_t, ValueType_Number);
    GETTYPEOF_SPECIALIZATION(uint16_t, ValueType_Number);
    GETTYPEOF_SPECIALIZATION(int32_t, ValueType_Number);
    GETTYPEOF_SPECIALIZATION(uint32_t, ValueType_Number);
    GETTYPEOF_SPECIALIZATION(int64_t, ValueType_Number);

    GETTYPEOF_SPECIALIZATION_PTR(char, ValueType_String);
    GETTYPEOF_SPECIALIZATION_PTR(wchar_t, ValueType_String);

#undef GETTYPEOF_SPECIALIZATION
#undef GETTYPEOF_SPECIALIZATION_PTR

    //
    // Constructor templates
    //
    template <typename Ty>
    inline Value::Value(_In_ const Ty& value)
        : _type(GetTypeOf(value))
    {
        SetValue(value);
    }

    template <typename Ty>
    inline Value::Value(_In_ const Ty* value)
        : _type(GetTypeOf(value))
    {
        SetValue(value);
    }

    //
    // Set templates
    //
    template <typename Ty>
    inline bool Value::Set(_In_ const Ty& value)
    {
        if (_type == ValueType_Empty)
        {
            _type = GetTypeOf(value);
        }
        else if (_type != GetTypeOf(value))
        {
            return false;
        }

        SetValue(value);
        return true;
    }

    template <typename Ty>
    inline bool Value::Set(_In_ const Ty* value)
    {
        if (_type == ValueType_Empty)
        {
            _type = GetTypeOf(value);
        }
        else if (_type != GetTypeOf(value))
        {
            return false;
        }

        SetValue(value);
        return true;
    }

    //
    // SetValue templates
    //
    template <>
    inline void Value::SetValue(_In_ const Value& value)
    {
        ClearValue();
        _number = value._number;
        if (_type == ValueType_Object && _object)
        {
            _object->AddRef();
        }
    }

    template <typename Ty>
    inline void Value::SetValue(_In_ const Ty& value)
    {
        ClearValue();
        _number = static_cast<double>(value);
    }

    template <typename Ty>
    inline void Value::SetValue(_In_ const Ty* value)
    {
        // clear value ensures object type is released properly
        ClearValue();

        if (_type == ValueType_Object)
        {
            _object = reinterpret_cast<IObject*>(const_cast<Ty*>((value)));
            if (_object)
            {
                _object->AddRef();
            }
        }
    }
} // GDK
