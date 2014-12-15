#pragma once

class Camera
{
public:
    void SetView(CXMMATRIX view);
    void SetProjection(CXMMATRIX projection);

    const XMFLOAT4X4& GetView() const;
    const XMFLOAT4X4& GetProjection() const;

    float GetNearPlane() const;
    float GetFarPlane() const;

private:
    XMFLOAT4X4 _view;
    XMFLOAT4X4 _projection;
};
