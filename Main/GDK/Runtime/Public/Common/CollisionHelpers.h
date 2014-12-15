#pragma once

#include <Platform.h>

namespace GDK
{
    struct CollisionPrimitive;
    struct Triangle;
    class Matrix;
    class GeometryContent;

    namespace Collision
    {
        std::shared_ptr<CollisionPrimitive> SphereFromGeometry(_In_ const std::shared_ptr<GeometryContent>& content, _In_ uint32_t frame);
        std::shared_ptr<CollisionPrimitive> AlignedCapsuleFromGeometry(_In_ const std::shared_ptr<GeometryContent>& content, _In_ uint32_t frame, _In_ float extraRadiusPadding);
        std::shared_ptr<CollisionPrimitive> AlignedCapsuleFromTriangleList(_In_ const std::vector<Triangle>& triangles, _In_ float extraRadiusPadding);
        std::shared_ptr<CollisionPrimitive> TriangleMeshFromGeometry(_In_ const std::shared_ptr<GeometryContent>& content, _In_ uint32_t frame);
        void TriangleListFromGeometry(_In_ const std::shared_ptr<GeometryContent>& content, _In_ const Matrix& transform, _In_ uint32_t frame, _Inout_ std::vector<Triangle>& triangles);
    }
}
