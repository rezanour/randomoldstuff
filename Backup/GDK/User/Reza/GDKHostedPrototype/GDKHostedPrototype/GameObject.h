#if defined(_MSC_VER)
#pragma once
#endif

#ifndef _GDK_GAMEOBJECT_H_
#define _GDK_GAMEOBJECT_H_

////////////////////////////////////////////////////////////////////////////////
// an object in a scene

namespace GDK
{
    struct IGameObject : public IRefCounted
    {
        // Identification
        GDK_IMETHOD_(uint32_t) GetID() const = 0;

        // Transform

        // Properties
        GDK_IMETHOD_(uint16_t) GetNumProperties() const = 0;
        GDK_IMETHOD GetProperty(_In_ uint16_t index, _Out_ void** placeHolder) = 0;
    };
} // GDK

#endif // _GDK_GAMEOBJECT_H_
