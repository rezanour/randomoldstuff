#include "stdafx.h"

Camera::Camera() :
    _pitch(0.0f),
    _yaw(0.0f),
    _speed(40.0f),
    _aspectRatio(1.0f)
{
    // Setup the view matrix
    SetViewParams( 
        GDK::Vector3::Zero(),    // default eye position.
        GDK::Vector3::Forward(), // default look at position.
        GDK::Vector3::Up()       // default up vector.
        );

    // Setup the projection matrix
    SetProjParams(GDK::Math::PiOver4, _aspectRatio, 1.0f, 100000.0f);
}

_Use_decl_annotations_
void Camera::SetViewParams(const GDK::Vector3& eye, const GDK::Vector3& lookAt, const GDK::Vector3& up )
{
    _worldMatrix.SetTranslation(eye);
    _worldMatrix.SetUp(up);
    auto forward = GDK::Vector3::Normalize(lookAt - eye);
    _worldMatrix.SetForward(forward);
    _worldMatrix.SetRight(GDK::Vector3::Cross(up, forward));
    UpdateViewMatrix();
}

_Use_decl_annotations_
void Camera::SetAspect(float aspectRatio)
{
    UpdateProjectionMatrix(aspectRatio);
}

float Camera::GetAspect() const
{
    return _aspectRatio;
}

_Use_decl_annotations_
void Camera::SetSpeed(float speed)
{
    _speed = speed;
}

_Use_decl_annotations_
void Camera::SetProjParams(float fieldOfView, float aspectRatio, float nearPlane, float farPlane )
{
    _fieldOfView = fieldOfView;
    _nearPlane = nearPlane;
    _farPlane = farPlane;
    _aspectRatio = aspectRatio;

    UpdateProjectionMatrix(aspectRatio);
}

GDK::Matrix Camera::View()
{
    return _viewMatrix;
}

GDK::Matrix Camera::Projection()
{ 
    return _projectionMatrix;
}

GDK::Matrix Camera::World()
{
    return _worldMatrix;
}

float Camera::GetFOV() const
{
    return _fieldOfView;
}

_Use_decl_annotations_
void FirstPersonCameraWithInput::Update(float elapsedGameTime)
{
    if (Mouse::IsButtonJustPressed(VK_RBUTTON))
    {
        _mouseDown = true;
        _mouseDownPos = Mouse::GetPosition();
    }
    
    if (Mouse::IsButtonJustReleased(VK_RBUTTON))
    {
        _mouseDown = false;
        _mouseDownPos = GDK::Vector2::Zero();
    }

    if (Mouse::IsButtonHeld(VK_RBUTTON))
    {
        if (_mouseDown)
        {
            GDK::Vector2 mouseCurPos = Mouse::GetPosition();
            GDK::Vector2 mouseCurDelta = Mouse::GetPositionDelta();
            GDK::Vector2 rotationDelta;
            rotationDelta.x = mouseCurDelta.x * 0.008f;
            rotationDelta.y = mouseCurDelta.y * 0.008f;

            GDK::Vector3 positionOffset;

            _pitch -= rotationDelta.y;
            _yaw += rotationDelta.x;

            // Limit pitch to straight up or straight down
            auto newPitch = __max(-GDK::Math::PiOver2, _pitch);
            newPitch = __min(+GDK::Math::PiOver2, newPitch);

            RotateAroundUp(rotationDelta.x);
            RotateAroundRight(rotationDelta.y - (_pitch - newPitch));

            _pitch = newPitch;

            auto position = _worldMatrix.GetTranslation();
            auto forward = _worldMatrix.GetForward();
            auto right = _worldMatrix.GetRight();

            if (Keyboard::IsKeyDown(VkKeyScan('s')) || Keyboard::IsKeyDown(VkKeyScan('w')))
            {
                if (Keyboard::IsKeyDown(VkKeyScan('s')))
                {
                    positionOffset -= forward * _speed * elapsedGameTime;
                }
                else
                {
                    positionOffset += forward * _speed * elapsedGameTime;
                }
            }

            if (Keyboard::IsKeyDown(VkKeyScan('a')) || Keyboard::IsKeyDown(VkKeyScan('d')))
            {
                if (Keyboard::IsKeyDown(VkKeyScan('d')))
                {
                    positionOffset += right * _speed * elapsedGameTime;
                }
                else
                {
                    positionOffset -= right * _speed * elapsedGameTime;
                }
            }

            Move(positionOffset);
        }
    }
}

_Use_decl_annotations_
void Camera::RotateAroundUp(float angle)
{
    auto forward = GDK::Vector3::Normalize(_worldMatrix.GetForward() * GDK::Matrix::CreateRotationY(angle));
    auto right = GDK::Vector3::Cross(_worldMatrix.GetUp(), forward);
    _worldMatrix.SetForward(forward);
    _worldMatrix.SetRight(right);
    UpdateViewMatrix();
}

_Use_decl_annotations_
void Camera::RotateAroundRight(float angle)
{
    auto right = _worldMatrix.GetRight();
    auto matrix = GDK::Matrix::CreateFromAxisAngle(right, angle);
    auto forward = GDK::Vector3::Normalize(_worldMatrix.GetForward() * matrix);
    auto up = GDK::Vector3::Up();
    right = GDK::Vector3::Cross(up, forward);
    _worldMatrix.SetForward(forward);
    _worldMatrix.SetRight(right);
    _worldMatrix.SetUp(up);
    UpdateViewMatrix();
}

_Use_decl_annotations_
void Camera::Move(const GDK::Vector3& offset)
{
    _worldMatrix.SetTranslation(_worldMatrix.GetTranslation() + offset);
    UpdateViewMatrix();
}

void Camera::UpdateViewMatrix()
{
    auto position = _worldMatrix.GetTranslation();
    auto forward = _worldMatrix.GetForward();
    auto up = _worldMatrix.GetUp();
    _viewMatrix = GDK::Matrix::CreateLookAt(position, position + forward, up);
}

_Use_decl_annotations_
void Camera::UpdateProjectionMatrix(float aspectRatio)
{
    _aspectRatio = aspectRatio;
    _projectionMatrix = GDK::Matrix::CreatePerspectiveFov(_fieldOfView, aspectRatio, _nearPlane, _farPlane);
}
