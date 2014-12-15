#pragma once

class Camera
{
public:
    Camera();

    void SetViewParams(_In_ const GDK::Vector3& eye, _In_ const GDK::Vector3& lookAt, _In_ const GDK::Vector3& up);
    void SetProjParams(_In_ float fieldOfView, _In_ float aspectRatio, _In_ float nearPlane, _In_ float farPlane);

    GDK::Matrix View();
    GDK::Matrix Projection();
    GDK::Matrix World();

    void SetAspect(_In_ float aspectRatio);
    float GetAspect() const;
    void SetSpeed(_In_ float speed);

    void RotateAroundUp(_In_ float angle);
    void RotateAroundRight(_In_ float angle);
    void Move(_In_ const GDK::Vector3& offset);

    float GetFOV() const;

protected:
    GDK::Matrix _worldMatrix;
    GDK::Matrix _viewMatrix;
    GDK::Matrix _projectionMatrix;

    float _fieldOfView;
    float _nearPlane;
    float _farPlane;
    float _aspectRatio;

    float _pitch;
    float _yaw;

    float _speed;

    void UpdateViewMatrix();
    void UpdateProjectionMatrix(_In_ float aspectRatio);
};

class FirstPersonCameraWithInput : public Camera
{
public:
    FirstPersonCameraWithInput() : 
        _mouseDown(false)
    {
        
    }

    void Update(_In_ float elapsedGameTime);

protected:
    bool _mouseDown;
    GDK::Vector2 _mouseDownPos;
};