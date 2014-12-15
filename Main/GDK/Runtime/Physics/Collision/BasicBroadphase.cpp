#include "..\BasicBroadphase.h"
#include <CollisionPrimitives.h>
#include <PhysicsInternal.h>
#include <SpacePartition.h>
#include <GDKError.h>

namespace GDK
{
    std::shared_ptr<BasicBroadphase> BasicBroadphase::Create()
    {
        return std::shared_ptr<BasicBroadphase>(GDKNEW BasicBroadphase);
    }

    BasicBroadphase::BasicBroadphase()
    {
    }

    std::weak_ptr<IBroadphaseProxy> BasicBroadphase::CreateProxy(_In_ const std::shared_ptr<RigidBody>& owner, _In_ const Vector3& aabbMin, _In_ const Vector3& aabbMax)
    {
        CHECK_NOT_NULL(owner);

        auto proxy = BasicBroadphaseProxy::Create(owner, aabbMin, aabbMax);

        if (owner->GetWorldShape()->type != CollisionPrimitiveType::TriangleMesh)
        {
            for (auto i = _proxies.begin(); i != _proxies.end(); ++i)
            {
                // if this new object is normal type, it pairs with everything
                // if it's sensor or kinematic, it pairs with all normals, but nothing else
                if (owner->GetType() != RigidBodyType::Normal &&
                    (*i)->GetBody()->GetType() != RigidBodyType::Normal)
                {
                    continue;
                }

                _pairs.push_back(std::make_pair(proxy, *i));
            }

            _proxies.push_back(proxy);

            if (owner->GetType() == RigidBodyType::Normal)
            {
                _dynamicProxies.push_back(proxy);
            }
        }
        else
        {
            _triangleMeshProxies.push_back(proxy);
        }

        return proxy;
    }

    void BasicBroadphase::DestroyProxy(_In_ const std::weak_ptr<IBroadphaseProxy>& proxy)
    {
        auto p = proxy.lock();
        if (p == nullptr)
        {
            // already removed
            return;
        }

        auto proxyObj = static_cast<BasicBroadphaseProxy*>(p.get());
        auto body = proxyObj->GetBody();

        if (body && body->GetWorldShape()->type != CollisionPrimitiveType::TriangleMesh)
        {
            // remove any pairs that contained this proxy
            for (int32_t i = static_cast<int32_t>(_pairs.size()) - 1; i >= 0; --i)
            {
                if (_pairs[i].first.get() == proxyObj ||
                    _pairs[i].second.get() == proxyObj)
                {
                    for (auto j = i; j < _pairs.size() - 1; ++j)
                    {
                        _pairs[j] = _pairs[j + 1];
                    }
                    _pairs.pop_back();
                }
            }

            // remove the proxy from the main list
            for (auto i = _proxies.begin(); i != _proxies.end(); ++i)
            {
                if (i->get() == proxyObj)
                {
                    _proxies.erase(i);
                    break;
                }
            }

            // if the object is normal type, remove the proxy from the dynamic list as well.
            // If the body has been deleted already, then we don't know if the type was normal or 
            // not, so walk the list and check by proxy pointer in case it was
            if (!body || proxyObj->GetBody()->GetType() == RigidBodyType::Normal)
            {
                for (auto i = _dynamicProxies.begin(); i != _dynamicProxies.end(); ++i)
                {
                    if (i->get() == proxyObj)
                    {
                        _dynamicProxies.erase(i);
                        break;
                    }
                }
            }
        }
        else
        {
            for (int32_t i = static_cast<int32_t>(_triangleMeshProxies.size()) - 1; i >= 0; --i)
            {
                if (_triangleMeshProxies[i].get() == proxyObj)
                {
                    for (auto j = i; j < _triangleMeshProxies.size() - 1; ++j)
                    {
                        _triangleMeshProxies[j] = _triangleMeshProxies[j + 1];
                    }
                    _triangleMeshProxies.pop_back();
                }
            }
        }
    }

    // TODO: investigate why a lambda didn't work
    static std::vector<const Triangle*> s_staticPairs;
    bool CollideTriangleMesh(_In_ const CollisionPrimitive*, _In_ const Triangle& triangle, _Out_ bool* continueTesting)
    {
        s_staticPairs.push_back(&triangle);
        *continueTesting = true;
        return true;
    }

    void BasicBroadphase::GetPotentialPairs(_Inout_ std::vector<PotentialPair>& potentialPairs, _Inout_ std::vector<PotentialStaticPair>& potentialStaticPairs)
    {
        UNREFERENCED_PARAMETER(potentialStaticPairs);

        for (auto i = _pairs.begin(); i != _pairs.end(); ++i)
        {
            potentialPairs.push_back(PotentialPair(i->first->GetBody(), i->second->GetBody()));
        }

        for (auto i = _dynamicProxies.begin(); i != _dynamicProxies.end(); ++i)
        {
            auto primitive = (*i)->GetBody()->GetWorldShape();
            // TODO: Get rid of this allocation!
            std::shared_ptr<CollisionPrimitive> primitiveCopy = CopyPrimitive(primitive.get());

            for (auto t = _triangleMeshProxies.begin(); t != _triangleMeshProxies.end(); ++t)
            {
                s_staticPairs.clear();

                const TriangleMesh* triMesh = static_cast<const TriangleMesh*>((*t)->GetBody()->GetWorldShape().get());

                // need to transform the object into the local space of the triangle mesh
                TransformPrimitive(primitive.get(), &triMesh->invTransform, primitiveCopy.get());

                triMesh->data->Intersects(primitiveCopy.get(), CollideTriangleMesh);

                for (uint32_t k = 0; k < s_staticPairs.size(); ++k)
                {
                    potentialStaticPairs.push_back(std::make_pair((*i)->GetBody(), s_staticPairs[k]));
                }
            }
        }
    }

    //====================================================
    // Collision Proxy
    //====================================================

    std::shared_ptr<BasicBroadphaseProxy> BasicBroadphaseProxy::Create(_In_ const std::shared_ptr<RigidBody>& body, _In_ const Vector3& aabbMin, _In_ const Vector3& aabbMax)
    {
        return std::shared_ptr<BasicBroadphaseProxy>(GDKNEW BasicBroadphaseProxy(body, aabbMin, aabbMax));
    }

    BasicBroadphaseProxy::BasicBroadphaseProxy(_In_ const std::shared_ptr<RigidBody>& body, _In_ const Vector3& aabbMin, _In_ const Vector3& aabbMax) :
        _body(body), _aabbMin(aabbMin), _aabbMax(aabbMax)
    {
    }

    std::shared_ptr<RigidBody> BasicBroadphaseProxy::GetBody() const
    {
        return _body.lock();
    }

    void BasicBroadphaseProxy::SetAabb(_In_ const Vector3& aabbMin, _In_ const Vector3& aabbMax)
    {
        _aabbMin = aabbMin;
        _aabbMax = aabbMax;
    }

    const Vector3& BasicBroadphaseProxy::AabbMin() const
    {
        return _aabbMin;
    }

    const Vector3& BasicBroadphaseProxy::AabbMax() const
    {
        return _aabbMax;
    }
}
