#pragma once

#include <stde\types.h>
#include <stde\non_copyable.h>
#include <GDK\ObjectModel\Property.h>

namespace CoreServices
{
    class Property : stde::non_copyable, public GDK::RefCountedBase<GDK::IProperty>
    {
    public:
        Property(_In_ const std::string& name, _In_ GDK::PropertyType Type);
        ~Property();

        // IProperty
        GDK_METHOD_(const char* const) GetName() const;
        GDK_METHOD_(GDK::PropertyType) GetType() const;

        GDK_METHOD AsFloat(_Out_ float* pValue) const;
        GDK_METHOD AsInt(_Out_ int* pValue) const;
        GDK_METHOD AsUint64(_Out_ uint64* pValue) const;
        GDK_METHOD AsBool(_Out_ bool* pValue) const;
        GDK_METHOD AsString(_Out_cap_(cchMax) char* pszValue, _In_ size_t cchMax) const;

        GDK_METHOD SetFloat(_In_ float value);
        GDK_METHOD SetInt(_In_ int value);
        GDK_METHOD SetUint64(_In_ uint64 value);
        GDK_METHOD SetBool(_In_ bool value);
        GDK_METHOD SetString(_In_ const char* value);

    private:
        std::string _name;
        GDK::PropertyType _type;

        // TODO: Move this out to a Var type or something?
        union
        {
            float _floatValue;
            bool _boolValue;
            int _intValue;
            uint64 _uint64Value;
        };
        std::string _stringValue;
    };
}

