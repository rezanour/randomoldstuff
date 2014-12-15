#include "precomp.h"
#include "gameobject.h"
#include "simpleobject.h"

GameObject* GameObject::Spawn(_In_ GamePlay* gamePlay, _In_ const GameObjectSpawnInfo* info)
{
    auto& content = gamePlay->GetContent();

    if (_strcmpi(info->Type, "testcube") == 0)
    {
        SimpleObject* object = new SimpleObject(gamePlay, info);
        object->_flags = SimpleObject::Flag::Rotate;
        object->SetGeometry(content.GetGeometry("cube.obj"));
        object->SetTexture(TextureType::Diffuse, content.GetTexture("blueprint.png", true));
        object->_rotationPerSecond = 0.5f;

        return object;
    }
    else if (_strcmpi(info->Type, "testcastle") == 0)
    {
        SimpleObject* object = new SimpleObject(gamePlay, info);
        object->SetGeometry(content.GetGeometry("castle/castle01.obj"));
        object->SetTexture(TextureType::Diffuse, content.GetTexture("blueprint.png", true));
        return object;
    }

    Error("Invalid game object type");
    return nullptr;
}
