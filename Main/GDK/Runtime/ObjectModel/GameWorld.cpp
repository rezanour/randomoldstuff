#include <GameWorld.h>
#include <GameObject.h>
#include <GDKMath.h>
#include <Collision.h>
#include <GDKError.h>
#include "RuntimeGameWorld.h"

namespace GDK
{
    static std::map<std::wstring, GameObjectCreateParameters> g_registeredTypes;
    static GameObjectCreateMethod g_defaultFactory;

    void GameWorld::RegisterDefaultFactory(_In_ GameObjectCreateMethod defaultFactory)
    {
        g_defaultFactory = defaultFactory;
    }

    void GameWorld::RegisterObjectType(_In_ const std::wstring& typeName, _In_ const GameObjectCreateParameters& parameters)
    {
        g_registeredTypes[typeName] = parameters;
    }

    const GameObjectCreateParameters& GameWorld::GetRegisteredTypeInfo(_In_ const std::wstring& typeName)
    {
        return g_registeredTypes[typeName];
    }

    std::vector<std::wstring> GameWorld::GetRegisteredObjectTypes()
    {
        std::vector<std::wstring> types;

        for (auto& pair : g_registeredTypes)
        {
            types.push_back(pair.first);
        }

        return types;
    }

    std::shared_ptr<GameWorld> GameWorld::Create(_In_ const GameWorldDevices& devices, _In_ const std::wstring& name)
    {
        UNREFERENCED_PARAMETER(name);
        return RuntimeGameWorld::Create(devices);
    }

    std::shared_ptr<GameWorld> GameWorld::Create(_In_ const GameWorldDevices& devices, _In_ const std::shared_ptr<GameWorldContent>& content, _In_ bool enableEditing)
    {
        return RuntimeGameWorld::Create(devices, content, enableEditing);
    }

    std::shared_ptr<GameWorld> GameWorld::Create(_In_ const GameWorldDevices& devices, _In_ const std::shared_ptr<GameWorld>& existingWorld, _In_ bool enableEditing)
    {
        return RuntimeGameWorld::Create(devices, existingWorld, enableEditing);
    }

    GameWorld::GameWorld(_In_ const GameWorldDevices& devices, _In_ bool enableEditing) :
        _devices(devices), _editingEnabled(enableEditing)
    {
    }

    std::shared_ptr<GameObject> GameWorld::CreateObject(_In_ const std::wstring& typeName)
    {
        std::shared_ptr<GameObject> gameObject;

        auto entry = g_registeredTypes.find(typeName);
        if (entry != g_registeredTypes.end())
        {
            gameObject = entry->second.factory(typeName);
            gameObject->Initialize(shared_from_this(), entry->second);
        }
        else
        {
            gameObject = g_defaultFactory(typeName);
            gameObject->Initialize(shared_from_this(), GameObjectCreateParameters());
        }

        _gameObjects.push_back(gameObject);
        gameObject->OnAddedToWorld();

        return gameObject;
    }

    std::shared_ptr<GameObject> GameWorld::CreateObjectEx(_In_ const std::wstring& geometryName, _In_ const std::wstring& textureName)
    {
        std::shared_ptr<GameObject> gameObject;

        gameObject = g_defaultFactory(L"");
        GameObjectCreateParameters gocr;
        gocr.geometry = geometryName;
        gocr.texture = textureName;
        gameObject->Initialize(shared_from_this(), gocr);

        _gameObjects.push_back(gameObject);
        gameObject->OnAddedToWorld();

        return gameObject;
    }

    std::vector<std::shared_ptr<GameObject>> GameWorld::GetGameObjects() const
    {
        VerifyEditing(true);

        return _gameObjects;
    }

    std::shared_ptr<GameObject> GameWorld::PickGameObject(_In_ const Ray& ray) const
    {
        VerifyEditing(true);

        std::shared_ptr<GameObject> closest;
        float closestDistance = 0.0f;

        Matrix identity(Matrix::Identity());
        RaycastResult result;
        for (auto& gameObject : _gameObjects)
        {
            auto fast = gameObject->GetFastCollisionPrimitive();
            auto& trans = gameObject->GetTransform();
            if (fast && Raycast(&ray, identity, fast, trans, &result))
            {
                auto accurate = gameObject->GetAccurateCollisionPrimitive();
                if (accurate && !Raycast(&ray, identity, accurate, trans, &result))
                {
                    continue;
                }

                if (!closest || result.distance < closestDistance)
                {
                    closest = gameObject;
                    closestDistance = result.distance;
                }
            }
        }

        return closest;
    }

    bool GameWorld::IsEditing() const
    {
        return _editingEnabled;
    }

    void GameWorld::VerifyEditing(_In_ bool editingEnabled) const
    {
        CHECK_TRUE(_editingEnabled == editingEnabled);
    }

    const std::shared_ptr<GraphicsDevice>& GameWorld::GetGraphicsDevice() const
    {
        return _devices.graphicsDevice;
    }
}
