#include <CollisionPrimitives.h>
#include <SpacePartition.h>
#include <GDKError.h>
#include <Log.h>

namespace GDK
{
    //====================================================================================
    // Transforms
    //====================================================================================

    static void TransformPrimitive(_In_ const Sphere* input, _In_ const Matrix& transform, _Inout_ Sphere* output)
    {
        // Scaling a sphere doesn't really work, unless the scale is uniform.
        // Here, we assume uniform scale, and so just take the x scale into consideration
        output->center = Matrix::Transform(input->center, transform);
        output->radius = input->radius * transform.GetRow(0).Length();
    }

    static void TransformPrimitive(_In_ const AlignedCapsule* input, _In_ const Matrix& transform, _Inout_ AlignedCapsule* output)
    {
        // Scaling an aligned capsule is a little weird. We have to treat the x & z the same, so we just pick the x arbitrarily
        output->center = Matrix::Transform(input->center, transform);
        output->length = input->length * transform.GetRow(1).Length();
        output->radius = input->radius * transform.GetRow(0).Length();
    }

    static void TransformPrimitive(_In_ const Aabb* input, _In_ const Matrix& transform, _Inout_ Aabb* output)
    {
        // N.B. currenty doesn't support rotation or scale
        output->aabbMin = input->aabbMin + transform.GetTranslation();
        output->aabbMax = input->aabbMax + transform.GetTranslation();
    }

    static void TransformPrimitive(_In_ const Plane* input, _In_ const Matrix& transform, _Inout_ Plane* output)
    {
        // For a plane, we can find any point on the plane, and transform it as a position.
        // We then transform the normal as a normal. We can specify the new plane in point, normal form
        Vector3 p = input->normal * input->dist;
        output->normal = Vector3::Normalize(Matrix::TransformNormal(input->normal, transform));
        output->dist = Vector3::Dot(Matrix::Transform(p, transform), output->normal);
    }

    static void TransformPrimitive(_In_ const Ray* input, _In_ const Matrix& transform, _Inout_ Ray* output)
    {
        // For a ray, we just transform the position as a position, and the direction
        // as a normal/vector
        output->start = Matrix::Transform(input->start, transform);
        output->direction = Vector3::Normalize(Matrix::TransformNormal(input->direction, transform));
    }

    static void TransformPrimitive(_In_ const Triangle* input, _In_ const Matrix& transform, _Inout_ Triangle* output)
    {
        // For a triangle, we just transform each vertex
        output->a = Matrix::Transform(input->a, transform);
        output->b = Matrix::Transform(input->b, transform);
        output->c = Matrix::Transform(input->c, transform);
        output->normal = Vector3::Normalize(Vector3::Cross(input->b - input->a, input->c - input->a));
    }

    static void TransformPrimitive(_In_ const TriangleMesh* input, _In_ const Matrix& transform, _Inout_ TriangleMesh* output)
    {
        // Ensure the output mesh refers to the same set of triangles
        output->data = input->data;

        // We need to determine the new final matrix, and then store the inverse of the transform for later
        Matrix oldTransform;
        Matrix::Inverse(input->invTransform, &oldTransform);

        Matrix newTransform = oldTransform * transform;
        Matrix::Inverse(newTransform, &output->invTransform);
    }

    template <typename T>
    static void CopyPrimitive(_In_ const T* input, _Inout_ T* output)
    {
        *output = *input;
    }

    void TransformPrimitive(_In_ const CollisionPrimitive* input, _In_opt_ const Matrix* transform, _Inout_ CollisionPrimitive* output)
    {
        CHECK_TRUE(input->type == output->type);

        if (transform)
        {
            switch (input->type)
            {
            case CollisionPrimitiveType::Sphere:
                TransformPrimitive(static_cast<const Sphere*>(input), *transform, static_cast<Sphere*>(output));
                break;
            case CollisionPrimitiveType::AlignedCapsule:
                TransformPrimitive(static_cast<const AlignedCapsule*>(input), *transform, static_cast<AlignedCapsule*>(output));
                break;
            case CollisionPrimitiveType::Aabb:
                TransformPrimitive(static_cast<const Aabb*>(input), *transform, static_cast<Aabb*>(output));
                break;
            case CollisionPrimitiveType::Plane:
                TransformPrimitive(static_cast<const Plane*>(input), *transform, static_cast<Plane*>(output));
                break;
            case CollisionPrimitiveType::Ray:
                TransformPrimitive(static_cast<const Ray*>(input), *transform, static_cast<Ray*>(output));
                break;
            case CollisionPrimitiveType::Triangle:
                TransformPrimitive(static_cast<const Triangle*>(input), *transform, static_cast<Triangle*>(output));
                break;
            case CollisionPrimitiveType::TriangleMesh:
                TransformPrimitive(static_cast<const TriangleMesh*>(input), *transform, static_cast<TriangleMesh*>(output));
                break;
            default:
                throw std::invalid_argument("type");
            }
        }
        else
        {
            switch (input->type)
            {
            case CollisionPrimitiveType::Sphere:
                CopyPrimitive(static_cast<const Sphere*>(input), static_cast<Sphere*>(output));
                break;
            case CollisionPrimitiveType::AlignedCapsule:
                CopyPrimitive(static_cast<const AlignedCapsule*>(input), static_cast<AlignedCapsule*>(output));
                break;
            case CollisionPrimitiveType::Aabb:
                CopyPrimitive(static_cast<const Aabb*>(input), static_cast<Aabb*>(output));
                break;
            case CollisionPrimitiveType::Plane:
                CopyPrimitive(static_cast<const Plane*>(input), static_cast<Plane*>(output));
                break;
            case CollisionPrimitiveType::Ray:
                CopyPrimitive(static_cast<const Ray*>(input), static_cast<Ray*>(output));
                break;
            case CollisionPrimitiveType::Triangle:
                CopyPrimitive(static_cast<const Triangle*>(input), static_cast<Triangle*>(output));
                break;
            case CollisionPrimitiveType::TriangleMesh:
                CopyPrimitive(static_cast<const TriangleMesh*>(input), static_cast<TriangleMesh*>(output));
                break;
            default:
                throw std::invalid_argument("type");
            }
        }
    }

