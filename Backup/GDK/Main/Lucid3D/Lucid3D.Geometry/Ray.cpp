#include "StdAfx.h"
#include "Ray.h"

using namespace Lucid3D;
using Lucid3D::Geometry::Ray;

// Construction
Ray::Ray(_In_ const Vector3& start, _In_ const Vector3& direction)
    : _start(start), _direction(Vector3::Normalize(direction))
{
}

// Assignment
const Ray& Ray::operator=(_In_ const Ray& other)
{
    if (this == &other)
        return *this;

    _start = other._start;
    _direction = other._direction;
    return *this;
}

// Properties
const Vector3& Ray::GetStart() const
{
    return _start;
}

const Vector3& Ray::GetDirection() const
{
    return _direction;
}

