#include "StdAfx.h"
#include "Sphere.h"
#include "AABB.h"

using namespace Lucid3D;
using Lucid3D::Geometry::AABB;
using Lucid3D::Geometry::Sphere;

// Construction
Sphere::Sphere()
    : _center(0, 0, 0), _radius(1)
{
}

Sphere::Sphere(_In_ const Sphere& other)
    : _center(other._center), _radius(other._radius)
{
}

Sphere::Sphere(_In_ const Vector3& center, _In_ float radius)
    : _center(center), _radius(radius)
{
}

Sphere Sphere::FromAABB(_In_ const AABB& other)
{
    // radius needs to reach corners
    Vector3 center = (other.GetMin() + other.GetMax()) * 0.5f;
    Vector3 halfWidth = other.GetMax() - center;
    float radius = halfWidth.Length();
    return Sphere(center, radius);
}

// Assignment
const Sphere& Sphere::operator= (_In_ const Sphere& other)
{
    if (&other == this)
        return *this;

    _center = other._center;
    _radius = other._radius;
    return *this;
}

// Properties
const Lucid3D::Vector3& Sphere::GetCenter() const
{
    return _center;
}

float Sphere::GetRadius() const
{
    return _radius;
}

Sphere& Sphere::Set(_In_ const Vector3& center, _In_ float radius)
{
    _center = center;
    _radius = radius;
    return *this;
}

Sphere& Sphere::SetCenter(_In_ const Vector3& center)
{
    _center = center;
    return *this;
}

Sphere& Sphere::SetRadius(_In_ float radius)
{
    _radius = radius;
    return *this;
}

// Intersection
bool Sphere::Intersects(_In_ const Sphere& other) const
{
    Vector3 dist = _center - other._center;
    float radius = _radius + other._radius;
    return (dist.LengthSquared() < radius * radius);
}

bool Sphere::Intersects(_In_ const AABB& other) const
{
    return other.Intersects(*this);
}
