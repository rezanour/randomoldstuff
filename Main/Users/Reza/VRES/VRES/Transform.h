#pragma once

namespace VRES
{
    class Transform
    {
    public:
        Transform();
        Transform(const Transform& other);
        Transform(const XMFLOAT3& position, const XMFLOAT4& orientation, const XMFLOAT3& scale);

        Transform& operator= (const Transform& other)
        {
            if (&other == this)
            {
                return *this;
            }

            _position = other._position;
            _orientation = other._orientation;
            _scale = other._scale;
            _worldDirty = true; // cheaper than copying world matrix, in case it gets modified before read
        }

        const XMFLOAT3& Position() const { return _position; }
        const XMFLOAT4& Orientation() const { return _orientation; }
        const XMFLOAT3& Scale() const { return _scale; }

        void SetPosition(const XMFLOAT3& value) { _position = value; _worldDirty = true; }
        void SetOrientation(const XMFLOAT4& value) { _orientation = value; _worldDirty = true; }
        void SetScale(const XMFLOAT3& value) { _scale = value; _worldDirty = true; }

        const XMFLOAT4X4& World() const
        {
            if (_worldDirty)
            {
                XMStoreFloat4x4(&_world, XMMatrixRotationQuaternion(XMLoadFloat4(&_orientation)) *
                                         XMMatrixScaling(_scale.x, _scale.y, _scale.z));
                *(XMFLOAT3*)_world.m[3] = _position;
                _worldDirty = false;
            }

            return _world;
        }

        const XMFLOAT3& Right() const
        {
            World(); // force refresh world if it's dirty
            return (XMFLOAT3&)_world.m[0];
        }

        const XMFLOAT3& Up() const
        {
            World(); // force refresh world if it's dirty
            return (XMFLOAT3&)_world.m[1];
        }

        const XMFLOAT3& Forward() const
        {
            World(); // force refresh world if it's dirty
            return (XMFLOAT3&)_world.m[2];
        }

        void LookTo(const XMFLOAT3& direction, const XMFLOAT3& up)
        {
            XMMATRIX rotation(XMMatrixIdentity());
            rotation.r[2] = XMVector3Normalize(XMLoadFloat3(&direction));
            XMVECTOR u = XMVector3Normalize(XMLoadFloat3(&up));
            rotation.r[0] = XMVector3Cross(u, rotation.r[2]);
            rotation.r[1] = XMVector3Cross(rotation.r[2], rotation.r[0]);
            XMStoreFloat4(&_orientation, XMQuaternionRotationMatrix(rotation));
            _worldDirty = true;
        }

    private:
        XMFLOAT4 _orientation;
        XMFLOAT3 _position;
        XMFLOAT3 _scale;

        mutable XMFLOAT4X4 _world;
        mutable bool _worldDirty;
    };
}
