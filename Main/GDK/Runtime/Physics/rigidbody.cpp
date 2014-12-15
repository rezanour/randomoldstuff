#include "physicsp.h"
#include <ObjectTable.h>

using namespace GDK;

//
// Internal
//

void InitRigidBody(rigidbody_t* body)
{
    memset(body, 0, sizeof(rigidbody_t));
    body->proxy.reserved = 0xFFFFFFFF;
    body->scene.reserved = 0xFFFFFFFF;
}

void DeleteRigidBody(rigidbody_t* body)
{
    if (body->proxy.reserved != 0xFFFFFFFF)
    {
        CollisionProxyDestroy(body->proxy);
    }
}

static ObjectTable<rigidbody_h, rigidbody_t, InitRigidBody, DeleteRigidBody> g_rigidBodyTable(100);

rigidbody_t* RigidBodyGet(rigidbody_h handle)
{
    return g_rigidBodyTable.Get(handle);
}


//
// Public
//

rigidbody_h RigidBodyCreate(void* context)
{
    rigidbody_h handle = g_rigidBodyTable.New();
    if (g_rigidBodyTable.IsValidHandle(handle))
    {
        rigidbody_t* body = g_rigidBodyTable.Get(handle);
        body->context = context;
        body->mass = 100.0f;
    }
    return handle;
}

void RigidBodyDestroy(rigidbody_h handle)
{
    g_rigidBodyTable.Delete(handle);
}

void* RigidBodyGetContext(rigidbody_h handle)
{
    rigidbody_t* body = g_rigidBodyTable.Get(handle);
    return body->context;
}

void RigidBodySetCollisionProxy(rigidbody_h handle, collisionproxy_h proxyHandle)
{
    rigidbody_t* body = g_rigidBodyTable.Get(handle);
    body->proxy = proxyHandle;
    // stomp over context
    CollisionProxySetContext(body->proxy, reinterpret_cast<void*>(handle.reserved));
}

collisionproxy_h RigidBodyGetCollisionProxy(rigidbody_h handle)
{
    rigidbody_t* body = g_rigidBodyTable.Get(handle);
    return body->proxy;
}

void RigidBodySetPosition(rigidbody_h handle, const float value[3])
{
    rigidbody_t* body = g_rigidBodyTable.Get(handle);
    body->position[0] = value[0];
    body->position[1] = value[1];
    body->position[2] = value[2];
    if (body->proxy.reserved != 0xFFFFFFFF)
    {
        CollisionProxySetPosition(body->proxy, body->position);
    }
}

void RigidBodyGetPosition(rigidbody_h handle, float value[3])
{
    rigidbody_t* body = g_rigidBodyTable.Get(handle);
    if (body->proxy.reserved != 0xFFFFFFFF)
    {
        CollisionProxyGetPosition(body->proxy, body->position);
    }
    value[0] = body->position[0];
    value[1] = body->position[1];
    value[2] = body->position[2];
}

void RigidBodySetRotation(rigidbody_h handle, const float value)
{
    rigidbody_t* body = g_rigidBodyTable.Get(handle);
    body->rotation = value;
    if (body->proxy.reserved != 0xFFFFFFFF)
    {
        CollisionProxySetRotation(body->proxy, body->rotation);
    }
}

float RigidBodyGetRotation(rigidbody_h handle)
{
    rigidbody_t* body = g_rigidBodyTable.Get(handle);
    if (body->proxy.reserved != 0xFFFFFFFF)
    {
        body->rotation = CollisionProxyGetRotation(body->proxy);
    }
    return body->rotation;
}

void RigidBodySetVelocity(rigidbody_h handle, const float value[3])
{
    rigidbody_t* body = g_rigidBodyTable.Get(handle);
    body->velocity[0] = value[0];
    body->velocity[1] = value[1];
    body->velocity[2] = value[2];
    if (body->proxy.reserved != 0xFFFFFFFF)
    {
        CollisionProxySetVelocity(body->proxy, body->velocity);
    }
}

void RigidBodyGetVelocity(rigidbody_h handle, float value[3])
{
    rigidbody_t* body = g_rigidBodyTable.Get(handle);
    if (body->proxy.reserved != 0xFFFFFFFF)
    {
        CollisionProxyGetVelocity(body->proxy, body->velocity);
    }
    value[0] = body->velocity[0];
    value[1] = body->velocity[1];
    value[2] = body->velocity[2];
}
