#include "StdAfx.h"
#include "AABB.h"
#include "Sphere.h"

using namespace Lucid3D;
using Lucid3D::Geometry::AABB;
using Lucid3D::Geometry::Sphere;

// Construction
AABB::AABB()
    : _min(0, 0, 0), _max(0, 0, 0)
{
}

AABB::AABB(_In_ const AABB& other)
    : _min(other._min), _max(other._max)
{
}

AABB::AABB(_In_ const Vector3& min, _In_ const Vector3& max)
    : _min(min), _max(max)
{
}

AABB AABB::FromSphere(_In_ const Sphere& sphere)
{
    const Vector3& center = sphere.GetCenter();
    float radius = sphere.GetRadius();

    Vector3 diag(radius, radius, radius);
    return AABB(center - diag, center + diag);
}

// Assignment
AABB& AABB::operator= (_In_ const AABB& other)
{
    if (&other == this)
        return *this;

    _min = other._min;
    _max = other._max;
    return *this;
}

// Properties
const Lucid3D::Vector3& AABB::GetMin() const
{
    return _min;
}

const Lucid3D::Vector3& AABB::GetMax() const
{
    return _max;
}

AABB& AABB::Set(_In_ const Vector3& min, _In_ const Vector3& max)
{
    _min = min;
    _max = max;
    return *this;
}

AABB& AABB::SetMin(_In_ const Vector3& min)
{
    _min = min;
    return *this;
}

AABB& AABB::SetMax(_In_ const Vector3& max)
{
    _max = max;
    return *this;
}

// Intersection
bool AABB::Intersects(_In_ const AABB& other) const
{
    return (_max.x > other._min.x && other._max.x > _min.x &&
            _max.y > other._min.y && other._max.y > _min.y &&
            _max.z > other._min.z && other._max.z > _min.z);
}

bool AABB::Intersects(_In_ const Sphere& other) const
{
    // find the closest point on the aabb to the sphere's center
    Vector3 closestPoint;
    const Vector3& center = other.GetCenter();

    closestPoint.x = min(max(center.x, _min.x), _max.x);
    closestPoint.y = min(max(center.y, _min.y), _max.y);
    closestPoint.z = min(max(center.z, _min.z), _max.z);

    // if closest point is closer than radius from the center, then 
    // we have an intersection
    float distSquared = (center - closestPoint).LengthSquared();
    float radius = other.GetRadius();
    return (distSquared < radius * radius);
}
