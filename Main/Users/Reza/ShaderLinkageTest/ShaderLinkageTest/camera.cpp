#include "precomp.h"
#include "camera.h"
#include "renderer.h"

using namespace DirectX;

//==============================================================================

Camera::Camera()
{
    XMStoreFloat4x4(&_view, XMMatrixLookAtLH(XMVectorSet(0, 0, -30.0f, 1.0f), XMVectorSet(0, 0, 5.0f, 1.0f), XMVectorSet(0, 1.0f, 0, 0.0f)));
    XMStoreFloat4x4(&_projection, XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), 16.0f / 9.0f, 0.1f, 1000.0f));
}

//==============================================================================

const XMFLOAT4X4& Camera::GetView() const
{
    return _view;
}

const XMFLOAT4X4& Camera::GetProjection() const
{
    return _projection;
}

//==============================================================================

