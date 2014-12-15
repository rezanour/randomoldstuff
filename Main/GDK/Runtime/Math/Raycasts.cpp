#include <Platform.h>
#include <GDKMath.h>
#include <CollisionPrimitives.h>

namespace GDK
{
    static inline Vector3 TransformCoord(_In_ const Vector4& p, _In_ const Matrix& m)
    {
        Vector4 X(p.x);
        Vector4 Y(p.y);
        Vector4 Z(p.z);
        Vector4 result = Z * m.GetRow(2) + m.GetRow(3);
        result = Y * m.GetRow(1) + result;
        result = X * m.GetRow(0) + result;

        return result.ToVector3() * (1.0f / result.w);
    }

    _Use_decl_annotations_
    bool ScreenRaycast(float screenX, float screenY, const Matrix& view, const Matrix& projection, Ray* ray)
    {
        if (!ray)
        {
            return false;
        }

        *ray = Ray();

        Vector4 offset(-1.0f, 1.0f, 0.0f, 0.0f);
        Matrix transform = view * projection;
        bool exists = Matrix::Inverse(transform, &transform);
        if (!exists)
        {
            return false;
        }

        screenX *= 2.0f;
        screenY *= -2.0f;
        Vector4 near(Vector4(screenX, screenY, 0.0f, 1.0f) + offset);
        Vector4 far(Vector4(screenX, screenY, 1.0f, 1.0f) + offset);

        ray->start = TransformCoord(near, transform);
        Vector3 end = TransformCoord(far, transform);
        ray->direction = Vector3::Normalize(end - ray->start);
        return true;
    }
}
