#pragma once

#include <Lucid3D.Math.h>

namespace Lucid3D
{
    namespace Geometry
    {
        class Ray;
        class Plane;
        class Sphere;
        class AABB;

        bool Raycast(_In_ const Ray& ray, _In_ const Plane& plane, _Out_opt_ float* distance);
        bool Raycast(_In_ const Ray& ray, _In_ const Sphere& sphere, _Out_opt_ float* distance);
        bool Raycast(_In_ const Ray& ray, _In_ const AABB& box, _Out_opt_ float* distance);
    }
}
