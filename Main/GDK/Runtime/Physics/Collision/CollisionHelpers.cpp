#include <CollisionHelpers.h>
#include <Collision.h>
#include <CollisionPrimitives.h>
#include <GeometryContent.h>
#include <SpacePartition.h>

namespace GDK
{
    namespace Collision
    {
        std::shared_ptr<CollisionPrimitive> SphereFromGeometry(_In_ const std::shared_ptr<GeometryContent>& content, _In_ uint32_t frame)
        {
            uint32_t numIndices = content->GetNumIndicesPerFrame();

            if (numIndices == 0)
                return nullptr;

            uint32_t stride = content->GetVertexStride();
            const uint32_t* indices = content->GetIndices() + numIndices * frame;
            const byte_t* positions = content->GetAttributeData(GeometryContent::AttributeType::Float3, GeometryContent::AttributeName::Position, 0);

            Vector3 minPosition = *reinterpret_cast<const Vector3*>(positions + stride * indices[0]);
            Vector3 maxPosition = *reinterpret_cast<const Vector3*>(positions + stride * indices[0]);
            Vector3 position;
            for (uint32_t i = 1; i < numIndices; ++i)
            {
                position = *reinterpret_cast<const Vector3*>(positions + stride * indices[i]);

                if (position.x < minPosition.x) minPosition.x = position.x;
                if (position.y < minPosition.y) minPosition.y = position.y;
                if (position.z < minPosition.z) minPosition.z = position.z;

                if (position.x > maxPosition.x) maxPosition.x = position.x;
                if (position.y > maxPosition.y) maxPosition.y = position.y;
                if (position.z > maxPosition.z) maxPosition.z = position.z;
            }
            Vector3 center = (minPosition + maxPosition) * 0.5f;

            float maxDist = 0.0f;
            for (uint32_t i = 0; i < numIndices; ++i)
            {
                float dist = (*reinterpret_cast<const Vector3*>(positions + stride * indices[i]) - center).Length();
                if (dist > maxDist)
                {
                    maxDist = dist;
                }
            }

            return CollisionPrimitive::Create(Sphere(center, maxDist));
        }

        std::shared_ptr<CollisionPrimitive> AlignedCapsuleFromGeometry(_In_ const std::shared_ptr<GeometryContent>& content, _In_ uint32_t frame, _In_ float extraRadiusPadding)
        {
            uint32_t numIndices = content->GetNumIndicesPerFrame();

            if (numIndices == 0)
                return nullptr;

            uint32_t stride = content->GetVertexStride();
            const uint32_t* indices = content->GetIndices() + numIndices * frame;
            const byte_t* positions = content->GetAttributeData(GeometryContent::AttributeType::Float3, GeometryContent::AttributeName::Position, 0);

            Vector3 minPosition = *reinterpret_cast<const Vector3*>(positions + stride * indices[0]);
            Vector3 maxPosition = *reinterpret_cast<const Vector3*>(positions + stride * indices[0]);
            Vector3 position;
            for (uint32_t i = 1; i < numIndices; ++i)
            {
                position = *reinterpret_cast<const Vector3*>(positions + stride * indices[i]);

                if (position.x < minPosition.x) minPosition.x = position.x;
                if (position.y < minPosition.y) minPosition.y = position.y;
                if (position.z < minPosition.z) minPosition.z = position.z;

                if (position.x > maxPosition.x) maxPosition.x = position.x;
                if (position.y > maxPosition.y) maxPosition.y = position.y;
                if (position.z > maxPosition.z) maxPosition.z = position.z;
            }
            Vector3 center = (minPosition + maxPosition) * 0.5f;

            float radius = std::max(maxPosition.x - center.x, maxPosition.z - center.z) + extraRadiusPadding;
            float height = maxPosition.y - minPosition.y;
            float length = (height - (2 * radius));
            if (length < 0)
            {
                length = 0;
            }

            return CollisionPrimitive::Create(AlignedCapsule(center, length, radius));
        }

        std::shared_ptr<CollisionPrimitive> AlignedCapsuleFromTriangleList(_In_ const std::vector<Triangle>& triangles, _In_ float extraRadiusPadding)
        {
            if (triangles.size() == 0)
            {
                return nullptr;
            }

            Vector3 minPosition = triangles[0].a;
            Vector3 maxPosition = triangles[0].a;

            for (uint32_t i = 0; i < triangles.size(); ++i)
            {
                minPosition = Vector3::Min(minPosition, Vector3::Min(Vector3::Min(triangles[i].a, triangles[i].b), triangles[i].c));
                maxPosition = Vector3::Max(maxPosition, Vector3::Max(Vector3::Max(triangles[i].a, triangles[i].b), triangles[i].c));
            }
            Vector3 center = (minPosition + maxPosition) * 0.5f;

            float radius = std::max(maxPosition.x - center.x, maxPosition.z - center.z) + extraRadiusPadding;
            float height = maxPosition.y - minPosition.y;
            float length = (height - (2 * radius));
            if (length < 0)
            {
                length = 0;
            }

            return CollisionPrimitive::Create(AlignedCapsule(center, length, radius));
        }

        std::shared_ptr<CollisionPrimitive> TriangleMeshFromGeometry(_In_ const std::shared_ptr<GeometryContent>& content, _In_ uint32_t frame)
        {
            uint32_t numIndices = content->GetNumIndicesPerFrame();

            if (numIndices == 0)
                return nullptr;

            uint32_t stride = content->GetVertexStride();
            const uint32_t* indices = content->GetIndices() + numIndices * frame;
            const byte_t* positions = content->GetAttributeData(GeometryContent::AttributeType::Float3, GeometryContent::AttributeName::Position, 0);

            Vector3 a, b, c;
            std::vector<Triangle> triangles;

            for (uint32_t i = 0; i < numIndices; i+=3)
            {
                a = *reinterpret_cast<const Vector3*>(positions + stride * indices[i]);
                b = *reinterpret_cast<const Vector3*>(positions + stride * indices[i+1]);
                c = *reinterpret_cast<const Vector3*>(positions + stride * indices[i+2]);
                triangles.push_back(Triangle(a, b, c));
            }

            return CollisionPrimitive::Create(TriangleMesh(SpacePartitionType::AabbTree, triangles));
        }

        void TriangleListFromGeometry(_In_ const std::shared_ptr<GeometryContent>& content, _In_ const Matrix& transform, _In_ uint32_t frame, _Inout_ std::vector<Triangle>& triangles)
        {
            uint32_t numIndices = content->GetNumIndicesPerFrame();

            if (numIndices == 0)
                return;

            uint32_t stride = content->GetVertexStride();
            const uint32_t* indices = content->GetIndices() + numIndices * frame;
            const byte_t* positions = content->GetAttributeData(GeometryContent::AttributeType::Float3, GeometryContent::AttributeName::Position, 0);

            Vector3 a, b, c;

            for (uint32_t i = 0; i < numIndices; i+=3)
            {
                a = *reinterpret_cast<const Vector3*>(positions + stride * indices[i]);
                b = *reinterpret_cast<const Vector3*>(positions + stride * indices[i+1]);
                c = *reinterpret_cast<const Vector3*>(positions + stride * indices[i+2]);
                triangles.push_back(Triangle(Matrix::Transform(a, transform), Matrix::Transform(b, transform), Matrix::Transform(c, transform)));
            }
        }

    }
}
