#pragma once

#include <Platform.h>
#include <GDKMath.h>
#include "RigidBody.h"

namespace GDK
{
    struct ContactData
    {
        // ContactData takes in shared_ptr to the rigid bodies, instead of weak_ptr. This ensures you lock the objects before creating contact data, which in turn
        // should prevent either of the objects from being deleted while we still have active contacts for them
        //
        // N.B. It is possible for body B to be nullptr. This is the case where body A has collided with static geometry
        //
        ContactData(_In_ const std::shared_ptr<RigidBody>& bodyA, _In_opt_ const std::shared_ptr<RigidBody>& bodyB, _In_ const Vector3& normal, _In_ float depth) :
            normal(normal), depth(depth)
        {
            bodies[0] = bodyA;
            bodies[1] = bodyB;
        }

        float GetSeparatingVelocity() const
        {
            Vector3 relativeVelocity = bodies[0]->GetVelocity();
            if (bodies[1] != nullptr)
            {
                relativeVelocity -= bodies[1]->GetVelocity();
            }
            return Vector3::Dot(relativeVelocity, normal);
        }

        std::shared_ptr<RigidBody> bodies[2];
        Vector3 normal;
        float depth;
    };
}
