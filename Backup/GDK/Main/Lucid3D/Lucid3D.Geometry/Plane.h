#pragma once

#include <Lucid3D.Math.h>

namespace Lucid3D
{
    namespace Geometry
    {
        class Plane
        {
        public:
            // Construction
            Plane(_In_ const Vector3& normal, _In_ float dist);
            Plane(_In_ const Vector3& a, _In_ const Vector3& b, _In_ const Vector3& c);
            Plane(_In_ const Vector3& pt, _In_ const Vector3& normal);

            // Assignment
            const Plane& operator= (_In_ const Plane& other);

            // Properties
            const Vector3& GetNormal() const;
            const float GetDist() const;

        private:
            Vector3 _normal;
            float _dist;
        };
    }
}
