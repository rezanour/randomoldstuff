#include "collisionp.h"
#include <ObjectTable.h>

using namespace GDK;

//
// Private declarations
//

struct collisionscene_t;

static void ProcessCollisionsDiscrete(collisionscene_t* scene, float deltaTime, uint32_t maxIterations);
static void ProcessCollisionsContinuous(collisionscene_t* scene, float deltaTime, uint32_t maxIterations);

//
// Internal
//

struct collisionscene_t
{
    collision_detect_type type;
    collisionproxy_h* proxies;
    uint32_t capacity;
    uint32_t count;
    pfnSolveContact solver;
};

void InitCollisionScene(collisionscene_t* scene)
{
    memset(scene, 0, sizeof(collisionscene_t));
}

void DeleteCollisionScene(collisionscene_t* scene)
{
    if (scene->proxies != nullptr)
    {
        collisionproxy_h* proxy = scene->proxies;
        uint32_t count = scene->count;
        while (count-- > 0)
        {
            CollisionProxyDestroy(*proxy);
            ++proxy;
        }
        delete [] scene->proxies;
    }
    else
    {
        assert(scene->numProxies == 0);
    }
}

static ObjectTable<collisionscene_h, collisionscene_t, InitCollisionScene, DeleteCollisionScene> g_collisionSceneTable(5);


//
// Public
//

collisionscene_h CollisionSceneCreate(collision_detect_type type)
{
    collisionscene_h handle = g_collisionSceneTable.New();
    if (g_collisionSceneTable.IsValidHandle(handle))
    {
        collisionscene_t* scene = g_collisionSceneTable.Get(handle);
        scene->type = type;
    }
    return handle;
}

void CollisionSceneDestroy(collisionscene_h handle)
{
    g_collisionSceneTable.Delete(handle);
}

void CollisionSceneAddProxy(collisionscene_h handle, collisionproxy_h proxyHandle)
{
    collisionscene_t* scene = g_collisionSceneTable.Get(handle);
    collisionproxy_t* proxy = CollisionProxyGet(proxyHandle);

    assert(proxy->scene.reserved == 0xFFFFFFFF);

    if (scene->count == scene->capacity)
    {
        scene->capacity += 100;
        collisionproxy_h* newProxies = new collisionproxy_h[scene->capacity];
        if (scene->proxies != nullptr)
        {
            for (uint32_t i = 0; i < scene->count; ++i)
            {
                newProxies[i] = scene->proxies[i];
            }
            delete [] scene->proxies;
        }
        scene->proxies = newProxies;
    }

    scene->proxies[scene->count++] = proxyHandle;
    proxy->scene = handle;
}

void CollisionSceneRemoveProxy(collisionscene_h handle, collisionproxy_h proxyHandle)
{
    assert(CollisionProxyGet(proxyHandle)->scene.reserved == handle.reserved);

    collisionscene_t* scene = g_collisionSceneTable.Get(handle);
    collisionproxy_h* p = scene->proxies;
    uint32_t count = scene->count;
    while (count-- > 0)
    {
        if (p->reserved == proxyHandle.reserved)
        {
            CollisionProxyDestroy(proxyHandle);
            for (uint32_t i = count; i < scene->count - 1; ++i)
            {
                scene->proxies[i] = scene->proxies[i + 1];
            }
            return;
        }
    }
}

void CollisionSceneSetSolver(collisionscene_h handle, pfnSolveContact solver)
{
    collisionscene_t* scene = g_collisionSceneTable.Get(handle);
    scene->solver = solver;
}

void CollisionSceneStep(collisionscene_h handle, float deltaTime, uint32_t maxIterations)
{
    collisionscene_t* scene = g_collisionSceneTable.Get(handle);
    switch (scene->type)
    {
    case collision_detect_type::discrete:
        ProcessCollisionsDiscrete(scene, deltaTime, maxIterations);
        break;

    case collision_detect_type::continuous:
        ProcessCollisionsContinuous(scene, deltaTime, maxIterations);
        break;

    default:
        assert(false);
    }
}


//
// Private method implementations
//

void ProcessCollisionsDiscrete(collisionscene_t* scene, float deltaTime, uint32_t maxIterations)
{
    assert(false && "not implemented yet");
    UNREFERENCED_PARAMETER(scene);
    UNREFERENCED_PARAMETER(deltaTime);
    UNREFERENCED_PARAMETER(maxIterations);
}

void ProcessCollisionsContinuous(collisionscene_t* scene, float deltaTime, uint32_t maxIterations)
{
    float remainingTime = deltaTime;
    uint32_t iteration = 0;

    ContactInfo contacts[500];
    uint32_t count;
    float tMin;

    while (remainingTime > 0 && iteration++ < maxIterations)
    {
        //
        // Step 1: for all potentially overlapping pairs, compute the toi contact data
        //
        // TODO: put in some broad phase to reduce the number of tests here
        //
        count = 0;
        for (uint32_t i = 0; i < scene->count - 1; ++i)
        {
            collisionproxy_t* proxyA = CollisionProxyGet(scene->proxies[i]);
            for (uint32_t j = i + 1; j < scene->count; ++j)
            {
                collisionproxy_t* proxyB = CollisionProxyGet(scene->proxies[j]);
                if (AABBOverlap(proxyA->min, proxyA->max, proxyB->min, proxyB->max))
                {
                    AppendTOIContactData(scene->proxies[i], scene->proxies[j], contacts, _countof(contacts) - count, &count);
                }
            }
        }

        //
        // Step 2: find the smallest TOI, advance all objects to that time, and solve all contacts within a small epsilon of that.
        //
        if (count > 0)
        {
            tMin = contacts[0].toi;
            for (uint32_t i = 1; i < count; ++i)
            {
                if (contacts[i].toi < tMin) tMin = contacts[i].toi;
            }

            float delta = tMin * remainingTime;

            // advance all objects to t
            for (uint32_t i = 0; i < scene->count; ++i)
            {
                collisionproxy_t* proxyA = CollisionProxyGet(scene->proxies[i]);
                float newPosition[3] = { 
                    proxyA->position[0] + proxyA->velocity[0] * delta, 
                    proxyA->position[1] + proxyA->velocity[1] * delta, 
                    proxyA->position[2] + proxyA->velocity[2] * delta
                };
                CollisionProxySetPosition(scene->proxies[i], newPosition);
            }

            // solve all velocities that are within range
            for (uint32_t i = 0; i < count; ++i)
            {
                if (contacts[i].toi - tMin < 0.0001f)
                {
                    scene->solver(&contacts[i]);
                }
            }
        }
        else
        {
            // just advance everyone to the end of the frame
            for (uint32_t i = 0; i < scene->count; ++i)
            {
                collisionproxy_t* proxyA = CollisionProxyGet(scene->proxies[i]);
                float newPosition[3] = { 
                    proxyA->position[0] + proxyA->velocity[0] * remainingTime, 
                    proxyA->position[1] + proxyA->velocity[1] * remainingTime, 
                    proxyA->position[2] + proxyA->velocity[2] * remainingTime
                };
                CollisionProxySetPosition(scene->proxies[i], newPosition);
            }

            remainingTime = 0;
        }
    }
}
