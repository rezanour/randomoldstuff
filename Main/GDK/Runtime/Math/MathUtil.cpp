#include <Platform.h>
#include <GDKMath.h>
#include <GeometryContent.h>

namespace GDK
{
    float ScaleToDesiredProjectedSize(_In_ const Vector3& viewPosition, _In_ const Vector3& viewDirection, _In_ float fovRadians, _In_ float viewWidth, _In_ float radius, _In_ const Vector3& targetPosition, _In_ float desiredProjectedSize)
    {
        float distToPlane = 0.5f * viewWidth / tanf(0.5f * fovRadians);
        float distToObject = Vector3::Dot(viewDirection, (targetPosition - viewPosition));
        float projectedHeight = (2 * radius * distToPlane) / distToObject;
        return desiredProjectedSize / projectedHeight;
    }
}
