#pragma once

//
// A miscellaneous collection of useful and/or optimized math functionality
//

// Take over these functions
#undef sign

namespace GDK
{
    class Vector3;
    class Matrix;
    struct Ray;

    namespace Math
    {
        const float Epsilon = 0.0001f;
        const float Pi = 3.14159f;
        const float TwoPi = Pi * 2.0f;
        const float PiOver2 = Pi / 2.0f;
        const float PiOver3 = Pi / 3.0f;
        const float PiOver4 = Pi / 4.0f;

        inline float ToRadians(_In_ float degrees)
        {
            return degrees * Pi / 180.0f;
        }

        inline float ToDegrees(_In_ float radians)
        {
            return radians * 180.0f / Pi;
        }
    };

    // returns -1 if f is negative, or 1 if it is positive
    template <typename T>
    inline T sign(T value)
    {
        return value < static_cast<T>(0) ? static_cast<T>(-1) : static_cast<T>(1);
    }

    bool ScreenRaycast(_In_ float screenX, _In_ float screenY, _In_ const Matrix& view, _In_ const Matrix& projection, _Out_ Ray* ray);

    float ScaleToDesiredProjectedSize(_In_ const Vector3& viewPosition, _In_ const Vector3& viewDirection, _In_ float fovRadians, _In_ float viewWidth, _In_ float radius, _In_ const Vector3& targetPosition, _In_ float desiredProjectedSize);
}

using GDK::sign;