    std::shared_ptr<CollisionPrimitive> CopyPrimitive(_In_ const CollisionPrimitive* input)
    {
        switch (input->type)
        {
        case CollisionPrimitiveType::Sphere:
            return CollisionPrimitive::Create(*static_cast<const Sphere*>(input));
        case CollisionPrimitiveType::AlignedCapsule:
            return CollisionPrimitive::Create(*static_cast<const AlignedCapsule*>(input));
        case CollisionPrimitiveType::Aabb:
            return CollisionPrimitive::Create(*static_cast<const Aabb*>(input));
        case CollisionPrimitiveType::Plane:
            return CollisionPrimitive::Create(*static_cast<const Plane*>(input));
        case CollisionPrimitiveType::Ray:
            return CollisionPrimitive::Create(*static_cast<const Ray*>(input));
        case CollisionPrimitiveType::Triangle:
            return CollisionPrimitive::Create(*static_cast<const Triangle*>(input));
        case CollisionPrimitiveType::TriangleMesh:
            return CollisionPrimitive::Create(*static_cast<const TriangleMesh*>(input));
        default:
            throw std::invalid_argument("type");
        }
    }

    //====================================================================================
    // Aabbs (Axis Aligned Bounding Box)
    //====================================================================================

    static void GetAabbForPrimitive(_In_ const Sphere* input, _Out_ Vector3* aabbMin, _Out_ Vector3* aabbMax)
    {
        Vector3 radius(input->radius);
        *aabbMin = input->center - radius;
        *aabbMax = input->center + radius;
    }

    static void GetAabbForPrimitive(_In_ const AlignedCapsule* input, _Out_ Vector3* aabbMin, _Out_ Vector3* aabbMax)
    {
        Vector3 radius(input->radius, 0.5f * input->length + input->radius, input->radius);
        *aabbMin = input->center - radius;
        *aabbMax = input->center + radius;
    }

    static void GetAabbForPrimitive(_In_ const Aabb* input, _Out_ Vector3* aabbMin, _Out_ Vector3* aabbMax)
    {
        *aabbMin = input->aabbMin;
        *aabbMax = input->aabbMax;
    }

    static void GetAabbForPrimitive(_In_ const Plane*, _Out_ Vector3*, _Out_ Vector3*)
    {
        throw std::invalid_argument("Planes don't have AABBs");
    }

    static void GetAabbForPrimitive(_In_ const Ray*, _Out_ Vector3*, _Out_ Vector3*)
    {
        throw std::invalid_argument("Rays don't have AABBs");
    }

    static void GetAabbForPrimitive(_In_ const Triangle* input, _Out_ Vector3* aabbMin, _Out_ Vector3* aabbMax)
    {
        *aabbMin = Vector3::Min(Vector3::Min(input->a, input->b), input->c);
        *aabbMax = Vector3::Max(Vector3::Max(input->a, input->b), input->c);
    }

    static void GetAabbForPrimitive(_In_ const TriangleMesh* input, _Out_ Vector3* aabbMin, _Out_ Vector3* aabbMax)
    {
        input->data->GetAabb(aabbMin, aabbMax);

        Matrix transform;
        Matrix::Inverse(input->invTransform, &transform);

        *aabbMin = Matrix::Transform(*aabbMin, transform);
        *aabbMax = Matrix::Transform(*aabbMax, transform);
    }

