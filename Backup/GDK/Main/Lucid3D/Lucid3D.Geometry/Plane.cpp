#include "StdAfx.h"
#include "Plane.h"

using namespace Lucid3D;
using Lucid3D::Geometry::Plane;

// Construction
Plane::Plane(_In_ const Vector3& normal, _In_ float dist)
    : _normal(Vector3::Normalize(normal)), _dist(dist)
{
}

Plane::Plane(_In_ const Vector3& a, _In_ const Vector3& b, _In_ const Vector3& c)
{
    _normal = Vector3::Normalize(Vector3::Cross(b - a, c - a));
    _dist = Vector3::Dot(a, _normal);
}

Plane::Plane(_In_ const Vector3& pt, _In_ const Vector3& normal)
{
    _normal = Vector3::Normalize(normal);
    _dist = Vector3::Dot(pt, _normal);
}

// Assignment
const Plane& Plane::operator=(_In_ const Plane& other)
{
    if (this == &other)
        return *this;

    _normal = other._normal;
    _dist = other._dist;
    return *this;
}

// Properties
const Vector3& Plane::GetNormal() const
{
    return _normal;
}

const float Plane::GetDist() const
{
    return _dist;
}

