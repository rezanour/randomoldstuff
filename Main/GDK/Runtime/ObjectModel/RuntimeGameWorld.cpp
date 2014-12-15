#include "RuntimeGameWorld.h"
#include "RuntimeGameObject.h"
#include <GameObject.h>
#include <GameObjectEdit.h>
#include <ContentCache.h>
#include <GameWorldContent.h>
#include <GDKError.h>
#include <Collision.h>
#include <CollisionHelpers.h>
#include <CollisionPrimitives.h>
#include <PhysicsInternal.h>
#include <StringUtils.h>

namespace GDK
{
    namespace GameWorld
    {
        std::shared_ptr<IGameWorldEdit> Create(_In_ const std::wstring& name, _In_ const DeviceContext& deviceContext)
        {
            return RuntimeGameWorld::Create(name, deviceContext);
        }

        std::shared_ptr<IGameWorldEdit> Create(_In_ const std::shared_ptr<GameWorldContent>& content, _In_ const DeviceContext& deviceContext)
        {
            return RuntimeGameWorld::Create(content, deviceContext, true);
        }
    }

    std::shared_ptr<RuntimeGameWorld> RuntimeGameWorld::Create(_In_ const std::wstring& name, _In_ const DeviceContext& deviceContext)
    {
        return std::shared_ptr<RuntimeGameWorld>(GDKNEW RuntimeGameWorld(name, deviceContext, true));
    }

    std::shared_ptr<RuntimeGameWorld> RuntimeGameWorld::Create(_In_ const std::shared_ptr<GameWorldContent>& content, _In_ const DeviceContext& deviceContext, _In_ bool enableEditing)
    {
        std::shared_ptr<RuntimeGameWorld> world(GDKNEW RuntimeGameWorld(content->GetName(), deviceContext, enableEditing));

        auto numModels = content->GetNumModels();
        auto models = content->GetModelEntries();
        auto chunks = content->GetModelChunks();
        auto numEntities = content->GetNumEntities();
        auto entities = content->GetEntityEntries();
        auto properties = content->GetEntityProperties();

        std::vector<VisualInfo> visuals;
        for (uint32_t i = 0; i < numModels; ++i)
        {
            visuals.clear();

            auto& entry = models[i];
            for (uint32_t c = entry.offset; c < entry.offset + entry.numChunks; ++c)
            {
                auto& chunk = chunks[c];
                visuals.push_back(VisualInfo(Matrix::Identity(), chunk.geometry, chunk.texture));
            }

            world->_models.push_back(visuals);
        }

        std::map<std::wstring, std::wstring> props;
        for (uint32_t i = 0; i < numEntities; ++i)
        {
            props.clear();

            auto& entry = entities[i];
            for (uint32_t p = entry.offset; p < entry.offset + entry.numProperties; ++p)
            {
                auto& prop = properties[p];
                props[prop.name] = prop.value;
            }

            world->CreateObject(props);
        }

        return world;
    }

    RuntimeGameWorld::RuntimeGameWorld(_In_ const std::wstring& name, _In_ const DeviceContext& deviceContext, _In_ bool enableEditing) :
        _name(name), _deviceContext(deviceContext), _editingEnabled(enableEditing), _content(ContentCache::Create(deviceContext)), 
        _physics(Physics::CreateWorld(Vector3(0, -3000, 0)))
    {
    }

    const std::wstring& RuntimeGameWorld::GetName() const
    {
        return _name;
    }

    void RuntimeGameWorld::SetName(_In_ const std::wstring& name)
    {
        _name.assign(name);
    }

    std::shared_ptr<IGameObject> RuntimeGameWorld::CreateObject(_In_ const std::map<std::wstring, std::wstring>& properties)
    {
        auto object = RuntimeGameObject::Create(shared_from_this(), properties);
        _objects.push_back(object);
        return object;
    }

    void RuntimeGameWorld::FindObjectsByClassName(_In_ const std::wstring& className, _Inout_ std::vector<std::shared_ptr<IGameObject>>& gameObjects) const
    {
        gameObjects.clear();
        for (size_t i = 0; i < _objects.size(); ++i)
        {
            if (_objects[i]->GetClassName() == className)
            {
                gameObjects.push_back(_objects[i]);
            }
        }
    }

    void RuntimeGameWorld::FindObjectsByTargetName(_In_ const std::wstring& targetName, _Inout_ std::vector<std::shared_ptr<IGameObject>>& gameObjects) const
    {
        gameObjects.clear();

        if (targetName == L"")
        {
            return;
        }

        for (size_t i = 0; i < _objects.size(); ++i)
        {
            if (_objects[i]->GetTargetName() == targetName)
            {
                gameObjects.push_back(_objects[i]);
            }
        }
    }

    const GameTime& RuntimeGameWorld::GetTime() const
    {
        return _gameTime;
    }

    const DeviceContext& RuntimeGameWorld::GetDeviceContext() const
    {
        return _deviceContext;
    }

    std::shared_ptr<IGameObjectEdit> RuntimeGameWorld::CreateModel(_In_ const std::vector<VisualInfo>& visualInfos)
    {
        _models.push_back(visualInfos);
        std::map<std::wstring, std::wstring> properties;
        if (_models.size() == 1) // first model, make it the world
        {
            properties[L"classname"] = L"worldspawn";
        }
        else
        {
            properties[L"model"] = L"*" + std::to_wstring(_models.size() - 1);
        }
        return CreateObjectEdit(properties);
    }

    const std::vector<VisualInfo>& RuntimeGameWorld::GetModel(_In_ uint32_t index) const
    {
        return _models[index];
    }

