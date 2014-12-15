#include "physicsp.h"
#include <ObjectTable.h>

using namespace GDK;

//
// Private declarations
//

static void SolveContact(const ContactInfo* contact);


//
// Internal
//

struct physicsscene_t
{
    collisionscene_h collisionscene;
    rigidbody_h* rigidbodies;
    uint32_t capacity;
    uint32_t count;
};

void InitPhysicsScene(physicsscene_t* scene)
{
    memset(scene, 0, sizeof(physicsscene_t));
}

void DeletePhysicsScene(physicsscene_t* scene)
{
    if (scene->rigidbodies != nullptr)
    {
        rigidbody_h* body = scene->rigidbodies;
        uint32_t count = scene->count;
        while (count-- > 0)
        {
            RigidBodyDestroy(*body);
            ++body;
        }
        delete [] scene->rigidbodies;
    }
    else
    {
        assert(scene->count == 0);
    }
}

static ObjectTable<physicsscene_h, physicsscene_t, InitPhysicsScene, DeletePhysicsScene> g_physicsSceneTable(5);

//
// Public
//

physicsscene_h PhysicsSceneCreate(collision_detect_type type)
{
    physicsscene_h handle = g_physicsSceneTable.New();
    if (g_physicsSceneTable.IsValidHandle(handle))
    {
        physicsscene_t* scene = g_physicsSceneTable.Get(handle);
        scene->collisionscene = CollisionSceneCreate(type);
        CollisionSceneSetSolver(scene->collisionscene, SolveContact);
    }
    return handle;
}

void PhysicsSceneDestroy(physicsscene_h handle)
{
    g_physicsSceneTable.Delete(handle);
}

collisionscene_h PhysicsSceneGetCollisionScene(physicsscene_h handle)
{
    physicsscene_t* scene = g_physicsSceneTable.Get(handle);
    return scene->collisionscene;
}

void PhysicsSceneAddBody(physicsscene_h handle, rigidbody_h bodyHandle)
{
    physicsscene_t* scene = g_physicsSceneTable.Get(handle);
    rigidbody_t* body = RigidBodyGet(bodyHandle);

    assert(body->scene.reserved == 0xFFFFFFFF);

    if (scene->count == scene->capacity)
    {
        scene->capacity += 100;
        rigidbody_h* newBodies = new rigidbody_h[scene->capacity];
        if (scene->rigidbodies != nullptr)
        {
            for (uint32_t i = 0; i < scene->count; ++i)
            {
                newBodies[i] = scene->rigidbodies[i];
            }
            delete [] scene->rigidbodies;
        }
        scene->rigidbodies = newBodies;
    }

    scene->rigidbodies[scene->count++] = bodyHandle;
    body->scene = handle;
}

void PhysicsSceneRemoveBody(physicsscene_h handle, rigidbody_h bodyHandle)
{
    assert(RigidBodyGet(proxyHandle)->scene.reserved == handle.reserved);

    physicsscene_t* scene = g_physicsSceneTable.Get(handle);
    rigidbody_h* p = scene->rigidbodies;
    uint32_t count = scene->count;
    while (count-- > 0)
    {
        if (p->reserved == bodyHandle.reserved)
        {
            RigidBodyDestroy(bodyHandle);
            for (uint32_t i = count; i < scene->count - 1; ++i)
            {
                scene->rigidbodies[i] = scene->rigidbodies[i + 1];
            }
            return;
        }
    }
}

void PhysicsSceneStep(physicsscene_h handle, float deltaTime)
{
    physicsscene_t* scene = g_physicsSceneTable.Get(handle);
    if (scene->collisionscene.reserved != 0xFFFFFFFF)
    {
        CollisionSceneStep(scene->collisionscene, deltaTime, 100);
    }
}

//
// Private implementation
//

void SolveContact(const ContactInfo* contact)
{
    rigidbody_h hA = { reinterpret_cast<uint32_t>(CollisionProxyGetContext(contact->A)) };
    rigidbody_h hB = { reinterpret_cast<uint32_t>(CollisionProxyGetContext(contact->B)) };
    rigidbody_t* A = RigidBodyGet(hA);
    rigidbody_t* B = RigidBodyGet(hB);

    UNREFERENCED_PARAMETER(hB);
    UNREFERENCED_PARAMETER(B);

    // normalize the normal
    float l = sqrtf(contact->normal[0]*contact->normal[0] + contact->normal[1]*contact->normal[1] + contact->normal[2]*contact->normal[2]);
    float invL = 1.0f / l;
    float n[3] = {
        contact->normal[0] * invL,
        contact->normal[1] * invL,
        contact->normal[2] * invL
    };

    if (contact->toi == 0)
    {
        // penetrating, resolve positions first

        //
        // TODO: solve this correctly using masses
        //
        float newPosA[3] = { 
            A->position[0] + n[0] * contact->depth,
            A->position[1] + n[1] * contact->depth,
            A->position[2] + n[2] * contact->depth
        };
        RigidBodySetPosition(hA, newPosA);
    }

    // solve the velocties

    //
    // TODO: Solve correctly using masses & restitution
    //
    float remainingVelA[3] = {
        A->velocity[0] * (1.0f - contact->toi),
        A->velocity[1] * (1.0f - contact->toi),
        A->velocity[2] * (1.0f - contact->toi)
    };

    float vn = remainingVelA[0]*n[0] + remainingVelA[1]*n[1] + remainingVelA[2]*n[2];
    float newVelA[3] = {
        remainingVelA[0] - n[0] * vn,
        remainingVelA[1] - n[1] * vn,
        remainingVelA[2] - n[2] * vn
    };

    RigidBodySetVelocity(hA, newVelA);
}
