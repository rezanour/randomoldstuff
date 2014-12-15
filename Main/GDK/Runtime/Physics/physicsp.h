#pragma once

#include <physics.h>

struct rigidbody_t
{
    float position[3];
    float rotation;
    float velocity[3];
    float mass;
    void* context;
    physicsscene_h scene;
    collisionproxy_h proxy;
};

rigidbody_t* RigidBodyGet(rigidbody_h handle);

