#include "precomp.h"
#include "gameobject.h"
#include "simpleobject.h"

_Use_decl_annotations_
SimpleObject::SimpleObject(GamePlay* gamePlay, const GameObjectSpawnInfo* info) :
    GameObject(gamePlay, info),
    _flags(Flag::None),
    _rotationPerSecond(0.0f)
{
}

_Use_decl_annotations_
void SimpleObject::Update(float deltaTime)
{
    if (IsFlagSet(_flags, Flag::Rotate))
    {
        XMVECTOR rotation = XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), _rotationPerSecond * deltaTime);
        XMFLOAT4 orientation;
        XMStoreFloat4(&orientation, XMQuaternionMultiply(XMLoadFloat4(&GetOrientation()), rotation));
        SetOrientation(orientation);
    }
}
