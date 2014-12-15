#pragma once

#include <Collision2.h>

struct collisionproxy_t
{
    float min[3];
    float max[3];
    float position[3];
    float velocity[3];
    float rotation;
    float radius; // sphere
    void* context;
    collisionscene_h scene;
};

collisionproxy_t* CollisionProxyGet(collisionproxy_h handle);

inline bool AABBOverlap(const float min1[3], const float max1[3], const float min2[3], const float max2[3])
{
    return (max1[0] > min2[0] && max1[1] > min2[1] && max1[2] > min2[2] &&
            max2[0] > min1[0] && max2[1] > min1[1] && max2[2] > min1[2]);
}

void AppendTOIContactData(collisionproxy_h A, collisionproxy_h B, ContactInfo* contacts, uint32_t spaceRemaining, uint32_t* numContacts);
