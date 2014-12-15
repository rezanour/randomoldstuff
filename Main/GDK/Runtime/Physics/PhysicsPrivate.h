#pragma once

#include <Platform.h>
#include <GDKMath.h>
#include "ContactData.h"

namespace GDK
{
    typedef std::pair<std::shared_ptr<RigidBody>, std::shared_ptr<RigidBody>> PotentialPair;
    typedef std::pair<std::shared_ptr<RigidBody>, const Triangle*> PotentialStaticPair;

    struct IBroadphaseProxy
    {
        virtual void SetAabb(_In_ const Vector3& aabbMin, _In_ const Vector3& aabbMax) = 0;
    };

    struct IBroadphase
    {
        // Proxies are owned by the broadphase, weak_ptrs are handed out as references
        virtual std::weak_ptr<IBroadphaseProxy> CreateProxy(_In_ const std::shared_ptr<RigidBody>& owner, _In_ const Vector3& aabbMin, _In_ const Vector3& aabbMax) = 0;
        virtual void DestroyProxy(_In_ const std::weak_ptr<IBroadphaseProxy>& proxy) = 0;

        virtual void GetPotentialPairs(_Inout_ std::vector<PotentialPair>& potentialPairs, _Inout_ std::vector<PotentialStaticPair>& potentialStaticPairs) = 0;
    };

    struct IContactSolver
    {
        virtual void Solve(_In_ std::vector<ContactData>& contacts, _In_ float deltaTime) = 0;
    };
}
