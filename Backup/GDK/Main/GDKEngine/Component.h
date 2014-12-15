#if defined(_MSC_VER)
#pragma once
#endif

#ifndef _GDK_COMPONENT_H_
#define _GDK_COMPONENT_H_

namespace GDK
{
    enum ComponentType
    {
        ComponentType_None = 0,
        ComponentType_Visual,
        ComponentType_Camera,
        ComponentType_Light,
        ComponentType_Collision,
        ComponentType_Dynamics,
        ComponentType_MessageHandler,
        ComponentType_Input,
    };

    // Implemented by all components
    struct IComponent : IContent
    {
        GDK_IMETHOD_(IGameObject*) GetOwner() const = 0;
        GDK_IMETHOD_(ComponentType) GetType() const = 0;
    };
} // GDK

#endif // _GDK_COMPONENT_H_
