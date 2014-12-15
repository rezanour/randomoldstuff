#pragma once

#include <Platform.h>
#include <MemoryObject.h>
#include <GDKMath.h>

namespace GDK
{
    enum class SpacePartitionType;
    struct ISpacePartition;

    // Supported collision shapes
    enum class CollisionPrimitiveType
    {
        None = 0,
        Sphere,
        AlignedCapsule, // an upright capsule that cannot rotate
        Aabb, // axis aligned bounding box
        Plane,
        Ray,
        Triangle,
        TriangleMesh,
    };

    //
    // NOTE:
    //
    // CollisionPrimitive's are designed to be as lightweight as possible,
    // and support on stack creation so they can be used as in-between computational aids.
    //
    // However, another common use is to keep a base class pointer to a heap allocated collision
    // primitive as a struct/class member, and so heap allocated use is also supported. To
    // help with memory tracking, we derive from MemoryObject for those scenarios.
    //
    struct CollisionPrimitive : MemoryObject
    {
        template <typename T>
        static std::shared_ptr<CollisionPrimitive> Create(_In_ const T& source)
        {
            return std::shared_ptr<CollisionPrimitive>(GDKNEW T(source));
        }

        template <typename T>
        static T* CreateArray(_In_ size_t size)
        {
            return GDKNEW T[size];
        }

        // Support for base pointer use (ensures derived instances destruct properly)
        virtual ~CollisionPrimitive();

        CollisionPrimitiveType type;

    protected:
        // CollisionPrimitive isn't useful on it's own, so force it to only be a base class
        CollisionPrimitive(_In_ CollisionPrimitiveType type);
    };

    // The out parameter must be an already existing primitive of matching type. It's data is overwritten with the transformed result
    // If the transform is null, this function will just duplicate the original primitive
    void TransformPrimitive(_In_ const CollisionPrimitive* input, _In_opt_ const Matrix* transform, _Inout_ CollisionPrimitive* output);

    std::shared_ptr<CollisionPrimitive> CopyPrimitive(_In_ const CollisionPrimitive* input);

    void GetAabbForPrimitive(_In_ const CollisionPrimitive* input, _Out_ Vector3* aabbMin, _Out_ Vector3* aabbMax);

    // =================================================================================
    // Specialized Collision Types
    // =================================================================================

    struct Sphere : CollisionPrimitive
    {
        Sphere();
        Sphere(_In_ const Vector3& center, _In_ float radius);

        Vector3 center;
        float radius;
    };

    struct AlignedCapsule : CollisionPrimitive
    {
        AlignedCapsule();
        AlignedCapsule(_In_ const Vector3& center, _In_ float length, _In_ float radius);

        Vector3 center;
        float length;
        float radius;
    };

    struct Aabb : CollisionPrimitive
    {
        Aabb();
        Aabb(_In_ const Vector3& aabbMin, _In_ const Vector3& aabbMax);

        Vector3 aabbMin;
        Vector3 aabbMax;
    };

    struct Plane : CollisionPrimitive
    {
        Plane();
        Plane(_In_ const Vector3& normal, _In_ float dist);
        Plane(_In_ const Vector3& a, _In_ const Vector3& b, _In_ const Vector3& c);
        Plane(_In_ const Vector3& point, _In_ const Vector3& normal);

        Vector3 normal;
        float dist;
    };

    struct Ray : CollisionPrimitive
    {
        Ray();
        Ray(_In_ const Vector3& start, _In_ const Vector3& direction);

        Vector3 start;
        Vector3 direction;
    };

    struct Triangle : CollisionPrimitive
    {
        Triangle();
        Triangle(_In_ const Vector3& a, _In_ const Vector3& b, _In_ const Vector3& c);

        bool IsDegenerate() const;

        Vector3 a;
        Vector3 b;
        Vector3 c;
        Vector3 normal;
    };

    struct TriangleMesh : CollisionPrimitive
    {
        TriangleMesh();
        TriangleMesh(_In_ SpacePartitionType type, _In_ const std::vector<Triangle>& triangles);
        TriangleMesh(_Inout_ std::istream& input);

        std::shared_ptr<ISpacePartition> data;

        // Because it is very costly to try and transform the data (we'd have to completely rebuild it),
        // we instead just track the inverse of the transform applied to this object, and use it to 
        // transform incoming raycasts/objects to the local space of the data structure.
        Matrix invTransform;
    };
}