    void GetAabbForPrimitive(_In_ const CollisionPrimitive* input, _Out_ Vector3* aabbMin, _Out_ Vector3* aabbMax)
    {
        switch (input->type)
        {
        case CollisionPrimitiveType::Sphere:
            GetAabbForPrimitive(static_cast<const Sphere*>(input), aabbMin, aabbMax);
            break;
        case CollisionPrimitiveType::AlignedCapsule:
            GetAabbForPrimitive(static_cast<const AlignedCapsule*>(input), aabbMin, aabbMax);
            break;
        case CollisionPrimitiveType::Aabb:
            GetAabbForPrimitive(static_cast<const Aabb*>(input), aabbMin, aabbMax);
            break;
        case CollisionPrimitiveType::Plane:
            GetAabbForPrimitive(static_cast<const Plane*>(input), aabbMin, aabbMax);
            break;
        case CollisionPrimitiveType::Ray:
            GetAabbForPrimitive(static_cast<const Ray*>(input), aabbMin, aabbMax);
            break;
        case CollisionPrimitiveType::Triangle:
            GetAabbForPrimitive(static_cast<const Triangle*>(input), aabbMin, aabbMax);
            break;
        case CollisionPrimitiveType::TriangleMesh:
            GetAabbForPrimitive(static_cast<const TriangleMesh*>(input), aabbMin, aabbMax);
            break;
        default:
            throw std::invalid_argument("type");
        }
    }

    CollisionPrimitive::CollisionPrimitive(_In_ CollisionPrimitiveType type) :
        type(type)
    {
    }

    CollisionPrimitive::~CollisionPrimitive()
    {
    }

    Sphere::Sphere() :
        CollisionPrimitive(CollisionPrimitiveType::Sphere), radius(1)
    {
    }

    Sphere::Sphere(_In_ const Vector3& center, _In_ float radius) :
        CollisionPrimitive(CollisionPrimitiveType::Sphere), center(center), radius(radius)
    {
    }

    AlignedCapsule::AlignedCapsule() :
        CollisionPrimitive(CollisionPrimitiveType::AlignedCapsule), length(1), radius(1)
    {
    }

    AlignedCapsule::AlignedCapsule(_In_ const Vector3& center, _In_ float length, _In_ float radius) :
        CollisionPrimitive(CollisionPrimitiveType::AlignedCapsule), center(center), length(length), radius(radius)
    {
    }

    Aabb::Aabb() :
        CollisionPrimitive(CollisionPrimitiveType::Aabb)
    {
    }

    Aabb::Aabb(_In_ const Vector3& aabbMin, _In_ const Vector3& aabbMax) :
        CollisionPrimitive(CollisionPrimitiveType::Aabb), aabbMin(aabbMin), aabbMax(aabbMax)
    {
    }

    Plane::Plane() :
        CollisionPrimitive(CollisionPrimitiveType::Plane), normal(Vector3::Zero()), dist(0)
    {
    }

    Plane::Plane(_In_ const Vector3& normal, _In_ float dist) :
        CollisionPrimitive(CollisionPrimitiveType::Plane), normal(normal), dist(dist)
    {
    }

    Plane::Plane(_In_ const Vector3& a, _In_ const Vector3& b, _In_ const Vector3& c) :
        CollisionPrimitive(CollisionPrimitiveType::Plane), normal(Vector3::Normalize(Vector3::Cross(b - a, c - a))), dist(Vector3::Dot(normal, a))
    {
    }

    Plane::Plane(_In_ const Vector3& point, _In_ const Vector3& normal) :
        CollisionPrimitive(CollisionPrimitiveType::Plane), normal(normal), dist(Vector3::Dot(normal, point))
    {
    }

    Triangle::Triangle() :
        CollisionPrimitive(CollisionPrimitiveType::Triangle), a(Vector3::Zero()), b(Vector3::Zero()), c(Vector3::Zero()), normal(Vector3::Zero())
    {
    }

    Triangle::Triangle(_In_ const Vector3& a, _In_ const Vector3& b, _In_ const Vector3& c) :
        CollisionPrimitive(CollisionPrimitiveType::Triangle), a(a), b(b), c(c), normal(Vector3::Normalize(Vector3::Cross(b - a, c - a)))
    {
    }

    bool Triangle::IsDegenerate() const
    {
        return ((b - a).LengthSquared() < Math::Epsilon) || 
               ((c - a).LengthSquared() < Math::Epsilon) ||
               (Vector3::Cross(b - a, c - a).LengthSquared() < Math::Epsilon);
    }

    Ray::Ray() :
        CollisionPrimitive(CollisionPrimitiveType::Ray), start(Vector3::Zero()), direction(Vector3::Zero())
    {
    }

    Ray::Ray(_In_ const Vector3& start, _In_ const Vector3& direction) :
        CollisionPrimitive(CollisionPrimitiveType::Ray), start(start), direction(direction)
    {
    }

    TriangleMesh::TriangleMesh() :
        CollisionPrimitive(CollisionPrimitiveType::TriangleMesh)
    {
    }

    TriangleMesh::TriangleMesh(_In_ SpacePartitionType type, _In_ const std::vector<Triangle>& triangles) :
        CollisionPrimitive(CollisionPrimitiveType::TriangleMesh), data(SpacePartition::Create(type, triangles))
    {
    }

    TriangleMesh::TriangleMesh(_Inout_ std::istream& input) :
        CollisionPrimitive(CollisionPrimitiveType::TriangleMesh), data(SpacePartition::Create(input))
    {
    }
}
