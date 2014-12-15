#pragma once

#include <Light.h>

class Light : public BaseObject<Light>, public ILight
{
public:
    static std::shared_ptr<Light> Create(_In_ LightType type, _In_ const XMFLOAT3& color, _In_ float radius);

    //
    // ILight
    //

    LightType GetType() const override;
    void SetPosition(_In_ const XMFLOAT3& position) override;
    void SetColor(_In_ const XMFLOAT3& color) override;
    void SetRadius(_In_ float radius) override;

    //
    // Internal
    //

    const XMFLOAT3& GetPosition() const;
    const XMFLOAT3& GetColor() const;
    float GetRadius() const;

private:
    Light(_In_ LightType type, _In_ const XMFLOAT3& color, _In_ float radius);

private:
    LightType _type;
    XMFLOAT3 _position;
    XMFLOAT3 _color;
    float _radius;
};
