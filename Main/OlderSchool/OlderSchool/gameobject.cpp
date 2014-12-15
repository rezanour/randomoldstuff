#include "precomp.h"
#include "gameobject.h"

_Use_decl_annotations_
std::shared_ptr<GameObject> GameObject::Create(GameScreen* screen)
{
    return std::shared_ptr<GameObject>(new GameObject(screen));
}

_Use_decl_annotations_
GameObject::GameObject(GameScreen* screen) :
    _screen(screen), _position(XMFLOAT3(0, 0, 0)), _orientation(XMFLOAT4(0, 0, 0, 1)), _scale(XMFLOAT3(1, 1, 1)),
    _isDead(false), _isVisible(false), _isSolid(false), _worldIsDirty(true)
{
}

const XMFLOAT4X4& GameObject::GetWorld() const
{
    if (_worldIsDirty)
    {
        XMStoreFloat4x4(&_cachedWorld, XMMatrixAffineTransformation(XMLoadFloat3(&_scale), XMVectorZero(), XMLoadFloat4(&_orientation), XMLoadFloat3(&_position)));
        _worldIsDirty = false;
    }
    return _cachedWorld;
}
