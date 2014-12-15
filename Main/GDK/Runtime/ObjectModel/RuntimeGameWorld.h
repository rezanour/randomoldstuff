#pragma once

#include <Platform.h>
#include <RuntimeObject.h>
#include <GameWorld.h>
#include <GameWorldEdit.h>
#include <GameTime.h>
#include <GDKMath.h>
#include <DeviceContext.h>

namespace GDK
{
    struct IGameObject;
    struct IGameObjectEdit;
    class ContentCache;
    class GameWorldContent;
    class RuntimeGameObject;
    struct IPhysicsWorld;

    class RuntimeGameWorld : 
        public RuntimeObject<RuntimeGameWorld>,
        public IGameWorld,
        public IGameWorldEdit
    {
    public:
        // Creation
        static std::shared_ptr<RuntimeGameWorld> Create(_In_ const std::wstring& name, _In_ const DeviceContext& deviceContext);
        static std::shared_ptr<RuntimeGameWorld> Create(_In_ const std::shared_ptr<GameWorldContent>& content, _In_ const DeviceContext& deviceContext, _In_ bool enableEditing);

        // Common between IGameWorld & IGameWorldEdit
        virtual const std::vector<VisualInfo>& GetModel(_In_ uint32_t index) const override;
        virtual bool IsEditing() const override;

        // IGameWorld
        virtual const DeviceContext& GetDeviceContext() const override;

        virtual std::shared_ptr<IGameObject> CreateObject(_In_ const std::map<std::wstring, std::wstring>& properties) override;
        virtual void RemoveObject(_In_ const std::shared_ptr<IGameObject>& gameObject) override;

        virtual void FindObjectsByClassName(_In_ const std::wstring& className, _Inout_ std::vector<std::shared_ptr<IGameObject>>& gameObjects) const override;
        virtual void FindObjectsByTargetName(_In_ const std::wstring& targetName, _Inout_ std::vector<std::shared_ptr<IGameObject>>& gameObjects) const override;

        virtual const GameTime& GetTime() const override;

        // IGameWorldEdit
        virtual const std::wstring& GetName() const override;
        virtual void SetName(_In_ const std::wstring& name) override;

        virtual std::shared_ptr<IGameObjectEdit> CreateModel(_In_ const std::vector<VisualInfo>& visualInfos) override;

        virtual std::shared_ptr<IGameObjectEdit> CreateObjectEdit(_In_ const std::map<std::wstring, std::wstring>& properties) override;
        virtual void RemoveObject(_In_ const std::shared_ptr<IGameObjectEdit>& object) override;

        virtual void Draw(_In_ const Matrix& view, _In_ const Matrix& projection) override;

        virtual std::shared_ptr<IGameObjectEdit> PickClosestObject(_In_ const Ray& ray) override;

        virtual std::shared_ptr<GameWorldContent> SaveToContent() const override;

        // RuntimeGameWorld
        void Update(_In_ const GameTime& gameTime);
        const std::shared_ptr<IPhysicsWorld>& RuntimeGameWorld::GetPhysicsWorld() const;

    private:
        RuntimeGameWorld(_In_ const std::wstring& name, _In_ const DeviceContext& deviceContext, _In_ bool enableEditing);

        void RemoveObject(_In_ const RuntimeGameObject* gameObject);

        void VerifyEditing(_In_ bool editing) const;
        void VerifyInUpdate() const;
        void VerifyInDraw() const;

        std::wstring _name;
        bool _editingEnabled;
        bool _inUpdate;
        bool _inDraw;
        GameTime _gameTime;
        Matrix _view, _projection;
        DeviceContext _deviceContext;
        std::shared_ptr<IPhysicsWorld> _physics;
        std::shared_ptr<ContentCache> _content;

        std::vector<std::vector<VisualInfo>> _models;
        std::vector<std::shared_ptr<RuntimeGameObject>> _objects;
        std::vector<std::shared_ptr<RuntimeGameObject>> _objectsCopy;
    };
}
