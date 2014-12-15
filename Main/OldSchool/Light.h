#pragma once

class Transform;

enum class LightType
{
    Ambient,
    Directional,
    Point
};

struct __declspec(novtable) ILight
{
    virtual LightType GetType() const = 0;

    virtual void SetColor(_In_ const XMFLOAT3& color) = 0;

    // For ambient lights, this is ignored
    // For directional lights, this is the direction the light is pointing
    // For point lights, it's the position of the point light
    virtual void SetPosition(_In_ const XMFLOAT3& position) = 0;

    // Point lights only
    virtual void SetRadius(_In_ float radius) = 0;
};
