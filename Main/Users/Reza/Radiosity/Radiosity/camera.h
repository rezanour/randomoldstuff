#pragma once

class Camera : public std::enable_shared_from_this<Camera>
{
public:
    Camera();

    const DirectX::XMFLOAT4X4& GetView() const;
    const DirectX::XMFLOAT4X4& GetProjection() const;

private:

private:
    DirectX::XMFLOAT4X4 _view;
    DirectX::XMFLOAT4X4 _projection;
};
