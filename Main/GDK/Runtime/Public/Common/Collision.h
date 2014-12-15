#pragma once

#include "Platform.h"
#include "GDKMath.h"

namespace GDK
{
    struct CollisionPrimitive;

    // =================================================================================
    // Raycasting
    // =================================================================================

    struct RaycastResult
    {
        Vector3 point;
        float distance;
    };

    // The global Raycast entry point. Callers use this entry point, which will
    // locate an appropriate raycast implementation for the type of primitive
    // provided. If only a boolean response is needed, nullptr may be passed for
    // the result parameter.
    bool Raycast(_In_ const Ray* ray, _In_opt_ const Matrix* rayTransform, _In_ const CollisionPrimitive* prim, _In_opt_ const Matrix* primTransform, _Inout_opt_ RaycastResult* result);

    // =================================================================================
    // Intersection Testing
    // =================================================================================

    // The convention is that results are returned from the point of view of object A.
    // So the normal will be pointing away from B, towards A. If you move A by normal * depth, you'll resolve the intersection
    struct IntersectionResult
    {
        Vector3 normal;
        float depth;
    };

    // The global Intersects entry point. Callers use this entry point, which will
    // locate an appropriate raycast implementation for the type of primitive
    // provided. If only a boolean response is needed, nullptr may be passed for
    // the result parameter.
    bool Intersects(_In_ const CollisionPrimitive* primA, _In_opt_ const Matrix* transformA, _In_ const CollisionPrimitive* primB, _In_opt_ const Matrix* transformB, _Inout_opt_ IntersectionResult* result);
}
