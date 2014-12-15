#include <GameWorld.h>
#include "GameObject.h"

namespace GDK
{
    std::shared_ptr<GameWorld> GameWorld::CreateNew(_In_ const std::wstring& name)
    {
        return std::shared_ptr<GameWorld>(new GameWorld(name, true));
    }

    std::shared_ptr<GameWorld> GameWorld::Load(_In_ const std::wstring& name)
    {
        UNREFERENCED_PARAMETER(name);
        throw std::exception("Not implemented.");
    }

    GameWorld::GameWorld(_In_ const std::wstring& name, _In_ bool startEditing) :
        _name(name),
        _editingEnabled(startEditing)
    {
    }

    bool GameWorld::IsEditingEnabled() const
    {
        return _editingEnabled;
    }

    void GameWorld::EnableEditing(_In_ bool enable)
    {
        _editingEnabled = enable;
    }

    const std::wstring& GameWorld::GetName() const
    {
        return _name;
    }

    void GameWorld::SetName(_In_ const std::wstring& name)
    {
        assert(_editingEnabled);
        _name = name;
    }

    const GameObjectList& GameWorld::GetObjects() const
    {
        return _gameObjects;
    }

    void GameWorld::AddObject(_In_ const GameObjectPtr& gameObject)
    {
        _gameObjects.push_back(gameObject);
    }

    void GameWorld::RemoveObject(_In_ const GameObjectPtr& gameObject)
    {
        for (auto entry = _gameObjects.begin(); entry != _gameObjects.end(); ++entry)
        {
            if (*entry == gameObject)
            {
                _gameObjects.erase(entry);
                return;
            }
        }
    }

    const CameraPtr& GameWorld::GetActiveCamera() const
    {
        return _activeCamera;
    }

    void GameWorld::SetActiveCamera(_In_ const CameraPtr& camera)
    {
        _activeCamera = camera;
    }

    void GameWorld::Update(_In_ double elapsedSeconds)
    {
        UNREFERENCED_PARAMETER(elapsedSeconds);
    }

    void GameWorld::Draw()
    {
    }

    void GameWorld::Draw(_In_ const Matrix& view, _In_ const Matrix& projection)
    {
        UNREFERENCED_PARAMETER(view);
        UNREFERENCED_PARAMETER(projection);
    }
}
