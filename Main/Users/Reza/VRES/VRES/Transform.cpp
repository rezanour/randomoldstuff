#include "Precomp.h"
#include "VRES.h"

using namespace VRES;

Transform::Transform() :
    _position(0, 0, 0), _orientation(0, 0, 0, 1),
    _scale(1, 1, 1), _worldDirty(true)
{
}

Transform::Transform(const Transform& other) :
    _position(other._position), _orientation(other._orientation),
    _scale(other._scale), _worldDirty(true) // cheaper than copying world in case caller modifies before read
{
}

Transform::Transform(const XMFLOAT3& position, const XMFLOAT4& orientation, const XMFLOAT3& scale) :
    _position(position), _orientation(orientation),
    _scale(scale), _worldDirty(true)
{
}
