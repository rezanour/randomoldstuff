#pragma once

#include <stde\non_copyable.h>
#include <GDK\ContentTag.h>
#include <stde\ref_ptr.h>
#include <stde\com_ptr.h>

namespace CoreServices
{
    class ContentTag;
    typedef stde::ref_ptr<ContentTag> ContentTagPtr;

    class ContentTag : stde::non_copyable, public GDK::RefCounted<GDK::IContentTag>
    {
    public:
        ContentTag(_In_ GDK::ContentType::Enum contentType, _In_ uint32 subContentType, _In_ const std::string& name);
        ContentTag(_In_ GDK::ContentType::Enum contentType, _In_ uint32 subContentType, _In_ const std::string& name, _In_ uint64 id);

        // Serialization
        static HRESULT Load(_In_ stde::com_ptr<IStream>& spStream, _Out_ ContentTagPtr& spContentTag);
        HRESULT Save(_In_ stde::com_ptr<IStream>& spStream);

        // IContentTag

        // Get content type
        GDK_IMETHOD_(GDK::ContentType::Enum) GetType() const;

        // Get sub type, if applicable
        GDK_IMETHOD_(GDK::ObjectType::Enum)     GetObjectType() const;
        GDK_IMETHOD_(GDK::ComponentType::Enum)  GetComponentType() const;
        GDK_IMETHOD_(GDK::AssetType::Enum)      GetAssetType() const;
        GDK_IMETHOD_(GDK::ResourceType::Enum)   GetResourceType() const;

        // A unique, unsigned 64bit value which can be used to refer to the content
        GDK_IMETHOD_(uint64) GetID() const;

        // A customizable string representing the name of the object. This can change and not guaranteed
        // to be unique, so it should only be used as a reference, and not for identification.
        GDK_IMETHOD_(const char* const) GetName() const;

    private:
        GDK::ContentType::Enum _contentType;
        std::string _name;
        uint64 _id;
        union
        {
            GDK::ObjectType::Enum _objectType;
            GDK::ComponentType::Enum _componentType;
            GDK::AssetType::Enum _assetType;
            GDK::ResourceType::Enum _resourceType;
        };
    };
}
