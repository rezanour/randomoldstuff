#pragma once

#include <PhysicsInternal.h>
#include <RuntimeObject.h>

namespace GDK
{
    class PhysicsWorld;
    struct IBroadphaseProxy;

    class RigidBody :
        public RuntimeObject<RigidBody>,
        public IRigidBody
    {
    public:
        static std::shared_ptr<RigidBody> Create(_In_ const std::shared_ptr<PhysicsWorld>& world, _In_ const std::shared_ptr<IGameObject>& owner, _In_ const RigidBodyCreateParameters& parameters);

        // IRigidBody
        virtual std::shared_ptr<IGameObject> GetOwner() const override;

        virtual const Vector3& GetPosition() const override;
        virtual const Vector3& GetVelocity() const override;
        virtual float GetMass() const override;
        virtual float GetRestitution() const override;
        virtual void SetPosition(_In_ const Vector3& value) override;
        virtual void SetRotation(_In_ float value) override;
        virtual void AddImpulse(_In_ const Vector3& value) override;
        virtual void SetGravityScale(_In_ float value) override;
        virtual const std::vector<GameObjectContact>& GetContacts() const override;

        virtual RigidBodyType GetType() const override;
        virtual bool IsSensor() const override;
        virtual bool IsKinematic() const override;

        // RigidBody
        float GetInvMass() const;
        const Vector3& GetAcceleration() const;

        void Integrate(_In_ float deltaSeconds);
        void BeginResolution();     // Called before/after contact resolution to allow
        void EndResolution();       //   body to suspend collision object updates and manage caches
        void AdjustVelocity(_In_ const Vector3& value);
        void AdjustPosition(_In_ const Vector3& value);
        const Vector3& GetPositionAdjustments() const;
        void ClearPositionAdjustments();

        void ClearContacts();
        void OnContact(_In_ const GameObjectContact& contact);

        const std::shared_ptr<CollisionPrimitive>& GetWorldShape() const;

        ~RigidBody();

    private:
        RigidBody(_In_ const std::shared_ptr<PhysicsWorld>& world, _In_ const std::shared_ptr<IGameObject>& owner, _In_ const RigidBodyCreateParameters& parameters);

        Vector3 _position;
        Vector3 _velocity;
        Vector3 _acceleration;
        Vector3 _impulses;
        float _rotation;
        float _invMass;
        float _restitution;
        float _gravityScale;

        // track adjustments to position during resolution stages,
        // so we can account for it in future iterations this frame.
        Vector3 _positionAdjustments;
        RigidBodyType _type;

        std::vector<GameObjectContact> _contacts;

        std::weak_ptr<PhysicsWorld> _world;
        std::weak_ptr<IGameObject> _owner;
        std::shared_ptr<CollisionPrimitive> _shape; // shape of the body, if any. Given in local coordinate frame of object
        std::shared_ptr<CollisionPrimitive> _worldShape; // transformed shape, given in the world coordinate frame
        std::weak_ptr<IBroadphaseProxy> _proxy;
    };
}
