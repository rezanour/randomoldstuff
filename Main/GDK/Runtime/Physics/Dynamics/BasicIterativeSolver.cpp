#include <Transform.h>
#include <Collision.h>
#include <CollisionPrimitives.h>
#include "..\BasicIterativeSolver.h"
#include "..\RigidBody.h"

namespace GDK
{
    std::shared_ptr<BasicIterativeSolver> BasicIterativeSolver::Create()
    {
        return std::shared_ptr<BasicIterativeSolver>(GDKNEW BasicIterativeSolver());
    }

    BasicIterativeSolver::BasicIterativeSolver()
    {
    }

    void BasicIterativeSolver::Solve(_In_ std::vector<ContactData>& contacts, _In_ float deltaTime)
    {
        // solve velocities
        for (auto i = 0; i < contacts.size() * 2; ++i)
        {
            float mostNegativeSeparatingVelocity = 0;
            ContactData* contactToSolve = nullptr;

            // each iteration, solve the most negative seperating velocity (largest closing velocity)
            auto contact = contacts.data();
            auto count = contacts.size();
            while (count-- > 0)
            {
                if (contact->GetSeparatingVelocity() < mostNegativeSeparatingVelocity)
                {
                    contactToSolve = contact;
                    mostNegativeSeparatingVelocity = contact->GetSeparatingVelocity();
                }
                ++contact;
            }

            if (mostNegativeSeparatingVelocity >= 0)
            {
                break; // done with velocity resolution
            }

            SolveVelocity(contactToSolve, deltaTime);
        }

        // solve positions
        for (auto i = 0; i < contacts.size() * 2; ++i)
        {
            float deepest = -1;
            ContactData* contactToSolve = nullptr;

            // each iteration, solve the deepest, and apply any corrective measure on the depth from the previous iteration
            auto contact = contacts.data();
            auto count = contacts.size();
            while (count-- > 0)
            {
                auto objA = contact->bodies[0];
                auto objB = contact->bodies[1] ? contact->bodies[1] : nullptr;

                contact->depth -= Vector3::Dot(objA->GetPositionAdjustments(), contact->normal);
                if (objB)
                {
                    contact->depth += Vector3::Dot(objB->GetPositionAdjustments(), contact->normal);
                }

                if (contact->depth > deepest)
                {
                    deepest = contact->depth;
                    contactToSolve = contact;
                }

                ++contact;
            }

            if (deepest <= 0)
            {
                break; // done with position resolution
            }

            contact = contacts.data();
            count = contacts.size();
            while (count-- > 0)
            {
                auto objA = contact->bodies[0];
                auto objB = contact->bodies[1] ? contact->bodies[1] : nullptr;

                objA->ClearPositionAdjustments();
                if (objB)
                {
                    objB->ClearPositionAdjustments();
                }
                ++contact;
            }

            SolvePosition(contactToSolve, deltaTime);
        }
    }

    void BasicIterativeSolver::SolveVelocity(_In_ ContactData* contact, _In_ float deltaSeconds)
    {
        UNREFERENCED_PARAMETER(deltaSeconds);

        float separatingVelocity = contact->GetSeparatingVelocity();
        if (separatingVelocity > 0)
        {
            // no impulse required
            return;
        }

        auto objA = contact->bodies[0];
        auto objB = contact->bodies[1] ? contact->bodies[1] : nullptr;

        float restitution = objA->GetRestitution();
        if (objB)
        {
            restitution = (restitution + objB->GetRestitution()) * 0.5f;
        }

        // special hack for standing on solid ground
        if (objB == nullptr && Vector3::Dot(contact->normal, Vector3::Up()) > 0.707f)
        {
            // cancel all velocity that's not upward
            auto vel = objA->GetVelocity();
            vel.x = vel.z = 0.0f;
            if (vel.y < 0) vel.y = 0.0f;
            objA->AdjustVelocity(-objA->GetVelocity() + vel);
            return;
        }

        float newSeparatingVelocity = -separatingVelocity * restitution; 

        // try to detect resting contacts and special case them
        Vector3 velocityFromAcceleration = objA->GetAcceleration();
        if (objB)
        {
            velocityFromAcceleration -= objB->GetAcceleration();
        }
        float seperatingVelocityFromAcceleration = Vector3::Dot(velocityFromAcceleration, contact->normal * deltaSeconds);

        if (seperatingVelocityFromAcceleration < 0)
        {
            newSeparatingVelocity += seperatingVelocityFromAcceleration * restitution; // restitution hard coded again

            if (newSeparatingVelocity < 0) newSeparatingVelocity = 0;
        }

        float deltaVelocity = newSeparatingVelocity - separatingVelocity;

        float totalInvMass = objA->GetInvMass();
        if (objB)
        {
            totalInvMass += objB->GetInvMass();
        }

        if (totalInvMass <= 0)
        {
            // can't move masslessness :)
            return;
        }

        float impulse = deltaVelocity / totalInvMass;

        Vector3 impulsePerInvMass = contact->normal * impulse;

        objA->AdjustVelocity(impulsePerInvMass * objA->GetInvMass());
        if (objB)
        {
            objB->AdjustVelocity(impulsePerInvMass * -objB->GetInvMass());
        }
    }

    void BasicIterativeSolver::SolvePosition(_In_ ContactData* contact, _In_ float deltaSeconds)
    {
        UNREFERENCED_PARAMETER(deltaSeconds);

        if (contact->depth <= 0)
        {
            // no penetration to solve
            return;
        }

        auto objA = contact->bodies[0];
        auto objB = contact->bodies[1] ? contact->bodies[1] : nullptr;

        float totalInvMass = objA->GetInvMass();
        if (objB)
        {
            totalInvMass += objB->GetInvMass();
        }

        if (totalInvMass <= 0)
        {
            // can't move masslessness :)
            return;
        }

        Vector3 movePerInvMass = contact->normal * (contact->depth / totalInvMass);

        objA->AdjustPosition(movePerInvMass * objA->GetInvMass());
        if (objB)
        {
            objB->AdjustPosition(movePerInvMass * -objB->GetInvMass());
        }
    }
}
