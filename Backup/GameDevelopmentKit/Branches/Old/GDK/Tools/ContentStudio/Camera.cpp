#include "stdafx.h"

using namespace DirectX;

#undef min // use __min instead
#undef max // use __max instead

Camera::Camera()
{
    // Setup the view matrix
    SetViewParams( 
        XMFLOAT3(0.0f, 0.0f, 0.0f),   // default eye position.
        XMFLOAT3(0.0f, 0.0f, 1.0f),   // default look at position.
        XMFLOAT3(0.0f, 1.0f, 0.0f)    // default up vector.
        );

    // Setup the projection matrix
    SetProjParams(XM_PI / 4, 1.0f, 1.0f, 1000.0f);
}

void Camera::LookDirection (_In_ XMFLOAT3 lookDirection)
{
    XMFLOAT3 lookAt;
    lookAt.x = m_eye.x + lookDirection.x;
    lookAt.y = m_eye.y + lookDirection.y;
    lookAt.z = m_eye.z + lookDirection.z;

    SetViewParams(m_eye, lookAt, m_up);
}

void Camera::Eye(_In_ XMFLOAT3 eye)
{
    SetViewParams(eye, m_lookAt, m_up);
}

void Camera::SetViewParams( _In_ XMFLOAT3 eye, _In_ XMFLOAT3 lookAt,_In_ XMFLOAT3 up )
{
    m_eye = eye;
    m_lookAt = lookAt;
    m_up = up;

    XMMATRIX view;

    view = XMMatrixLookAtLH( XMLoadFloat3(&m_eye), XMLoadFloat3(&m_lookAt), XMLoadFloat3(&m_up));

    XMVECTOR det;
    XMMATRIX inverseView = XMMatrixInverse(&det, view);
    XMStoreFloat4x4(&m_viewMatrix, view);
    XMStoreFloat4x4(&m_inverseView, inverseView);
}

void Camera::SetAspect(_In_ float aspectRatio)
{
    SetProjParams(m_fieldOfView, aspectRatio, m_nearPlane, m_farPlane);
}

void Camera::SetProjParams( _In_ float fieldOfView, _In_ float aspectRatio, _In_ float nearPlane, _In_ float farPlane )
{
    m_fieldOfView = fieldOfView;
    m_aspectRatio = aspectRatio;
    m_nearPlane = nearPlane;
    m_farPlane = farPlane;

    XMStoreFloat4x4( &m_projectionMatrix, XMMatrixPerspectiveFovLH( m_fieldOfView, m_aspectRatio, m_nearPlane, m_farPlane ) );
}

DirectX::XMMATRIX Camera::View()
{
    return XMLoadFloat4x4(&m_viewMatrix);
}

DirectX::XMMATRIX Camera::Projection()
{ 
    return XMLoadFloat4x4(&m_projectionMatrix);
}

DirectX::XMMATRIX Camera::World()
{
    return XMLoadFloat4x4(&m_inverseView);
}

DirectX::XMFLOAT3 Camera::Eye()
{
    return m_eye; 
}

DirectX::XMFLOAT3 Camera::Up()
{
    return m_up; 
}

DirectX::XMFLOAT3 Camera::LookAt()
{
    return m_lookAt;
}

float Camera::NearClipPlane()
{
    return m_nearPlane;
}

float Camera::FarClipPlane()
{
    return m_farPlane;
}

float Camera::Pitch()
{
    return m_cameraPitchAngle;
}

float Camera::Yaw()
{
    return m_cameraYawAngle;
}
