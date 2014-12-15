// An object component is an individual 'aspect' of a game object. For instance, it's visual or physical qualities, it's animation support, etc...
#pragma once

#include "..\Platform.h"
#include "..\RefCounted.h"
#include "GameObject.h"

namespace GDK
{
    enum ObjectComponentType
    {
        // Provided by rendering engine, used to assign visual aspects to a game object
        VisualComponent = 0,
        // Provided by rendering engine, used to assign camera aspects to a game object
        CameraComponent,
        // Provided by rendering engine, used to assign light aspects to a game object
        LightComponent,
        // Provided by physics engine, used to assign dynamic aspects to a game object
        PhysicsComponent,
        // Provided by physics engine, used to assign collision detection aspects to a game object
        CollisionComponent,
    };

    struct IObjectComponent : public IRefCounted
    {
        GDK_METHOD_(const IGameObject*) GetOwner() const = 0;
        GDK_METHOD_(ObjectComponentType) GetType() const = 0;
    };

    struct IVisualComponent : public IObjectComponent
    {
    };

    struct ICameraComponent : public IObjectComponent
    {
    };

    struct ILightComponent : public IObjectComponent
    {
    };

    struct IPhysicsComponent : public IObjectComponent
    {
    };

    struct ICollisionComponent : public IObjectComponent
    {
    };
}
