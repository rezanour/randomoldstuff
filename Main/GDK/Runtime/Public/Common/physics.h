#pragma once

#include <Platform.h>
#include <Collision2.h>

#ifdef __cplusplus
extern "C" {
#endif

//
// Rigid Body
//

// represents an object in the dynamics system
struct rigidbody_h { uint32_t reserved; };

// create and destroy
rigidbody_h RigidBodyCreate(void* context);
void RigidBodyDestroy(rigidbody_h handle);

// basic info
void* RigidBodyGetContext(rigidbody_h handle);

// collision info
void RigidBodySetCollisionProxy(rigidbody_h handle, collisionproxy_h proxyHandle);
collisionproxy_h RigidBodyGetCollisionProxy(rigidbody_h handle);

// accessing transform (if the body has a collision proxy, this is the same as modifying that)
void RigidBodySetPosition(rigidbody_h handle, const float value[3]);
void RigidBodyGetPosition(rigidbody_h handle, float value[3]);
void RigidBodySetRotation(rigidbody_h handle, const float value);
float RigidBodyGetRotation(rigidbody_h handle);

// access linear velocity
void RigidBodySetVelocity(rigidbody_h handle, const float value[3]);
void RigidBodyGetVelocity(rigidbody_h handle, float value[3]);


//
// Simulation System
//

// represents a collection of collision proxies that can interact 
// with one another
struct physicsscene_h { uint32_t reserved; };

// create and destroy
physicsscene_h PhysicsSceneCreate(collision_detect_type type);
void PhysicsSceneDestroy(physicsscene_h handle);

collisionscene_h PhysicsSceneGetCollisionScene(physicsscene_h handle);

// manage rigid bodies
void PhysicsSceneAddBody(physicsscene_h handle, rigidbody_h body);
void PhysicsSceneRemoveBody(physicsscene_h handle, rigidbody_h body);

void PhysicsSceneStep(physicsscene_h handle, float deltaTime);

#ifdef __cplusplus
}
#endif