    std::shared_ptr<IGameObjectEdit> RuntimeGameWorld::CreateObjectEdit(_In_ const std::map<std::wstring, std::wstring>& properties)
    {
        auto object = RuntimeGameObject::Create(shared_from_this(), properties);
        _objects.push_back(object);
        return object;
    }

    void RuntimeGameWorld::RemoveObject(_In_ const std::shared_ptr<IGameObjectEdit>& object)
    {
        RemoveObject(static_cast<const RuntimeGameObject*>(object.get()));
    }

    void RuntimeGameWorld::RemoveObject(_In_ const std::shared_ptr<IGameObject>& gameObject)
    {
        RemoveObject(static_cast<const RuntimeGameObject*>(gameObject.get()));
    }

    void RuntimeGameWorld::RemoveObject(_In_ const RuntimeGameObject* gameObject)
    {
        for (auto it = _objects.begin(); it != _objects.end(); ++it)
        {
            if (it->get() == gameObject)
            {
                _objects.erase(it);
                return;
            }
        }
    }

    const std::shared_ptr<IPhysicsWorld>& RuntimeGameWorld::GetPhysicsWorld() const
    {
        return _physics;
    }

    void RuntimeGameWorld::Update(_In_ const GameTime& gameTime)
    {
        VerifyEditing(false);

        _gameTime = gameTime;
        _inUpdate = true;

        _objectsCopy = _objects;
        for (auto& object : _objectsCopy)
        {
            object->Update();
        }
        _objectsCopy.clear();

        if (_physics)
        {
            _physics->Update(gameTime.deltaTime);
        }

        for (auto& object : _objects)
        {
            object->PostPhysicsUpdate();
        }

        _inUpdate = false;
    }

    bool RuntimeGameWorld::IsEditing() const
    {
        return _editingEnabled;
    }

    void RuntimeGameWorld::Draw(_In_ const Matrix& view, _In_ const Matrix& projection)
    {
        _view = view;
        _projection = projection;

        _deviceContext.graphicsDevice->SetViewProjection(_view, _projection);

        _inDraw = true;

        for (auto& object : _objects)
        {
            auto& visualInfos = object->GetVisualInfos();

            for (auto& visualInfo : visualInfos)
            {
                _deviceContext.graphicsDevice->BindTexture(0, !visualInfo.texture.empty() ? _content->GetTexture(visualInfo.texture) : nullptr);
                _deviceContext.graphicsDevice->BindGeometry(_content->GetGeometry(visualInfo.geometry));
                _deviceContext.graphicsDevice->Draw(object->GetTransform().GetWorld(), 0);
            }
        }

        _inDraw = false;
    }

    std::shared_ptr<IGameObjectEdit> RuntimeGameWorld::PickClosestObject(_In_ const Ray& ray)
    {
        VerifyEditing(true);

        std::shared_ptr<IGameObjectEdit> closest;
        float closestDist = 0.0f;
        RaycastResult result;

        for (auto& object : _objects)
        {
            Matrix transform = object->GetTransform().GetWorld();
            if (object->GetVisualInfos().size())
            {
                if (Raycast(&ray, nullptr, object->GetSphere(), &transform, nullptr) &&
                    Raycast(&ray, nullptr, object->GetPickingMesh(), &transform, &result))
                {
                    if (!closest || closestDist > result.distance)
                    {
                        closest = object;
                        closestDist = result.distance;
                    }
                }
            }
        }

        return closest;
    }

    std::shared_ptr<GameWorldContent> RuntimeGameWorld::SaveToContent() const
    {
        VerifyEditing(true);

        std::vector<GameWorldContent::ModelEntry> models;
        std::vector<GameWorldContent::ModelChunk> chunks;
        std::vector<GameWorldContent::EntityEntry> entities;
        std::vector<GameWorldContent::EntityProperty> properties;

        for (auto& model : _models)
        {
            GameWorldContent::ModelEntry entry;
            entry.offset = static_cast<uint32_t>(chunks.size());
            entry.numChunks = static_cast<uint32_t>(model.size());

            for (auto& visual : model)
            {
                GameWorldContent::ModelChunk chunk;
                wcscpy_s(chunk.geometry, visual.geometry.c_str());
                wcscpy_s(chunk.texture, visual.texture.c_str());
                chunks.push_back(chunk);
            }

            models.push_back(entry);
        }

        std::map<std::wstring, std::wstring> props;
        for (auto& object : _objects)
        {
            props.clear();
            object->GetProperties(props);

            if (props.find(L"dontsave") == props.end())
            {
                GameWorldContent::EntityEntry entry;
                entry.offset = static_cast<uint32_t>(properties.size());
                entry.numProperties = static_cast<uint32_t>(props.size());

                for (auto& pair : props)
                {
                    GameWorldContent::EntityProperty prop;
                    wcscpy_s(prop.name, pair.first.c_str());
                    wcscpy_s(prop.value, pair.second.c_str());
                    properties.push_back(prop);
                }

                entities.push_back(entry);
            }
        }

        return GameWorldContent::Create(
            _name, 
            static_cast<uint32_t>(models.size()), models.data(), 
            static_cast<uint32_t>(chunks.size()), chunks.data(),
            static_cast<uint32_t>(entities.size()), entities.data(),
            static_cast<uint32_t>(properties.size()), properties.data());
    }

    void RuntimeGameWorld::VerifyEditing(_In_ bool editing) const
    {
        CHECK_TRUE(editing == _editingEnabled);
    }

    void RuntimeGameWorld::VerifyInUpdate() const
    {
        CHECK_TRUE(_inUpdate);
    }

    void RuntimeGameWorld::VerifyInDraw() const
    {
        CHECK_TRUE(_inDraw);
    }
}
