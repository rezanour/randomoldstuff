#include "Precomp.h"

using namespace DirectX;

AxisAlignedBox DynamicCollider::Bounds() const
{
    return AxisAlignedBox();
}

_Use_decl_annotations_
bool DynamicCollider::Raycast(FXMVECTOR start, FXMVECTOR direction, float* distance) const
{
    UNREFERENCED_PARAMETER(start);
    UNREFERENCED_PARAMETER(direction);
    *distance = 0.0f;
    return false;
}

_Use_decl_annotations_
bool DynamicCollider::Collide(const IDynamicCollider* object, FXMVECTOR movement, XMFLOAT3* contactPosition, XMFLOAT3* contactNormal, float* distance) const
{
    UNREFERENCED_PARAMETER(object);
    UNREFERENCED_PARAMETER(movement);
    *contactPosition = XMFLOAT3(0, 0, 0);
    *contactNormal = XMFLOAT3(0, 0, 0);
    *distance = 0.0f;
    return false;
}

std::shared_ptr<IDynamicCollider> CreateDynamicCollider()
{
    return std::shared_ptr<IDynamicCollider>(new DynamicCollider);
}

