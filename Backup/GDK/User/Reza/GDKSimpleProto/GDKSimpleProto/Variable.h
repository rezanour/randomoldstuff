#pragma once

#include "Value.h"

namespace GDK
{
    class Variable
    {
    public:
        // Construction
        Variable(_In_opt_ const char* name, _In_ const Value& value);
        Variable(_In_opt_ const char* name, _In_ ValueType type);
        explicit Variable(_In_ const Value& value);
        explicit Variable(_In_ ValueType type);

        // Destruction
        ~Variable();

        // Accessors
        const char* GetName() const;
        const Value& GetValue() const;

        bool SetValue(_In_ const Value& value);

        // Methods
        bool CopyValueTo(Variable& other);

    private:
        Variable(const Variable&);
        Variable& operator=(const Variable&);

        char* _name;
        Value _value;
    };
} // GDK
