#pragma once

#include <Platform.h>
#include <RuntimeObject.h>
#include "..\PhysicsPrivate.h"

namespace GDK
{
    class BasicBroadphaseProxy;
    struct ISpacePartition;

    class BasicBroadphase : 
        public RuntimeObject<BasicBroadphase>,
        public IBroadphase
    {
    public:
        static std::shared_ptr<BasicBroadphase> Create();

        // IBroadphase
        virtual std::weak_ptr<IBroadphaseProxy> CreateProxy(_In_ const std::shared_ptr<RigidBody>& owner, _In_ const Vector3& aabbMin, _In_ const Vector3& aabbMax) override;
        virtual void DestroyProxy(_In_ const std::weak_ptr<IBroadphaseProxy>& proxy) override;
        virtual void GetPotentialPairs(_Inout_ std::vector<PotentialPair>& potentialPairs, _Inout_ std::vector<PotentialStaticPair>& potentialStaticPairs) override;

    private:
        BasicBroadphase();

        // Collection of all proxies for tracking
        std::vector<std::shared_ptr<BasicBroadphaseProxy>> _proxies;

        // Collection of normal objects only, for faster enumeration
        std::vector<std::shared_ptr<BasicBroadphaseProxy>> _dynamicProxies;

        // Collection of triangle mesh objects only, for faster enumeration
        std::vector<std::shared_ptr<BasicBroadphaseProxy>> _triangleMeshProxies;

        // Since we don't do any object/object broadphase yet, we precompute all combinations and cache them to speed things up a bit
        typedef std::vector<std::pair<std::shared_ptr<BasicBroadphaseProxy>, std::shared_ptr<BasicBroadphaseProxy>>> PairVector;
        PairVector _pairs;
    };

    class BasicBroadphaseProxy : 
        public RuntimeObject<BasicBroadphaseProxy>,
        public IBroadphaseProxy
    {
    public:
        static std::shared_ptr<BasicBroadphaseProxy> Create(_In_ const std::shared_ptr<RigidBody>& body, _In_ const Vector3& aabbMin, _In_ const Vector3& aabbMax);

        // IBroadphaseProxy
        virtual void SetAabb(_In_ const Vector3& aabbMin, _In_ const Vector3& aabbMax) override;

        // BasicBroadphaseProxy
        std::shared_ptr<RigidBody> GetBody() const;
        const Vector3& AabbMin() const;
        const Vector3& AabbMax() const;

    private:
        BasicBroadphaseProxy(_In_ const std::shared_ptr<RigidBody>& owner, _In_ const Vector3& aabbMin, _In_ const Vector3& aabbMax);

        std::weak_ptr<RigidBody> _body;
        Vector3 _aabbMin;
        Vector3 _aabbMax;
    };
}
