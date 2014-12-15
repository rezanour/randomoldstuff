#pragma once

#include <Lucid3D.Math.h>

namespace Lucid3D
{
    namespace Geometry
    {
        class Sphere;

        class AABB
        {
        public:
            // Construction
            AABB();
            AABB(_In_ const AABB& other);
            AABB(_In_ const Vector3& min, _In_ const Vector3& max);

            static AABB FromSphere(_In_ const Sphere& sphere);

            // Assignment
            const AABB& operator= (_In_ const AABB& other);

            // Properties
            const Vector3& GetMin() const;
            const Vector3& GetMax() const;
            Vector3 GetCenter() const;
            Vector3 GetHalfWidths() const;

            AABB& Set(_In_ const Vector3& min, _In_ const Vector3& max);
            AABB& SetMin(_In_ const Vector3& min);
            AABB& SetMax(_In_ const Vector3& max);

            // Intersection
            bool Intersects(_In_ const AABB& other) const;
            bool Intersects(_In_ const Sphere& other) const;

        private:
            Vector3 _min, _max;
        };
    }
}

