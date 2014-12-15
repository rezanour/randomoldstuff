#pragma once

#include <PhysicsInternal.h>
#include <RuntimeObject.h>
#include "PhysicsPrivate.h"

namespace GDK
{
    class RigidBody;
    struct ContactData;

    class PhysicsWorld : 
        public RuntimeObject<PhysicsWorld>,
        public IPhysicsWorld
    {
    public:
        static std::shared_ptr<PhysicsWorld> Create(_In_ const Vector3& gravity);

        // IPhysicsWorld
        virtual std::weak_ptr<IRigidBody> CreateBody(_In_ const std::shared_ptr<IGameObject>& owner, _In_ const RigidBodyCreateParameters& parameters) override;
        virtual void DestroyBody(_In_ const std::weak_ptr<IRigidBody>& body) override;
        virtual void Update(_In_ float elapsedSeconds) override;

        // PhysicsWorld
        const Vector3& GetGravity() const;
        const std::shared_ptr<IBroadphase>& GetBroadphase() const;

    private:
        PhysicsWorld(_In_ const Vector3& gravity);

        void Integrate(_In_ float deltaSeconds);
        void GenerateContacts();

        std::vector<std::shared_ptr<RigidBody>> _bodies;
        Vector3 _gravity;

        std::vector<PotentialPair> _pairs;
        std::vector<PotentialStaticPair> _staticPairs;
        std::shared_ptr<IBroadphase> _broadphase;

        std::vector<ContactData> _contacts;
        std::shared_ptr<IContactSolver> _solver;
    };
}
