#pragma once

struct IGeometry;
struct ITexture;
class Transform;

enum class TextureUsage
{
    Diffuse,
    Normal,
    Specular,
    Max
};

struct __declspec(novtable) IModel
{
    virtual void SetGeometry(_In_ const std::shared_ptr<IGeometry>& geometry) = 0;
    virtual void SetTexture(_In_ TextureUsage usage, _In_ const std::shared_ptr<ITexture>& texture) = 0;

    virtual void SetOwner(_In_opt_ const std::shared_ptr<IReadonlyObject>& owner) = 0;

    // Without this set, the object will always be rendered opaquely
    virtual void EnableTransparency(_In_ bool enable) = 0;
};
