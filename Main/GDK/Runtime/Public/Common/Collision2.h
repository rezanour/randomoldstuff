#pragma once

#include <Platform.h>

#ifdef __cplusplus
extern "C" {
#endif

//
// Collision Proxy
//

// represents an object in the collision system
struct collisionproxy_h { uint32_t reserved; };

// create and destroy
collisionproxy_h CollisionProxyCreate();
void CollisionProxyDestroy(collisionproxy_h handle);

// basic info
void CollisionProxySetContext(collisionproxy_h handle, void* context);
void* CollisionProxyGetContext(collisionproxy_h handle);

// NOTE NOTE: currently, we only support spheres. I need to iron out what the shape story is (I think we need ref counted handles)
void CollisionProxySetRadius(collisionproxy_h handle, float radius);
float CollisionProxyGetRadius(collisionproxy_h handle);

// access transform
void CollisionProxySetPosition(collisionproxy_h handle, const float value[3]);
void CollisionProxyGetPosition(collisionproxy_h handle, float value[3]);
void CollisionProxySetRotation(collisionproxy_h handle, const float value);
float CollisionProxyGetRotation(collisionproxy_h handle);

// access linear velocity
void CollisionProxySetVelocity(collisionproxy_h handle, const float value[3]);
void CollisionProxyGetVelocity(collisionproxy_h handle, float value[3]);


//
// Collision System
//

// represents a collection of collision proxies that can interact 
// with one another
struct collisionscene_h { uint32_t reserved; };


enum class collision_detect_type
{
    discrete = 0,
    continuous
};

// create and destroy
collisionscene_h CollisionSceneCreate(collision_detect_type type);
void CollisionSceneDestroy(collisionscene_h handle);

// manage collision proxies
void CollisionSceneAddProxy(collisionscene_h handle, collisionproxy_h proxy);
void CollisionSceneRemoveProxy(collisionscene_h handle, collisionproxy_h proxy);

// manage contacts

struct ContactInfo
{
    // contexts
    collisionproxy_h A, B;

    // data
    float normal[3];
    float toi;      // if continuous collision enabled, is between [0, 1]
    float depth;    // usually used for discrete, but can also be used when t = 0 in CCD, if objects start penetrating
};

typedef void (*pfnSolveContact)(const ContactInfo* contact);

void CollisionSceneSetSolver(collisionscene_h handle, pfnSolveContact solver);
void CollisionSceneStep(collisionscene_h handle, float deltaTime, uint32_t maxIterations);


#ifdef __cplusplus
}
#endif
