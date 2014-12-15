#include "Precomp.h"
#include "SimpleCollider.h"
#include "Collider_p.h"

using namespace DirectX;

AxisAlignedBox StaticCollider::Bounds() const
{
    return AxisAlignedBox();
}

_Use_decl_annotations_
bool StaticCollider::Raycast(FXMVECTOR start, FXMVECTOR direction, float* distance) const
{
    UNREFERENCED_PARAMETER(start);
    UNREFERENCED_PARAMETER(direction);
    *distance = 0.0f;
    return false;
}

_Use_decl_annotations_
bool StaticCollider::Collide(const IDynamicCollider* object, FXMVECTOR movement, XMFLOAT3* contactPosition, XMFLOAT3* contactNormal, float* distance) const
{
    UNREFERENCED_PARAMETER(object);
    UNREFERENCED_PARAMETER(movement);
    *contactPosition = XMFLOAT3(0, 0, 0);
    *contactNormal = XMFLOAT3(0, 0, 0);
    *distance = 0.0f;
    return false;
}
