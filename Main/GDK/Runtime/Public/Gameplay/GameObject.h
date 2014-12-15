#pragma once

#include <Platform.h>
#include <VisualInfo.h>
#include <GDKMath.h>

namespace GDK
{
    class Transform;
    struct IGameWorld;
    struct IGameObjectController;
    struct CollisionPrimitive;
    struct IGameObject;

    enum class PhysicsBodyType
    {
        Normal = 0,
        Sensor,
        Kinematic
    };

    //
    // The gameplay provided gameobject factory method
    // returns one of these filled out with info
    // instructing the runtime on how to create the object.
    //
    struct GameObjectCreateParameters
    {
        GameObjectCreateParameters() :
            rotation(0.0f), physicsType(PhysicsBodyType::Normal), floating(false)
        {}

        std::wstring className;
        std::wstring targetName;
        Vector3 position;
        float rotation;
        std::shared_ptr<IGameObjectController> controller;

        std::vector<VisualInfo> visuals;

        PhysicsBodyType physicsType;
        std::shared_ptr<CollisionPrimitive> collisionPrimitive;
        bool floating;
    };

    struct GameObjectContact
    {
        std::shared_ptr<IGameObject> other;
        Vector3 normal;
    };

    //
    // IGameObject is the runtime representation of an object.
    // This is a sealed type in the engine, but is extended through
    // the use of the IGameObjectController interface.
    //
    struct IGameObject
    {
        virtual const std::wstring& GetClassName() const = 0;
        virtual const std::wstring& GetTargetName() const = 0;

        virtual const std::shared_ptr<IGameObjectController>& GetController() const = 0;
        virtual const std::shared_ptr<IGameWorld> GetGameWorld() const = 0;

        virtual const Transform& GetTransform() const = 0;
        virtual Transform& GetTransform() = 0;

        virtual void AddImpulse(_In_ const Vector3& value) = 0;
        virtual const std::vector<GameObjectContact>& GetContacts() const = 0;
    };

    //
    // The IGameObjectController interface is to be implemented by the game
    // DLL to customize the behaviors of game objects.
    //
    struct IGameObjectController
    {
        // gameplay provided type
        virtual uint32_t GetTypeID() const = 0;

        // called when the associated gameobject is created and bound
        // to this controller
        virtual void OnCreate(_In_ const std::weak_ptr<IGameObject>& gameObject) = 0;

        // called when the associated game object is being destroyed
        virtual void OnDestroy() = 0;

        // called once per frame to allow the controller to update any state associated with the object
        virtual void OnUpdate() = 0;

        virtual void AppendProperties(_Inout_ std::map<std::wstring, std::wstring>& properties) const = 0;

        // called when the associated gameobject is activated
        virtual void OnActivate() = 0;
    };

    typedef GameObjectCreateParameters (*GameObjectCreateMethod)(_In_ const std::shared_ptr<IGameWorld>& gameWorld, _In_ const std::map<std::wstring, std::wstring>& properties);

    namespace GameObject
    {
        void RegisterFactory(_In_ GameObjectCreateMethod createMethod);
    }
}
