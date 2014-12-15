#include "precomp.h"
#include "GameObject.h"

GameObjectNew::GameObjectNew()
{

}

const Transform& GameObjectNew::GetTransform() const
{
    return _transform;
}

_Use_decl_annotations_
std::shared_ptr<GameObjectNew> GameObjectNew::CreateGameObject()
{
    return std::shared_ptr<GameObjectNew>(new GameObjectNew());
}