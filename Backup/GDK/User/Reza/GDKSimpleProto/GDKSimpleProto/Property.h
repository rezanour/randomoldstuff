#pragma once

#include "Variable.h"
#include "MethodInfo.h"

namespace GDK
{
    class Property
    {
    public:
        Property(_In_ ValueType type, _In_ const char* name);
        ~Property();

        bool GetValue(_Out_ Value* value) const;
        bool SetValue(_In_ Value& value);

    private:
        Variable _variable;
        bool _readonly;
        MethodInfo* _getter;
        MethodInfo* _setter;
    };
} // GDK
