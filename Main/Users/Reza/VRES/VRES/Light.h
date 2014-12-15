#pragma once

#include "Scene.h"

namespace VRES
{
    enum class LightType
    {
        Directional = 0,
        Point,
    };

    class Light : NonCopyable<Light>, public SceneNode
    {
    public:
        virtual ~Light();

        LightType Type() const { return _type; };
        const XMFLOAT3& Color() const { return _color; }

        void SetColor(const XMFLOAT3& color) { _color = color; }

    protected:
        Light(LightType type, const XMFLOAT3& color);

    private:
        LightType _type;
        XMFLOAT3 _color;
    };

    class DirectionalLight : NonCopyable<DirectionalLight>, public Light
    {
    public:
        DirectionalLight(const XMFLOAT3& color);
    };

    class PointLight : NonCopyable<PointLight>, public Light
    {
    public:
        PointLight(const XMFLOAT3& color, float radius);

        float Radius() const { return _radius; }

    private:
        float _radius;
    };
}
