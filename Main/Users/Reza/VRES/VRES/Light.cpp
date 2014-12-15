#include "Precomp.h"
#include "VRES.h"

using namespace VRES;

Light::Light(LightType type, const XMFLOAT3& color) :
    SceneNode(SceneNodeType::Light), _type(type), _color(color)
{
}

Light::~Light()
{
}

DirectionalLight::DirectionalLight(const XMFLOAT3& color) : 
    Light(LightType::Directional, color)
{
}

PointLight::PointLight(const XMFLOAT3& color, float radius) : 
    Light(LightType::Point, color), _radius(radius)
{
}
