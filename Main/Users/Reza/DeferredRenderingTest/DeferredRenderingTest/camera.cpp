#include "precomp.h"
#include "camera.h"

void Camera::SetView(CXMMATRIX view)
{
    XMStoreFloat4x4(&_view, view);
}

void Camera::SetProjection(CXMMATRIX projection)
{
    XMStoreFloat4x4(&_projection, projection);
}

const XMFLOAT4X4& Camera::GetView() const
{
    return _view;
}

const XMFLOAT4X4& Camera::GetProjection() const
{
    return _projection;
}

float Camera::GetNearPlane() const
{
    return (-_projection._43 / _projection._33);
}

float Camera::GetFarPlane() const
{
    return (_projection._33 * GetNearPlane()) / (_projection._33 - 1);
}
