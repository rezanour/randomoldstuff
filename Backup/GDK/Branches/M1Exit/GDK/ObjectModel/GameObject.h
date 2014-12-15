// A GameObject is the central building block of a scene. Every element in a scene is a game object
#pragma once

#include "..\Platform.h"
#include "..\RefCounted.h"
#include "..\Transform.h"
#include "PropertyBag.h"

namespace GDK
{
    struct IGameObject : public IRefCounted
    {
        // A scene-unique identifier for the object
        GDK_METHOD_(unsigned long) GetID() const = 0;

        // Get the object's human-readable name
        GDK_METHOD_(const char* const) GetName() const = 0;

        // Set a human-readable name for the object
        GDK_METHOD_(void) SetName(_In_ const char* name) = 0;

        // A value that changes whenever the transform is updated. Can be used by caching mechanisms
        // to avoid unnecessarily reading an unchanged transform.
        GDK_METHOD_(size_t) GetTransformSalt() const = 0;

        // Get the object's world-space transform information, including position, orientation, and scale
        GDK_METHOD_(void) GetTransform(_Out_ Transform* pTransform) const = 0;

        // Update the object's world-space transform information. This also updates the transform salt.
        GDK_METHOD_(void) SetTransform(_In_ const Transform& transform) = 0;

        // Get the property bag associated with the object, which can contain user-defined properties
        GDK_METHOD GetPropertyBag(_Deref_out_ IPropertyBag** ppProperties) const = 0;
    };
}
