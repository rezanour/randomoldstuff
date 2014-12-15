#include "collisionp.h"
#include <ObjectTable.h>

using namespace GDK;

//
// Private declarations
//

inline void ComputeAABB(collisionproxy_t* proxy)
{
    proxy->min[0] = proxy->position[0] - proxy->radius + proxy->velocity[0];
    proxy->min[1] = proxy->position[1] - proxy->radius + proxy->velocity[1];
    proxy->min[2] = proxy->position[2] - proxy->radius + proxy->velocity[2];
    proxy->max[0] = proxy->position[0] + proxy->radius + proxy->velocity[0];
    proxy->max[1] = proxy->position[1] + proxy->radius + proxy->velocity[1];
    proxy->max[2] = proxy->position[2] + proxy->radius + proxy->velocity[2];
}

static bool TestForContactTOI(const float cA[3], const float rA, const float vA[3], const float cB[3], const float rB, const float vB[3], ContactInfo* contact);

//
// Internal
//

void InitCollisionProxy(collisionproxy_t* proxy)
{
    memset(proxy, 0, sizeof(collisionproxy_t));
    proxy->scene.reserved = 0xFFFFFFFF;
}

void DeleteCollisionProxy(collisionproxy_t* proxy)
{
    UNREFERENCED_PARAMETER(proxy);
}

static ObjectTable<collisionproxy_h, collisionproxy_t, InitCollisionProxy, DeleteCollisionProxy> g_collisionProxyTable(100);

collisionproxy_t* CollisionProxyGet(collisionproxy_h handle)
{
    return g_collisionProxyTable.Get(handle);
}

void AppendTOIContactData(collisionproxy_h A, collisionproxy_h B, ContactInfo* contacts, uint32_t spaceRemaining, uint32_t* numContacts)
{
    collisionproxy_t* proxyA = g_collisionProxyTable.Get(A);
    collisionproxy_t* proxyB = g_collisionProxyTable.Get(B);
    if (spaceRemaining > 0 && TestForContactTOI(proxyA->position, proxyA->radius, proxyA->velocity, proxyB->position, proxyB->radius, proxyB->velocity, &contacts[*numContacts]))
    {
        contacts[*numContacts].A = A;
        contacts[*numContacts].B = B;
        (*numContacts)++;
    }
}


//
// Public
//

collisionproxy_h CollisionProxyCreate()
{
    return g_collisionProxyTable.New();
}

void CollisionProxyDestroy(collisionproxy_h handle)
{
    g_collisionProxyTable.Delete(handle);
}

// basic info
void CollisionProxySetContext(collisionproxy_h handle, void* context)
{
    collisionproxy_t* proxy = g_collisionProxyTable.Get(handle);
    proxy->context = context;
}

void* CollisionProxyGetContext(collisionproxy_h handle)
{
    collisionproxy_t* proxy = g_collisionProxyTable.Get(handle);
    return proxy->context;
}

// NOTE NOTE: currently, we only support spheres. I need to iron out what the shape story is (I think we need ref counted handles)
void CollisionProxySetRadius(collisionproxy_h handle, float radius)
{
    collisionproxy_t* proxy = g_collisionProxyTable.Get(handle);
    proxy->radius = radius;
    ComputeAABB(proxy);
}

float CollisionProxyGetRadius(collisionproxy_h handle)
{
    collisionproxy_t* proxy = g_collisionProxyTable.Get(handle);
    return proxy->radius;
}

// set up the transform
void CollisionProxySetPosition(collisionproxy_h handle, const float value[3])
{
    collisionproxy_t* proxy = g_collisionProxyTable.Get(handle);
    proxy->position[0] = value[0];
    proxy->position[1] = value[1];
    proxy->position[2] = value[2];
    ComputeAABB(proxy);
}

void CollisionProxyGetPosition(collisionproxy_h handle, float value[3])
{
    collisionproxy_t* proxy = g_collisionProxyTable.Get(handle);
    value[0] = proxy->position[0];
    value[1] = proxy->position[1];
    value[2] = proxy->position[2];
}

void CollisionProxySetRotation(collisionproxy_h handle, const float value)
{
    collisionproxy_t* proxy = g_collisionProxyTable.Get(handle);
    proxy->rotation = value;
}

float CollisionProxySetRotation(collisionproxy_h handle)
{
    collisionproxy_t* proxy = g_collisionProxyTable.Get(handle);
    return proxy->rotation;
}

// set up linear velocities
void CollisionProxySetVelocity(collisionproxy_h handle, const float value[3])
{
    collisionproxy_t* proxy = g_collisionProxyTable.Get(handle);
    proxy->velocity[0] = value[0];
    proxy->velocity[1] = value[1];
    proxy->velocity[2] = value[2];
    ComputeAABB(proxy);
}

void CollisionProxyGetVelocity(collisionproxy_h handle, float value[3])
{
    collisionproxy_t* proxy = g_collisionProxyTable.Get(handle);
    value[0] = proxy->velocity[0];
    value[1] = proxy->velocity[1];
    value[2] = proxy->velocity[2];
}


//
// Private implementation
//

bool TestForContactTOI(const float cA[3], const float rA, const float vA[3], const float cB[3], const float rB, const float vB[3], ContactInfo* contact)
{
    // relative velocity
    float v[3] = { vA[0] - vB[0], vA[1] - vB[1], vA[2] - vB[2] };

    // treat B as origin, and move A accordingly
    float c[3] = { cA[0] - cB[0], cA[1] - cB[1], cA[2] - cB[2] };

    // combine radii for CSO
    float r = rA + rB;

    // quadratic, with A = vv, B = 2vc, C = cc. 
    // interleave computations with early out checks (no solution)
    float A = v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
    if (A == 0)
    {
        return false;
    }

    float B = 2 * (v[0]*c[0] + v[1]*c[1] + v[2]*c[2]);
    float C = c[0]*c[0] + c[1]*c[1] + c[2]*c[2];
    float top = B*B - 4*A*C;
    if (top < 0)
    {
        return false;
    }

    // find solutions
    float numerator = sqrtf(top);
    float invDenom = 1.0f / (2 * A);
    float t1 = (-B + numerator) * invDenom;
    float t2 = (-B - numerator) * invDenom;

    // handle all cases
    if (t1 * t2 < 0)
    {
        // we started intersected. Find the normal & depth, t = 0
        contact->normal[0] = c[0];
        contact->normal[1] = c[1];
        contact->normal[2] = c[2];
        contact->depth = r - sqrt(C);
        contact->toi = 0;
        return true;
    }
    else if (t1 < 0 && t2 < 0)
    {
        // moving away and didn't hit
        return false;
    }
    else
    {
        float tMin = std::min(t1, t2);
        if (tMin > 1.0f)
        {
            return false;
        }

        contact->normal[0] = c[0] + v[0] * tMin;
        contact->normal[1] = c[1] + v[1] * tMin;
        contact->normal[2] = c[2] + v[2] * tMin;
        contact->toi = tMin;
        return true;
    }
}
