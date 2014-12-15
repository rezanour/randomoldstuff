#pragma once

#include <Lucid3D.Math.h>

namespace Lucid3D
{
    namespace Geometry
    {
        class Ray
        {
        public:
            // Construction
            Ray(_In_ const Vector3& start, _In_ const Vector3& direction);

            // Assignment
            const Ray& operator=(_In_ const Ray& other);

            // Properties
            const Vector3& GetStart() const;
            const Vector3& GetDirection() const;

        private:
            Vector3 _start, _direction;
        };
    }
}

