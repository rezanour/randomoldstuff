#pragma once

#include <Platform.h>
#include <GDKMath.h>

namespace GDK
{
    struct IGameObject;
    struct GameObjectContact;
    struct CollisionPrimitive;
    struct Triangle;

    enum class RigidBodyType
    {
        Normal = 0, // participates in standard collision detection and resolution phases
        Sensor,     // participates in collision detection, but not resolution. Objects pass through this object (but both parties are notified)
        Kinematic,  // participates in collision detection & resolution, but all resolution of the collision is applied to the other object
    };

    // Represents a dynamic object in the physics simulation
    struct IRigidBody
    {
        // The game object that this rigid body belongs to
        virtual std::shared_ptr<IGameObject> GetOwner() const = 0;

        // Basic dynamics properties
        virtual const Vector3& GetPosition() const = 0;
        virtual const Vector3& GetVelocity() const = 0;
        virtual float GetMass() const = 0;
        virtual float GetRestitution() const = 0;

        virtual void SetPosition(_In_ const Vector3& value) = 0;
        virtual void SetRotation(_In_ float value) = 0;

        // Adds an impulse for one frame
        virtual void AddImpulse(_In_ const Vector3& value) = 0;

        // Set a scale value to multiply with gravity. 0 = floating, 1 = normal.
        // This can be used for flying objects, and other special effects
        virtual void SetGravityScale(_In_ float value) = 0;

        virtual RigidBodyType GetType() const = 0;
        virtual bool IsSensor() const = 0;
        virtual bool IsKinematic() const = 0;

        virtual const std::vector<GameObjectContact>& GetContacts() const = 0;
    };

    // Parameters used to construct a new rigid body
    struct RigidBodyCreateParameters
    {
        RigidBodyCreateParameters(_In_ const Vector3& initialPosition, _In_ float mass, _In_opt_ const CollisionPrimitive* shape) :
            initialPosition(initialPosition), mass(mass), restitution(0.1f), gravityScale(1.0f), shape(shape), type(RigidBodyType::Normal)
        {
        }

        Vector3 initialPosition;
        Vector3 initialVelocity;
        float mass;
        float restitution;
        float gravityScale;
        const CollisionPrimitive* shape;
        RigidBodyType type;
    };

    // Represents the physics simulation world
    struct IPhysicsWorld
    {
        // PhysicsWorld owns the lifetime of the rigid bodies. It hands out weak_ptr to reference them with.
        virtual std::weak_ptr<IRigidBody> CreateBody(_In_ const std::shared_ptr<IGameObject>& owner, _In_ const RigidBodyCreateParameters& parameters) = 0;
        virtual void DestroyBody(_In_ const std::weak_ptr<IRigidBody>& body) = 0;

        // Step the simulation by the specified time interval
        virtual void Update(_In_ float elapsedSeconds) = 0;
    };

    namespace Physics
    {
        std::shared_ptr<IPhysicsWorld> CreateWorld(_In_ const Vector3& gravity);

        // Initialize and teardown the physics/collision infrastructure
        void Startup();
        void Shutdown();
    }
}
