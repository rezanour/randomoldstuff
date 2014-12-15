#include "Precomp.h"
#include "GameWorld.h"
#include "GameObject.h"
#include "Quake2Game.h"

using namespace GDK;
using Microsoft::WRL::ComPtr;

namespace Quake2
{

ComPtr<GameWorld> GameWorld::Create(_In_ Quake2Game* game, _In_opt_z_ const wchar_t* name)
{
    ComPtr<GameWorld> world = Make<GameWorld>(game, name);

    ComPtr<IGameObjectEdit> gameObject;
    CHECKHR(world->CreateObject(L"enemies\\soldier.object", gameObject.ReleaseAndGetAddressOf()));
    GameObject* go = reinterpret_cast<GameObject*>(gameObject.Get());
    go->SetPosition(Vector3(-50.0f, 0.0f, 0.0f));

    CHECKHR(world->CreateObject(L"enemies\\soldier.object", gameObject.ReleaseAndGetAddressOf()));
    go = reinterpret_cast<GameObject*>(gameObject.Get());
    go->SetPosition(Vector3(50.0f, 0.0f, 0.0f));

    return world.Detach();
}

Microsoft::WRL::ComPtr<GameWorld> GameWorld::Create(_In_ Quake2Game* game, _In_ GDK::IStream* stream)
{
    auto world = Make<GameWorld>(game, GameObject::GetStringFromStream(stream).c_str());
    ULONG read = 0;
    size_t count = 0;
    CHECKHR(stream->Read(&count, sizeof(count), &read));
    while (count-- > 0)
    {
        world->_gameObjects.push_back(GameObject::Load(stream, game->Graphics(), game->Content()));
    }
    return world;
}

GameWorld::GameWorld(_In_ Quake2Game* game, _In_opt_z_ const wchar_t* name) :
    _name(name),
    _game(game)
{
}

GameWorld::~GameWorld()
{
}

//
// IWorldEdit
//
const wchar_t* GDKAPI GameWorld::GetName() const
{
    return _name.c_str();
}

void GDKAPI GameWorld::SetName(_In_z_ const wchar_t* name)
{
    _name = name;
}

HRESULT GDKAPI GameWorld::Save(_In_ GDK::IStream* stream)
{
    MODULE_GUARD_BEGIN

    GameObject::WriteStringToStream(stream, _name);

    ULONG written = 0;
    size_t count = _gameObjects.size();
    CHECKHR(stream->Write(&count, sizeof(count), &written));

    for (size_t i = 0; i < _gameObjects.size(); ++i)
    {
        _gameObjects[i]->Save(stream);
    }

    MODULE_GUARD_END
}

HRESULT GDKAPI GameWorld::CreateObject(_In_z_ const wchar_t* type, _COM_Outptr_ GDK::IGameObjectEdit** gameObject)
{
    MODULE_GUARD_BEGIN

    CHECK_NOT_NULL(type, E_INVALIDARG);
    CHECK_NOT_NULL(gameObject, E_POINTER);
    *gameObject = nullptr;

    auto go = GameObject::Create(type, _game->Graphics(), _game->Content());
    CHECKHR(go.CopyTo(gameObject));

    _gameObjects.push_back(go);

    MODULE_GUARD_END
}

HRESULT GDKAPI GameWorld::GetObjects(_COM_Outptr_ GDK::IStringList** ids)
{
    MODULE_GUARD_BEGIN

    CHECK_NOT_NULL(ids, E_POINTER);
    *ids = nullptr;

    std::vector<const wchar_t*> idList;
    for (size_t i = 0; i < _gameObjects.size(); ++i)
    {
        idList.push_back(_gameObjects[i]->GetId().c_str());
    }

    CHECKHR(GDK::CreateStringList(idList.data(), idList.size(), ids));

    MODULE_GUARD_END
}

HRESULT GDKAPI GameWorld::SetSelection(_In_opt_z_ const wchar_t* id)
{
    UNREFERENCED_PARAMETER(id);
    return E_NOTIMPL;
}

HRESULT GDKAPI GameWorld::RemoveObject(_In_z_ const wchar_t* id)
{
    MODULE_GUARD_BEGIN

    CHECK_NOT_NULL(id, E_INVALIDARG);

    for (auto it = _gameObjects.begin(); it != _gameObjects.end(); ++it)
    {
        if ((*it)->GetId().compare(id) == 0)
        {
            _gameObjects.erase(it);
            break;
        }
    }

    MODULE_GUARD_END
}

HRESULT GDKAPI GameWorld::GetObject(_In_z_ const wchar_t* id, _COM_Outptr_ GDK::IGameObjectEdit** gameObject)
{
    MODULE_GUARD_BEGIN

    CHECK_NOT_NULL(gameObject, E_POINTER);
    *gameObject = nullptr;

    CHECK_NOT_NULL(id, E_INVALIDARG);

    for (size_t i = 0; i < _gameObjects.size(); ++i)
    {
        if (_gameObjects[i]->GetId().compare(id) == 0)
        {
            CHECKHR(_gameObjects[i].CopyTo(gameObject));
            break;
        }
    }

    CHECKHR(HRESULT_FROM_WIN32(ERROR_NOT_FOUND));

    MODULE_GUARD_END
}

HRESULT GDKAPI GameWorld::PickClosestObject(_In_ const GDK::Matrix& view, _In_ const GDK::Matrix& projection, _In_ const GDK::Vector2& normalizedScreenPosition, _COM_Outptr_ GDK::IGameObjectEdit** gameObject)
{
    UNREFERENCED_PARAMETER(view);
    UNREFERENCED_PARAMETER(projection);
    UNREFERENCED_PARAMETER(normalizedScreenPosition);
    UNREFERENCED_PARAMETER(gameObject);

    return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
}

void GDKAPI GameWorld::Update(_In_ double elapsedSeconds)
{
    UNREFERENCED_PARAMETER(elapsedSeconds);
}

void GDKAPI GameWorld::Draw()
{
    auto graphics = _game->Graphics();

    for (size_t i = 0; i < _gameObjects.size(); ++i)
    {
        graphics->BindGeometry(_gameObjects[i]->GetGeometry());
        graphics->BindTexture(0, _gameObjects[i]->GetTexture());
        graphics->Draw(_gameObjects[i]->GetWorld());
    }
}

} // Quake2