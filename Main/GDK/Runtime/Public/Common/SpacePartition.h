#pragma once

#include <Platform.h>
#include <GDKMath.h>
#include <Collision.h>

namespace GDK
{
    struct Triangle;

    enum class SpacePartitionType
    {
        AabbTree,
        BihTree,
    };

    // passed into the partition's collide method. The specified function is called for each potential hit.
    // Depending on the result, the collision testing may or may not continue.
    typedef bool (*CollideWithTriangle)(_In_ const CollisionPrimitive* primitive, _In_ const Triangle& triangle, _Out_ bool* continueTesting);

    struct ISpacePartition
    {
        virtual SpacePartitionType GetType() const = 0;
        virtual void Save(_Inout_ std::ostream& output) const = 0;

        virtual void GetAabb(_Out_ Vector3* aabbMin, _Out_ Vector3* aabbMax) const = 0;

        // Raycast is only supported in the local space of the partition data structure.
        // Ray must be transformed prior to calling
        virtual bool Raycast(_In_ const Ray& ray, _Out_opt_ RaycastResult* result) const = 0;

        // Collides the specified primitive with the spatial partition, using the callback provided for narrow phase.
        // Note that primitive must be in the local space of the partition.
        virtual bool Intersects(_In_ const CollisionPrimitive* primitive, _In_ CollideWithTriangle collisionCallback) const = 0;
    };

    namespace SpacePartition
    {
        std::shared_ptr<ISpacePartition> Create(_Inout_ std::istream& input);
        std::shared_ptr<ISpacePartition> Create(_In_ SpacePartitionType type, _In_ const std::vector<Triangle>& triangles);
    }
}
