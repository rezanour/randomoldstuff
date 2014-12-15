#pragma once

namespace GDK
{
    __interface IObject;

    enum ValueType
    {
        ValueType_Empty,
        ValueType_Boolean,
        ValueType_Number,
        ValueType_String,
        ValueType_Object,
    };

    class Value
    {
    public:
        //
        // Get ValueType from various common type (see .inl file for complete specialization list)
        //
        template <typename Ty>
        static ValueType GetTypeOf(_In_ const Ty& value);

        template <typename Ty>
        static ValueType GetTypeOf(_In_ const Ty* value);

        //
        // Construction
        //

        // Constructs an empty value
        Value();

        // Constructs a value from another one
        Value(_In_ const Value& other);

        // Constructs a logical zero of a particular type
        explicit Value(_In_ ValueType type);

        // for convenience, support construction (and implicit conversion) from various common types

        // value types
        template <typename Ty>
        Value(_In_ const Ty& value);

        // pointer types
        template <typename Ty>
        Value(_In_ const Ty* value);

        //
        // Destruction
        //

        ~Value();

        //
        // Operators
        //

        Value& operator=(_In_ const Value& other);

        //
        // Methods
        //

        // Check for empty value
        bool IsEmpty() const;

        // Clear to empty
        void Clear();

        // Clear the value to logical zero, but keep the type information
        void ClearValue();

        // Get the type of the value
        ValueType GetType() const;

        // Conversion checks
        bool CanConvertToBoolean() const;
        bool CanConvertToNumber() const;

        // Get value, returns false if no conversion exists
        bool AsBoolean(_Out_ bool* value);
        bool AsNumber(_Out_ double* value);
        bool AsObject(_Out_ IObject** value);

        // Set value, returns false if no conversion exists
        template <typename Ty>
        bool Set(_In_ const Ty& value);

        template <typename Ty>
        bool Set(_In_ const Ty* value);

    private:
        template <typename Ty>
        void SetValue(_In_ const Ty& value);

        template <typename Ty>
        void SetValue(_In_ const Ty* value);

        ValueType _type;
        union
        {
            double      _number;
            IObject*    _object;
        };
    };
} // GDK

#include "Value.inl"
