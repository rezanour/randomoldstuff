#include "Precomp.h"
#include "Transform.h"

Transform::Transform() :
    _position(0, 0, 0), _orientation(0, 0, 0, 1), _scale(1, 1, 1), _worldIsDirty(true)
{
}

_Use_decl_annotations_
Transform::Transform(const Transform& other) :
    _position(other._position), _orientation(other._orientation), _scale(other._scale), _world(other._world), _worldIsDirty(other._worldIsDirty)
{
}

_Use_decl_annotations_
Transform::Transform(Transform&& other) :
    _position(other._position), _orientation(other._orientation), _scale(other._scale), _world(other._world), _worldIsDirty(other._worldIsDirty)
{
}

_Use_decl_annotations_
Transform::Transform(const XMFLOAT3& position, const XMFLOAT4& orientation, const XMFLOAT3& scale) :
    _position(position), _orientation(orientation), _scale(scale), _worldIsDirty(true)
{
}

_Use_decl_annotations_
Transform::Transform(const XMVECTOR& position, const XMVECTOR& orientation, const XMVECTOR& scale) :
    _worldIsDirty(true)
{
    XMStoreFloat3(&_position, position);
    XMStoreFloat4(&_orientation, orientation);
    XMStoreFloat3(&_scale, scale);
}

_Use_decl_annotations_
Transform& Transform::operator= (const Transform& other)
{
    _position = other._position;
    _orientation = other._orientation;
    _scale = other._scale;
    _world = other._world;
    _worldIsDirty = other._worldIsDirty;
    return *this;
}

_Use_decl_annotations_
Transform& Transform::operator= (Transform&& other)
{
    _position = other._position;
    _orientation = other._orientation;
    _scale = other._scale;
    _world = other._world;
    _worldIsDirty = other._worldIsDirty;
    return *this;
}

const XMFLOAT3& Transform::GetPosition() const
{
    return _position;
}

const XMFLOAT4& Transform::GetOrientation() const
{
    return _orientation;
}

const XMFLOAT3& Transform::GetScale() const
{
    return _scale;
}

const XMFLOAT4X4& Transform::GetWorld() const
{
    if (_worldIsDirty)
    {
        XMStoreFloat4x4(&_world, XMMatrixAffineTransformation(XMLoadFloat3(&_scale), XMVectorZero(), XMLoadFloat4(&_orientation), XMLoadFloat3(&_position)));
        _worldIsDirty = false;
    }
    return _world;
}

XMVECTOR Transform::GetPositionV() const
{
    return XMLoadFloat3(&_position);
}

XMVECTOR Transform::GetOrientationV() const
{
    return XMLoadFloat4(&_orientation);
}

XMVECTOR Transform::GetScaleV() const
{
    return XMLoadFloat3(&_scale);
}

XMMATRIX Transform::GetWorldV() const
{
    return XMLoadFloat4x4(&GetWorld());
}

XMFLOAT3 Transform::GetRight() const
{
    auto world = GetWorld();
    return XMFLOAT3(world._11, world._12, world._13);
}

XMFLOAT3 Transform::GetUp() const
{
    auto world = GetWorld();
    return XMFLOAT3(world._21, world._22, world._23);
}

XMFLOAT3 Transform::GetForward() const
{
    auto world = GetWorld();
    return XMFLOAT3(world._31, world._32, world._33);
}

XMVECTOR Transform::GetRightV() const
{
    auto world = GetWorld();
    return XMVectorSet(world._11, world._12, world._13, 0);
}

XMVECTOR Transform::GetUpV() const
{
    auto world = GetWorld();
    return XMVectorSet(world._21, world._22, world._23, 0);
}

XMVECTOR Transform::GetForwardV() const
{
    auto world = GetWorld();
    return XMVectorSet(world._31, world._32, world._33, 0);
}

_Use_decl_annotations_
void Transform::SetPosition(const XMFLOAT3& value)
{
    _position = value;
    _worldIsDirty = true;
}

_Use_decl_annotations_
void Transform::SetPosition(const XMVECTOR& value)
{
    XMStoreFloat3(&_position, value);
    _worldIsDirty = true;
}

_Use_decl_annotations_
void Transform::SetOrientation(const XMFLOAT4& value)
{
    _orientation = value;
    _worldIsDirty = true;
}

_Use_decl_annotations_
void Transform::SetOrientation(const XMVECTOR& value)
{
    XMStoreFloat4(&_orientation, XMQuaternionNormalize(value));
    _worldIsDirty = true;
}

_Use_decl_annotations_
void Transform::SetScale(const XMFLOAT3& value)
{
    _scale = value;
    _worldIsDirty = true;
}

_Use_decl_annotations_
void Transform::SetScale(const XMVECTOR& value)
{
    XMStoreFloat3(&_scale, value);
    _worldIsDirty = true;
}

_Use_decl_annotations_
void Transform::Move(const XMFLOAT3& movement)
{
    Move(XMLoadFloat3(&movement));
}

_Use_decl_annotations_
void Transform::Move(const XMVECTOR& movement)
{
    SetPosition(XMVectorAdd(GetPositionV(), movement));
}

_Use_decl_annotations_
void Transform::Rotate(const XMFLOAT3& axis, float angle)
{
    return Rotate(XMLoadFloat3(&axis), angle);
}

_Use_decl_annotations_
void Transform::Rotate(const XMVECTOR& axis, float angle)
{
    XMVECTOR rot = XMQuaternionRotationAxis(axis, angle);
    SetOrientation(XMQuaternionMultiply(GetOrientationV(), rot));
}

_Use_decl_annotations_
void Transform::LookAt(const XMFLOAT3& target, const XMFLOAT3& up)
{
    XMVECTOR t = XMLoadFloat3(&target);
    XMVECTOR u = XMLoadFloat3(&up);
    LookAt(t, u);
}

_Use_decl_annotations_
void Transform::LookAt(const XMVECTOR& target, const XMVECTOR& up)
{
    XMVECTOR forward = XMVector3Normalize(XMVectorSubtract(target, GetPositionV()));
    XMVECTOR right = XMVector3Normalize(XMVector3Cross(up, forward));
    XMVECTOR u = XMVector3Normalize(XMVector3Cross(forward, right));
    XMMATRIX rotation(right, u, forward, XMVectorSet(0, 0, 0, 1));
    SetOrientation(XMQuaternionRotationMatrix(rotation));
}

_Use_decl_annotations_
void Transform::AlignUp(const XMFLOAT3& axis)
{
    return AlignUp(XMLoadFloat3(&axis));
}

_Use_decl_annotations_
void Transform::AlignUp(const XMVECTOR& axis)
{
    XMVECTOR up = XMVector3Normalize(axis);
    XMVECTOR forward = GetForwardV();
    XMVECTOR right = XMVector3Cross(up, forward);
    up = XMVector3Cross(forward, right);
    XMMATRIX rotation(right, up, forward, XMVectorSet(0, 0, 0, 1));
    SetOrientation(XMQuaternionRotationMatrix(rotation));
}
