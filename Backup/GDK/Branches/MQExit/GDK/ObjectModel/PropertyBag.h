// A property bag is set of name-value pairs of IProperty's. Every game object contains a property bag,
// and some other objects do as well.
#pragma once

#include "..\Platform.h"
#include "Property.h"

namespace GDK
{
    struct IPropertyBag : public IRefCounted
    {
        GDK_METHOD_(size_t) GetCount() const = 0;

        GDK_METHOD CreateProperty(_In_ PropertyType type, _In_ const char* name, _Deref_out_ IProperty** ppProperty) = 0;
        GDK_METHOD GetProperty(_In_ const char* name, _Deref_out_ IProperty** ppProperty) const = 0;
        GDK_METHOD GetProperty(_In_ size_t index, _Deref_out_ IProperty** ppProperty) const = 0;
        GDK_METHOD DeleteProperty(_In_ const char* name) = 0;
        GDK_METHOD_(void) Clear() = 0;
    };
}
