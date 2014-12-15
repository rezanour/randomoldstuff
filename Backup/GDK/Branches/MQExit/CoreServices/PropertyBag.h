#pragma once

#include <stde\non_copyable.h>
#include <GDK\ObjectModel\PropertyBag.h>
#include <map>
#include <stde\com_ptr.h>
#include <stde\ref_counted.h>

#include "Property.h"

namespace CoreServices
{
    class PropertyBag;
    typedef stde::ref_counted_ptr<GDK::IPropertyBag> PropertyBagPtr;

    class PropertyBag : stde::non_copyable, public GDK::RefCountedBase<GDK::IPropertyBag>
    {
    public:
        PropertyBag();
        ~PropertyBag();

        // Save/Load
        static HRESULT Load(_In_ stde::com_ptr<IStream> spStream, _Out_ PropertyBagPtr& spPropertyBag);
        HRESULT Save(_In_ stde::com_ptr<IStream> spStream);

        // IPropertyBag
        GDK_METHOD_(size_t) GetCount() const;

        GDK_METHOD CreateProperty(_In_ GDK::PropertyType type, _In_ const char* name, _Deref_out_ GDK::IProperty** ppProperty);
        GDK_METHOD GetProperty(_In_ const char* name, _Deref_out_ GDK::IProperty** ppProperty) const;
        GDK_METHOD GetProperty(_In_ size_t index, _Deref_out_ GDK::IProperty** ppProperty) const;
        GDK_METHOD DeleteProperty(_In_ const char* name);
        GDK_METHOD_(void) Clear();

    private:
        typedef std::map<std::string, stde::ref_counted_ptr<Property>> PropertyMap;
        PropertyMap _properties;
    };
}
