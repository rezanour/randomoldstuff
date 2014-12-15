#if defined(_MSC_VER)
#pragma once
#endif

#ifndef _GDK_GAMEOBJECT_H_
#define _GDK_GAMEOBJECT_H_

namespace GDK
{
    struct Transform;

    struct IGameObject : IRefCounted
    {
        // A scene-unique identifier for the object
        GDK_IMETHOD_(unsigned long) GetID() const = 0;

        // Get the object's human-readable name
        GDK_IMETHOD_(const char* const) GetName() const = 0;

        // Set a human-readable name for the object
        GDK_IMETHOD_(void) SetName(_In_ const char* name) = 0;

        // A value that changes whenever the transform is updated. Can be used by caching mechanisms
        // to avoid unnecessarily reading an unchanged transform.
        GDK_IMETHOD_(size_t) GetTransformSalt() const = 0;

        // Get the object's world-space transform information, including position, orientation, and scale
        GDK_IMETHOD_(void) GetTransform(_Out_ Transform* pTransform) const = 0;

        // Update the object's world-space transform information. This also updates the transform salt.
        GDK_IMETHOD_(void) SetTransform(_In_ const Transform& transform) = 0;
    };
}

#endif // _GDK_GAMEOBJECT_H_
