#pragma once

namespace GDK
{
    enum VarType
    {
        VarType_Empty = 0,
        VarType_Boolean,
        VarType_Integer,
        VarType_Real,
        VarType_StringID,
        VarType_Object,
    };

    class ObjectInstance;

    class Var
    {
    public:
        Var();
        ~Var();

        VarType GetType() const;
        void SetType(_In_ VarType type);

        bool IsEmpty() const;
        bool IsCompatible(_In_ VarType type) const;
        bool IsBoolean() const;
        bool IsInteger() const;
        bool IsReal() const;
        bool IsNumber() const;
        bool IsString() const;
        bool IsObject() const;

        static bool IsBoolean(_In_ VarType type);
        static bool IsInteger(_In_ VarType type);
        static bool IsReal(_In_ VarType type);
        static bool IsNumber(_In_ VarType type);
        static bool IsString(_In_ VarType type);
        static bool IsObject(_In_ VarType type);

        template <typename Ty>
        static VarType TypeOf(_In_ const Ty& value);

        template <typename Ty>
        static VarType TypeOf(_In_ const Ty* value);

        bool            GetBoolean() const;
        int64_t         GetInteger() const;
        double          GetReal() const;
        uint64_t        GetString() const;
        ObjectInstance* GetObject() const;

        template <typename Ty>
        void Set(_In_ const Ty& value);

        template <typename Ty>
        void Set(_In_ const Ty* value);

        void SetString(_In_ uint64_t value);

    private:
        VarType _type;

        union
        {
            bool            _boolean;
            int64_t         _integer;
            double          _real;
            uint64_t        _stringID;
            ObjectInstance* _object;
        };
    };
} // GDK

#include "Var.inl"
