#include <Precomp.h>
#include "LightImpl.h"

std::shared_ptr<Light> Light::Create(_In_ LightType type, _In_ const XMFLOAT3& color, _In_ float radius)
{
    return std::shared_ptr<Light>(new Light(type, color, radius));
}

Light::Light(_In_ LightType type, _In_ const XMFLOAT3& color, _In_ float radius) :
    _type(type), _color(color), _radius(radius)
{
    if (radius != 0.0f)
    {
        assert(type == LightType::Point);
    }
}

LightType Light::GetType() const
{
    return _type;
}

void Light::SetPosition(_In_ const XMFLOAT3& position)
{
    if (_type == LightType::Directional)
    {
        // Normalize, since it's a direction we're storing
        // Invert, since we want to store the direction "to" the light, instead of light direction
        XMStoreFloat3(&_position, XMVectorNegate(XMVector3Normalize(XMLoadFloat3(&position))));
    }
    else
    {
        _position = position;;
    }
}

void Light::SetColor(_In_ const XMFLOAT3& color)
{
    _color = color;
}

void Light::SetRadius(_In_ float radius)
{
    assert(_type == LightType::Point);
    _radius = radius;
}

const XMFLOAT3& Light::GetPosition() const
{
    return _position;
}

const XMFLOAT3& Light::GetColor() const
{
    return _color;
}

float Light::GetRadius() const
{
    return _radius;
}
