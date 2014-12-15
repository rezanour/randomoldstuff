#pragma once

#include <Platform.h>
#include <GDKMath.h>

namespace GDK
{
    class Transform
    {
    public:
        Transform();

        const Vector3& GetPosition() const;
        void SetPosition(_In_ const Vector3& value);

        float GetRotation() const;
        void SetRotation(_In_ float value);

        const Matrix& GetWorld() const;

    private:
        Vector3 _position;
        float _rotation;
        mutable Matrix _cachedWorld;
        mutable bool _worldDirty;
    };
}
