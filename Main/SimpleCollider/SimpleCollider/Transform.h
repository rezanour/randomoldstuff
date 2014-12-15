#pragma once

//
// Left handed transform
//
class Transform
{
public:
    Transform() :
        _position(0, 0, 0), _orientation(0, 0, 0, 1), _scale(1, 1, 1), _worldDirty(true)
    {}

    Transform(const DirectX::XMFLOAT3& position,
              const DirectX::XMFLOAT4& orientation,
              const DirectX::XMFLOAT3& scale) :
        _position(position), _orientation(orientation), _scale(scale), _worldDirty(true)
    {}

    //
    // Allow copy
    //
    Transform(const Transform& other) :
        _position(other._position), _orientation(other._orientation), _scale(other._scale), _worldDirty(true)
    {}

    Transform& operator= (const Transform& other)
    {
        if (&other != this)
        {
            _position = other._position;
            _orientation = other._orientation;
            _scale = other._scale;
            _worldDirty = true;
        }

        return *this;
    }

    //
    // Get properties
    //
    const DirectX::XMFLOAT3& Position() const
    {
        return _position;
    }

    const DirectX::XMFLOAT4& Orientation() const
    {
        return _orientation;
    }

    const DirectX::XMFLOAT3& Scale() const
    {
        return _scale;
    }

    const DirectX::XMFLOAT3& Right() const
    {
        World();
        return *(DirectX::XMFLOAT3*)_world.m[0];
    }

    const DirectX::XMFLOAT3& Up() const
    {
        World();
        return *(DirectX::XMFLOAT3*)_world.m[1];
    }

    const DirectX::XMFLOAT3& Forward() const
    {
        World();
        return *(DirectX::XMFLOAT3*)_world.m[2];
    }

    const DirectX::XMFLOAT4X4& World() const
    {
        if (_worldDirty)
        {
            DirectX::XMStoreFloat4x4(&_world,
                        DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&_orientation)) *
                        DirectX::XMMatrixScaling(_scale.x, _scale.y, _scale.z));
            *(DirectX::XMFLOAT3*)_world.m[3] = _position;
            _worldDirty = false;
        }

        return _world;
    }

    //
    // Settable properties
    //
    void SetPosition(const DirectX::XMFLOAT3& position)
    {
        _position = position;
        _worldDirty = true;
    }

    void SetOrientation(const DirectX::XMFLOAT4& orientation)
    {
        _orientation = orientation;
        _worldDirty = true;
    }

    void SetScale(const DirectX::XMFLOAT3& scale)
    {
        _scale = scale;
        _worldDirty = true;
    }

    //
    // Utility
    //
    void LookTo(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& up)
    {
        DirectX::XMMATRIX rotation(DirectX::XMMatrixIdentity());
        rotation.r[2] = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&direction));
        DirectX::XMVECTOR u = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&up));
        rotation.r[0] = DirectX::XMVector3Cross(rotation.r[2], u);
        rotation.r[1] = DirectX::XMVector3Cross(rotation.r[0], rotation.r[2]);
        DirectX::XMStoreFloat4(&_orientation, DirectX::XMQuaternionRotationMatrix(rotation));
        _worldDirty = true;
    }

private:
    mutable DirectX::XMFLOAT4X4 _world;
    DirectX::XMFLOAT4 _orientation;
    DirectX::XMFLOAT3 _position;
    DirectX::XMFLOAT3 _scale;
    mutable bool _worldDirty;
};
