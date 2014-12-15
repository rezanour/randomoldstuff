#if defined(_MSC_VER)
#pragma once
#endif

#ifndef _GDK_TRANSFORM_H_
#define _GDK_TRANSFORM_H_

#define DIRECTXMATH_INTEROP
#include <Lucid3D\Lucid3D.Math.h>

namespace GDK
{
    struct Transform
    {
        // The translational position of the object
        Lucid3D::Vector3 Position;
        // The scale factor in each axis direction
        Lucid3D::Vector3 Scale;
        // The orientation of the object represented as a unit quaternion
        Lucid3D::Vector4 Orientation;
        // Cached matrix representation which is valid when you receive the transform object.
        // Any manipulation of the members of the struct invalidates the matrix
        Lucid3D::Matrix World;
    };
}

#endif // _GDK_TRANSFORM_H_

