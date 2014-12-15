#if defined(_MSC_VER)
#pragma once
#endif

#ifndef _GDK_VAR_H_
#define _GDK_VAR_H_

namespace GDK
{
    typedef uint64_t value_t;
    typedef int64_t int_t;
    typedef double real_t;

    enum VarType
    {
        VarType_Empty = 0,
        VarType_Bool,
        VarType_Int,
        VarType_Real,
        VarType_StringID,
        VarType_Param,
        VarType_Local
    };

    class Var
    {
    public:
        // Conversions to and from storage format
        static value_t FromBool(bool value);
        static value_t FromInt(int_t value);
        static value_t FromReal(real_t value);
        static value_t FromString(const StringID& value);

        static bool ToBool(value_t value);
        static int_t ToInt(value_t value);
        static real_t ToReal(value_t value);
        static StringID ToString(value_t value);

        // Construction
        Var();
        explicit Var(const Var& other);
        explicit Var(bool value);
        explicit Var(int_t value);
        explicit Var(real_t value);
        explicit Var(const StringID& value);

        // Methods
        Var& operator= (const Var& other);

        void SetType(VarType type);
        VarType GetType() const;

        bool IsEmpty() const;
        bool IsBool() const;
        bool IsInt() const;
        bool IsReal() const;
        bool IsString() const;

        bool ToBool() const;
        int_t ToInt() const;
        real_t ToReal() const;
        StringID ToString() const;

        void SetBool(bool value);
        void SetInt(int_t value);
        void SetReal(real_t value);
        void SetString(const StringID& value);

    private:
        VarType _type;
        value_t _value;
    };
} // GDK

#include "Var.inl"

#endif // _GDK_VAR_H_
