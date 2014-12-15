#include "precomp.h"
#include "gameplay.h"
#include "gameobject.h"
#include "scene.h"
#include "material.h"

_Use_decl_annotations_
GameObject::GameObject(GamePlay* gamePlay, const GameObjectSpawnInfo* info) :
    _gamePlay(gamePlay),
    _material(MaterialType::Matte),
    _node(nullptr),
    _bounds(),
    _boundsDirty(true),
    _worldDirty(true),
    _flags(GameObjectFlag::None),
    _position(info->Position),
    _orientation(info->Orientation)
{
}

GameObject::~GameObject()
{
    _gamePlay->GetScene()->RemoveObject(this);
    _node = nullptr;
}

_Use_decl_annotations_
bool GameObject::HasAllFlags(GameObjectFlag flags) const
{
    return (static_cast<uint32_t>(_flags) & static_cast<uint32_t>(flags)) == static_cast<uint32_t>(flags);
}

_Use_decl_annotations_
bool GameObject::HasAnyFlags(GameObjectFlag flags) const
{
    return (static_cast<uint32_t>(_flags) & static_cast<uint32_t>(flags)) != 0;
}

const XMFLOAT4X4& GameObject::GetWorldTransform() const
{
    if (_worldDirty)
    {
        XMStoreFloat4x4(&_world, XMMatrixMultiply(XMMatrixRotationQuaternion(XMLoadFloat4(&_orientation)), XMMatrixTranslation(_position.x, _position.y, _position.z)));
        _worldDirty = false;
    }
    return _world;
}

const AABB& GameObject::GetBounds() const
{
    if (_boundsDirty)
    {
        _bounds = AABB(_position, _position);
        _boundsDirty = false;
    }
    return _bounds;
}

_Use_decl_annotations_
void GameObject::SetGeometry(const Geometry& geometry)
{
    _geometry = geometry;
}

_Use_decl_annotations_
void GameObject::SetTexture(TextureType type, const Texture& texture)
{
    _textures[(uint32_t)type] = texture;;
}
