#pragma once

#include <RuntimeObject.h>
#include <GameObject.h>
#include <GameObjectEdit.h>
#include <Transform.h>
#include <PhysicsInternal.h>

namespace GDK
{
    class RuntimeGameWorld;
    struct CollisionPrimitive;

    class RuntimeGameObject : 
        public RuntimeObject<RuntimeGameObject>, 
        public IGameObject, 
        public IGameObjectEdit
    {
    public:
        // Creation
        static std::shared_ptr<RuntimeGameObject> Create(_In_ const std::shared_ptr<RuntimeGameWorld>& world, _In_ const std::map<std::wstring, std::wstring>& properties);

        // Common overrides
        virtual const std::wstring& GetClassName() const override;
        virtual const Transform& GetTransform() const override;
        virtual Transform& GetTransform() override;

        // IGameObject-specific
        virtual const std::wstring& GetTargetName() const override;
        virtual const std::shared_ptr<IGameObjectController>& GetController() const override;
        virtual const std::shared_ptr<IGameWorld> GetGameWorld() const override;
        virtual void AddImpulse(_In_ const Vector3& value) override;
        virtual const std::vector<GameObjectContact>& GetContacts() const override;

        // RuntimeGameObject
        void Update();
        void PostPhysicsUpdate();
        const std::vector<VisualInfo>& GetVisualInfos() const;
        const CollisionPrimitive* GetSphere() const;
        const CollisionPrimitive* GetPickingMesh() const;
        void GetProperties(_Inout_ std::map<std::wstring, std::wstring>& properties) const;

        ~RuntimeGameObject();

    private:
        RuntimeGameObject(_In_ const std::shared_ptr<RuntimeGameWorld>& world, _In_ const GameObjectCreateParameters& parameters);

        std::wstring _className;
        std::wstring _targetName;
        Transform _transform;
        std::map<std::wstring, std::wstring> _initialProperties;
        std::vector<VisualInfo> _visualInfos;
        std::weak_ptr<RuntimeGameWorld> _gameWorld;
        std::shared_ptr<IGameObjectController> _controller;

        std::weak_ptr<IRigidBody> _body;

        // When editing, we keep a triangle list to do picking against.
        // NOTE: once the full collision system is figured out, we may deprecate this.
        std::shared_ptr<CollisionPrimitive> _sphere;
        std::shared_ptr<CollisionPrimitive> _triangleMesh;
    };
}
