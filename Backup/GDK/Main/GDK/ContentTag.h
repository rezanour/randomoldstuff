// ContentTag is a complete identifier for content. It contains type and name information
#pragma once

#include "Platform.h"
#include "RefCounted.h"
#include "ContentTypes.h"

namespace GDK
{
    struct IContentTag : public IRefCounted
    {
        // Get content type
        GDK_IMETHOD_(ContentType::Enum)    GetType() const = 0;

        // Get sub type, if applicable
        GDK_IMETHOD_(ObjectType::Enum)     GetObjectType() const = 0;
        GDK_IMETHOD_(ComponentType::Enum)  GetComponentType() const = 0;
        GDK_IMETHOD_(AssetType::Enum)      GetAssetType() const = 0;
        GDK_IMETHOD_(ResourceType::Enum)   GetResourceType() const = 0;

        // A unique, unsigned 64bit value which can be used to refer to the content
        GDK_IMETHOD_(unsigned long long) GetID() const = 0;

        // A customizable string representing the name of the object. This can change and not guaranteed
        // to be unique, so it should only be used as a reference, and not for identification.
        GDK_IMETHOD_(const char* const) GetName() const = 0;
    };

    GDK_INLINE HRESULT AreContentTagsEqual(_In_ const IContentTag* pTag1, _In_ const IContentTag* pTag2)
    {
        if (!pTag1 || !pTag2)
            return E_INVALIDARG;

        return pTag1->GetID() == pTag2->GetID();
    };
}
