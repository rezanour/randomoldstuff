#include <Collision.h>
#include <CollisionPrimitives.h>
#include <GameObject.h>
#include "..\PhysicsWorld.h"
#include "..\RigidBody.h"
#include "..\PhysicsPrivate.h"
#include "..\BasicIterativeSolver.h"
#include "..\BasicBroadphase.h"
#include <ObjectTable.h>
#include <SharedHandle.h>
#include <Log.h>

namespace GDK
{
    struct rigidbody_h { uint32_t reserved; };

    struct rigidbody_t
    {
        // Stuff
        float mass;
    };

    void InitializeRigidBody(rigidbody_t* body)
    {
        memset(body, 0, sizeof(rigidbody_t));
    }

    void CleanupRigidBody(rigidbody_t* body)
    {
        Log::WriteInfo(L"foo");
        body;
    }

    static ObjectTable<rigidbody_h, rigidbody_t, InitializeRigidBody, CleanupRigidBody> g_rigidbodies(100);

    void RigidBodyDestroy(rigidbody_h handle)
    {
        g_rigidbodies.Delete(handle);
    }

    namespace Physics
    {
        std::shared_ptr<IPhysicsWorld> CreateWorld(_In_ const Vector3& gravity)
        {
            return PhysicsWorld::Create(gravity);
        }
    }

    std::shared_ptr<PhysicsWorld> PhysicsWorld::Create(_In_ const Vector3& gravity)
    {
        return std::shared_ptr<PhysicsWorld>(GDKNEW PhysicsWorld(gravity));
    }

    PhysicsWorld::PhysicsWorld(_In_ const Vector3& gravity) :
        _gravity(gravity), _broadphase(BasicBroadphase::Create()), _solver(BasicIterativeSolver::Create())
    {
        {
            SharedHandle<rigidbody_h, RigidBodyDestroy> rb(g_rigidbodies.New());
            if (g_rigidbodies.IsValidHandle(rb.Get()))
            {
                auto rb2 = rb;
                {
                    auto rb3(rb);
                    rigidbody_t* body = g_rigidbodies.Get(rb.Get());

                    {
                        body->mass = 1.0f;
                        auto rb4(std::move(rb3));
                    }
                }
            }
        }
    }

    std::weak_ptr<IRigidBody> PhysicsWorld::CreateBody(_In_ const std::shared_ptr<IGameObject>& owner, _In_ const RigidBodyCreateParameters& parameters)
    {
        auto body = RigidBody::Create(shared_from_this(), owner, parameters);
        _bodies.push_back(body);
        return body;
    }

    void PhysicsWorld::DestroyBody(_In_ const std::weak_ptr<IRigidBody>& body)
    {
        auto b = body.lock();
        if (b == nullptr)
        {
            // already removed
            return;
        }

        for (auto i = _bodies.begin(); i != _bodies.end(); ++i)
        {
            if (i->get() == b.get())
            {
                _bodies.erase(i);
                return;
            }
        }
    }

    void PhysicsWorld::Update(_In_ float elapsedSeconds)
    {
        elapsedSeconds = 0.0166f;

        Integrate(elapsedSeconds);

        GenerateContacts();

        _solver->Solve(_contacts, elapsedSeconds);

        // make sure we don't hold refs via the contacts
        _contacts.clear();

        for (auto i = _bodies.begin(); i != _bodies.end(); ++i)
        {
            (*i)->EndResolution();
        }
    }

    const Vector3& PhysicsWorld::GetGravity() const
    {
        return _gravity;
    }

    const std::shared_ptr<IBroadphase>& PhysicsWorld::GetBroadphase() const
    {
        return _broadphase;
    }

    void PhysicsWorld::Integrate(_In_ float deltaSeconds)
    {
        for (auto i = _bodies.begin(); i != _bodies.end(); ++i)
        {
            (*i)->Integrate(deltaSeconds);
            (*i)->ClearContacts();
            (*i)->BeginResolution();
        }
    }

    void PhysicsWorld::GenerateContacts()
    {
        _broadphase->GetPotentialPairs(_pairs, _staticPairs);

        IntersectionResult result;

        // check dynamic pairs
        GameObjectContact contact;
        PotentialPair* pair = _pairs.data();
        auto count = _pairs.size();
        while (count-- > 0)
        {
            if (Intersects(pair->first->GetWorldShape().get(), nullptr, pair->second->GetWorldShape().get(), nullptr, &result))
            {
                if (!pair->first->IsSensor() && !pair->second->IsSensor())
                {
                    _contacts.push_back(ContactData(pair->first, pair->second, result.normal, result.depth));
                }

                // Process notifications
                contact.other = pair->second->GetOwner();
                contact.normal = result.normal;
                pair->first->OnContact(contact);

                contact.other = pair->first->GetOwner();
                contact.normal = -result.normal;
                pair->second->OnContact(contact);
            }

            ++pair;
        }

        // check static triangles
        PotentialStaticPair* staticPair = _staticPairs.data();
        count = _staticPairs.size();
        while (count-- > 0)
        {
            if (Intersects(staticPair->first->GetWorldShape().get(), nullptr, staticPair->second, nullptr, &result))
            {
                _contacts.push_back(ContactData(staticPair->first, nullptr, result.normal, result.depth));

                // Process notifications
                contact.other = nullptr;
                contact.normal = result.normal;
                staticPair->first->OnContact(contact);
            }

            ++staticPair;
        }

        // make sure we don't hold refs via the pairs
        _pairs.clear();
        _staticPairs.clear();
    }
}
