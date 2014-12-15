#include <Transform.h>

namespace GDK
{
    Transform::Transform() :
        _rotation(0.0f), _worldDirty(true)
    {
    }

    const Vector3& Transform::GetPosition() const
    {
        return _position;
    }

    void Transform::SetPosition(_In_ const Vector3& value)
    {
        _position = value;
        _worldDirty = true;
    }

    float Transform::GetRotation() const
    {
        return _rotation;
    }

    void Transform::SetRotation(_In_ float value)
    {
        _rotation = value;
        _worldDirty = true;
    }

    const Matrix& Transform::GetWorld() const
    {
        if (_worldDirty)
        {
            _cachedWorld = Matrix::CreateRotationY(_rotation);
            _cachedWorld.SetTranslation(_position);
            _worldDirty = false;
        }

        return _cachedWorld;
    }
}
