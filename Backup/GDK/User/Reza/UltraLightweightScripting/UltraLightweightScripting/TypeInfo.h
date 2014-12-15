#pragma once

#include "Var.h"
#include "Method.h"

namespace GDK
{
    struct PropertyInfo
    {
        std::string Name;
        VarType Type;

        PropertyInfo(_In_ const char* name, _In_ VarType type)
            : Name(name), Type(type)
        {
        }
    };

    struct MethodInfo
    {
        std::string Name;
        Method* MethodInstance;

        MethodInfo(_In_ const char* name, _In_ Method* method)
            : Name(name), MethodInstance(method)
        {
        }
    };

    class TypeInfo
    {
    public:
        TypeInfo(_In_ const char* name);
        ~TypeInfo();

        uint32_t GetID() const;
        const char* GetName() const;

        uint16_t GetNumProperties() const;
        const PropertyInfo* GetPropertyInfo(_In_ uint16_t index) const;

        uint16_t GetNumMethods() const;
        const MethodInfo* GetMethodInfo(_In_ uint16_t index) const;

        // Design mode only
        HRESULT AddProperty(_In_ const char* name, _In_ VarType type);
        HRESULT AddMethod(_In_ const char* name, _In_ Method* method);

    private:
        uint32_t    _id;
        std::string _name;
        std::vector<PropertyInfo> _properties;
        std::vector<MethodInfo> _methods;

        static uint32_t s_nextID;
    };


} // GDK
