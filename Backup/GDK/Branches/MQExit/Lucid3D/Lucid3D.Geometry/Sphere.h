#pragma once

#include <Lucid3D.Math.h>

namespace Lucid3D
{
    namespace Geometry
    {
        class AABB;

        class Sphere
        {
        public:
            // Construction
            Sphere();
            Sphere(_In_ const Sphere& other);
            Sphere(_In_ const Vector3& center, _In_ float radius);

            static Sphere FromAABB(_In_ const AABB& aabb);

            // Assignment
            Sphere& operator= (_In_ const Sphere& other);

            // Properties
            const Vector3& GetCenter() const;
            float GetRadius() const;

            Sphere& Set(_In_ const Vector3& center, _In_ float radius);
            Sphere& SetCenter(_In_ const Vector3& center);
            Sphere& SetRadius(_In_ float radius);

            // Intersection
            bool Intersects(_In_ const Sphere& other) const;
            bool Intersects(_In_ const AABB& other) const;

        private:
            Vector3 _center;
            float _radius;
        };
    }
}
