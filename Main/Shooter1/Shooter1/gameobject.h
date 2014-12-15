#pragma once

#include "texture.h"
#include "geometry.h"
#include "material.h"

class GamePlay;

enum class GameObjectFlag
{
    None = 0,
    Max
};

struct GameObjectSpawnInfo
{
    const char* Type;
    XMFLOAT3 Position;
    XMFLOAT4 Orientation;
};

class GameObject : public TrackedObject<MemoryTag::GameObject>
{
public:
    static GameObject* Spawn(_In_ GamePlay* gamePlay, _In_ const GameObjectSpawnInfo* info);
    virtual ~GameObject();

    const XMFLOAT3& GetPosition() const { return _position; }
    const XMFLOAT4& GetOrientation() const { return _orientation; }
    const XMFLOAT4X4& GetWorldTransform() const;

    bool HasAllFlags(_In_ GameObjectFlag flags) const;
    bool HasAnyFlags(_In_ GameObjectFlag flags) const;

    const AABB& GetBounds() const;

    MaterialType GetMaterial() const { return _material; }
    void SetMaterial(_In_ MaterialType material) { _material = material; }

    const Geometry& GetGeometry() const { return _geometry; }
    const Texture& GetTexture(_In_ TextureType type) const { return _textures[static_cast<uint32_t>(type)]; }

    virtual void Update(_In_ float deltaTime) = 0;

protected:
    GameObject(_In_ GamePlay* gamePlay, _In_ const GameObjectSpawnInfo* info);

    void SetPosition(_In_ const XMFLOAT3& position) { _position = position; _worldDirty = true; _boundsDirty = true; }
    void SetOrientation(_In_ const XMFLOAT4& orientation) { _orientation = orientation; _worldDirty = true; _boundsDirty = true; }

    void SetGeometry(_In_ const Geometry& geometry);
    void SetTexture(_In_ TextureType type, _In_ const Texture& texture);

private:
    GameObject(const GameObject&);

private:
    friend class Scene;
    friend class SceneNode;
    SceneNode* _node;
    GamePlay* _gamePlay;

    GameObjectFlag _flags : 30; 
    mutable bool _boundsDirty : 1;
    mutable bool _worldDirty : 1;

    XMFLOAT3 _position;
    XMFLOAT4 _orientation;
    mutable AABB _bounds;
    mutable XMFLOAT4X4 _world;
    Geometry _geometry;
    Texture _textures[static_cast<uint32_t>(TextureType::Max)];
    MaterialType _material;
};
