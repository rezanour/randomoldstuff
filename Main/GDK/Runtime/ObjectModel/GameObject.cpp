#include <GameObject.h>
#include <GameWorld.h>
#include <GDKMath.h>
#include <GDKError.h>
#include <GraphicsDevice.h>
#include <ContentCache.h>
#include "RuntimeGameWorld.h"
#include <GeometryContent.h>

namespace GDK
{
    GameObject::GameObject(_In_ const std::wstring& typeName) :
        _typeName(typeName), _rotation(0), _transformDirty(true), _isValid(false)
    {
    }

    const std::wstring& GameObject::GetTypeName() const
    {
        return _typeName;
    }

    const std::wstring& GameObject::GetTexName() const
    {
        return _texName;
    }

    const std::wstring& GameObject::GetGeoName() const
    {
        return _geoName;
    }

    bool GameObject::IsValid() const
    {
        return _isValid;
    }

    const Vector3& GameObject::GetPosition() const
    {
        VerifyValid();
        return _position;
    }

    void GameObject::SetPosition(_In_ const Vector3& position)
    {
        VerifyValid();
        _position = position;
        _transformDirty = true;
    }

    float GameObject::GetRotation() const
    {
        VerifyValid();
        return _rotation;
    }

    void GameObject::SetRotation(_In_ float rotation)
    {
        VerifyValid();
        _rotation = rotation;
        _transformDirty = true;
    }

    const Matrix& GameObject::GetTransform() const
    {
        VerifyValid();
        EnsureTransform();
        return _transform;
    }

    std::shared_ptr<GameWorld> GameObject::GetGameWorld() const
    {
        VerifyValid();
        return _gameWorld.lock();
    }

    void GameObject::Destroy()
    {
        VerifyValid();

        // Remove from the world
        auto world = GetGameWorld();
        static_cast<RuntimeGameWorld*>(world.get())->RemoveObject(shared_from_this());

        OnRemovedFromWorld();

        OnDestroy();

        _isValid = false;
    }

    void GameObject::Update()
    {
        VerifyValid();
        OnUpdate();
    }

    void GameObject::Draw()
    {
        VerifyValid();

        if (OnDraw())
        {
            auto gameWorld = GetGameWorld();
            RuntimeGameWorld* world = static_cast<RuntimeGameWorld*>(gameWorld.get());
            auto& content = world->GetContentCache();

            gameWorld->GetGraphicsDevice()->BindTexture(0, content->GetTexture(_parameters.texture));
            gameWorld->GetGraphicsDevice()->BindGeometry(content->GetGeometry(_parameters.geometry));
            gameWorld->GetGraphicsDevice()->Draw(GetTransform(), 0);
        }
    }

    const CollisionPrimitive* GameObject::GetFastCollisionPrimitive() const
    {
        return _fastCollision.get();
    }

    const CollisionPrimitive* GameObject::GetAccurateCollisionPrimitive() const
    {
        return _accurateCollision.get();
    }

    void GameObject::OnCreate()
    {
    }

    void GameObject::OnAddedToWorld()
    {
    }

    void GameObject::OnRemovedFromWorld()
    {
    }

    void GameObject::OnDestroy()
    {
    }

    void GameObject::OnUpdate()
    {
    }

    bool GameObject::OnDraw()
    {
        return true;
    }

    void GameObject::Initialize(_In_ const std::shared_ptr<GameWorld>& gameWorld, _In_ const GameObjectCreateParameters& parameters)
    {
        _gameWorld = gameWorld;
        _parameters = parameters;

        // use default paths if none specified
        if (_parameters.geometry.empty())
        {
            _parameters.geometry = _typeName + L"\\tris.md2.geometry";
        }
        if (_parameters.texture.empty())
        {
            _parameters.texture = _typeName + L"\\skin.pcx.texture";
        }

        std::shared_ptr<GDK::GeometryContent> geometryContent;
        geometryContent = Content::LoadGeometryContent(_parameters.geometry);

        _geoName = _parameters.geometry;
        _texName = _parameters.texture;

        _fastCollision = geometryContent->GenerateFastCollisionObject();

        // only enable full triangle meshes in editing (for picking)
        // HACK! Need a way to detect static geometry that needs full collision (level pieces)
        if (gameWorld->IsEditing() || _parameters.geometry.find(L"monster") == std::wstring::npos)
        {
            _accurateCollision = geometryContent->GenerateAccurateCollisionObject();
        }

        _isValid = true;

        OnCreate();
    }

    void GameObject::EnsureTransform() const
    {
        if (_transformDirty)
        {
            _transform = Matrix::CreateRotationY(_rotation);
            _transform.SetTranslation(_position);
            _transformDirty = false;
        }
    }

    void GameObject::VerifyValid() const
    {
        CHECK_TRUE(_isValid);
    }
}
