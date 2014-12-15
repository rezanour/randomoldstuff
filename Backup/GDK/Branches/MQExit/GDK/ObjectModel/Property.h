// A Property is a single value, of a particular type, on the object. Properties are visible from script,
// and are generally used for game-related information
#pragma once

#include "..\Platform.h"
#include "..\RefCounted.h"

namespace GDK
{
    enum PropertyType
    {
        Uint64,
        Integer,
        Float,
        Boolean,
        String,
    };

    struct IProperty : public IRefCounted
    {
        GDK_METHOD_(const char* const) GetName() const = 0;
        GDK_METHOD_(PropertyType) GetType() const = 0;

        GDK_METHOD AsFloat(_Out_ float* pValue) const = 0;
        GDK_METHOD AsInt(_Out_ int* pValue) const = 0;
        GDK_METHOD AsUint64(_Out_ unsigned long long* pValue) const = 0;
        GDK_METHOD AsBool(_Out_ bool* pValue) const = 0;
        GDK_METHOD AsString(_Out_cap_(cchMax) char* pszValue, _In_ size_t cchMax) const = 0;

        GDK_METHOD SetFloat(_In_ float value) = 0;
        GDK_METHOD SetInt(_In_ int value) = 0;
        GDK_METHOD SetUint64(_In_ unsigned long long value) = 0;
        GDK_METHOD SetBool(_In_ bool value) = 0;
        GDK_METHOD SetString(_In_ const char* value) = 0;
    };
}
