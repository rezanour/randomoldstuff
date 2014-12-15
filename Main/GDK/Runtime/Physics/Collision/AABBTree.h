#pragma once

#include <Platform.h>
#include <RuntimeObject.h>
#include <SpacePartition.h>
#include <GDKMath.h>

namespace GDK
{
    // Optimized, integer based Aabb tree.
    //
    // Note: since each triangle is exactly one leaf,
    // the number of nodes in the tree is always 2N-1 nodes.
    // We can allocate all the nodes in a contiguous block, and 
    // use indices as the children. Also, all the triangles
    // are then put in another parallel array, and the 
    // indices to the triangles are also unioned.
    struct AabbNode
    {
        Vector3 mins;
        Vector3 maxs;

        static const uint32_t LeafNode = 0xffffffff;

        // if left == LeafNode, then right is triangle index
        uint32_t left;
        uint32_t right;
    };

    // Note: All operations on the tree are in the 
    // local coordinate space of the tree, which never 
    // transforms. You must pretransform all inputs into
    // this space before calling any of the operations.
    class AabbTree : 
        public RuntimeObject<AabbTree>,
        public ISpacePartition
    {
    public:
        static std::shared_ptr<AabbTree> Create(_Inout_ std::istream& input);
        static std::shared_ptr<AabbTree> Create(_In_ const std::vector<Triangle>& triangles);

        // ISpacePartition
        virtual SpacePartitionType GetType() const override;
        virtual void Save(_Inout_ std::ostream& output) const override;
        virtual void GetAabb(_Out_ Vector3* aabbMin, _Out_ Vector3* aabbMax) const override;
        virtual bool Raycast(_In_ const Ray& ray, _Out_opt_ RaycastResult* result) const override;
        virtual bool Intersects(_In_ const CollisionPrimitive* primitive, _In_ CollideWithTriangle collisionCallback) const override;

    private:
        AabbTree(_In_ uint32_t numTriangles, _Inout_ std::unique_ptr<Triangle[]>& triangles, _Inout_ std::unique_ptr<AabbNode[]>& nodes);

        bool RaycastNode(_In_ uint32_t index, _In_ const Ray& ray, _In_ const Vector3 slopes[], _Out_opt_ RaycastResult* result) const;
        bool IntersectsNode(_In_ uint32_t index, _In_ const Vector3& aabbMin, _In_ const Vector3& aabbMax, _In_ const CollisionPrimitive* primitive, _In_ CollideWithTriangle collisionCallback, _Out_ bool* continueTesting) const;

        uint32_t _numTriangles;
        std::unique_ptr<Triangle[]> _triangles;
        std::unique_ptr<AabbNode[]> _nodes;
    };
}
