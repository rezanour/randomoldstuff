#pragma once

#include <Model.h>

class GraphicsScene;

class Model : public BaseObject<Model>, public IModel
{
public:
    static std::shared_ptr<Model> Create(_In_opt_ const std::shared_ptr<IReadonlyObject>& owner);

    //
    // IModel
    //

    void SetGeometry(_In_ const std::shared_ptr<IGeometry>& geometry) override;
    void SetTexture(_In_ TextureUsage usage, _In_ const std::shared_ptr<ITexture>& texture) override;
    void SetOwner(_In_opt_ const std::shared_ptr<IReadonlyObject>& owner) override;
    void EnableTransparency(_In_ bool enable) override;

    //
    // Internal
    //

    uint32_t GetId() const;
    void SetScene(_In_ const std::shared_ptr<GraphicsScene>& scene);
    const std::shared_ptr<IGeometry>& GetGeometry() const;
    const std::shared_ptr<ITexture>& GetTexture(_In_ TextureUsage usage) const;
    std::shared_ptr<IReadonlyObject> GetOwner() const;
    bool IsTransparent() const;

private:
    Model();

    void OnAABBChanged();

private:
    std::weak_ptr<IReadonlyObject> _owner;
    std::weak_ptr<GraphicsScene> _scene;
    uint32_t _token;

    std::shared_ptr<IGeometry> _geometry;
    std::shared_ptr<ITexture> _textures[static_cast<uint32_t>(TextureUsage::Max)];
    bool _isTransparent;

    static uint32_t s_nextId;
    uint32_t _id;
};
