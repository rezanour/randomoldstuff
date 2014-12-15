#include "RuntimeGameObject.h"
#include "RuntimeGameWorld.h"
#include <GDKError.h>
#include <Content.h>
#include <Collision.h>
#include <CollisionPrimitives.h>
#include <CollisionHelpers.h>
#include <PhysicsInternal.h>
#include <SpacePartition.h>
#include <StringUtils.h>

namespace GDK
{
    static GameObjectCreateMethod g_gameObjectFactory = nullptr;

    namespace GameObject
    {
        void RegisterFactory(_In_ GameObjectCreateMethod createMethod)
        {
            g_gameObjectFactory = createMethod;
        }
    }

    static RigidBodyType PhysicsBodyTypeToRigidBodyType(_In_ PhysicsBodyType type)
    {
        switch (type)
        {
        case PhysicsBodyType::Normal:       return RigidBodyType::Normal;
        case PhysicsBodyType::Kinematic:    return RigidBodyType::Kinematic;
        case PhysicsBodyType::Sensor:       return RigidBodyType::Sensor;
        default:
            throw std::invalid_argument("type");
        }
    }

    std::shared_ptr<RuntimeGameObject> RuntimeGameObject::Create(_In_ const std::shared_ptr<RuntimeGameWorld>& world, _In_ const std::map<std::wstring, std::wstring>& properties)
    {
        CHECK_NOT_NULL(g_gameObjectFactory);

        // Get details from the gameplay module
        GameObjectCreateParameters parameters = g_gameObjectFactory(world, properties);

        // Create base object with those parameters
        std::shared_ptr<RuntimeGameObject> object(GDKNEW RuntimeGameObject(world, parameters));

        // stash away these initial properties for later
        object->_initialProperties = properties;

        // do we have rigid body info? (requires a collision primitive)
        if (parameters.collisionPrimitive)
        {
            RigidBodyCreateParameters rigidBodyParams(parameters.position, 100.0f, parameters.collisionPrimitive.get());
            rigidBodyParams.type = PhysicsBodyTypeToRigidBodyType(parameters.physicsType);
            rigidBodyParams.gravityScale = (parameters.floating) ? 0.0f : 1.0f;
            object->_body = world->GetPhysicsWorld()->CreateBody(object, rigidBodyParams);
        }

        // if we're in the editor, let's create the picking mesh & sphere
        if (world->IsEditing())
        {
            std::vector<Triangle> triangles;
            Matrix identity = Matrix::Identity();

            auto& visuals = object->GetVisualInfos();
            if (visuals.size())
            {
                for (auto i = 0; i < visuals.size(); ++i)
                {
                    Collision::TriangleListFromGeometry(Content::LoadGeometryContent(visuals[i].geometry), identity, 0, triangles);
                }

                object->_triangleMesh = CollisionPrimitive::Create(TriangleMesh(SpacePartitionType::AabbTree, triangles));

                // loose sphere around the AABB. Not a best fit, but fast to compute and "good enough" as a pre-test
                Vector3 aabbMin, aabbMax;
                GetAabbForPrimitive(object->_triangleMesh.get(), &aabbMin, &aabbMax);
                object->_sphere = CollisionPrimitive::Create(Sphere((aabbMin + aabbMax) * 0.5f, (aabbMax - aabbMin).Length() * 0.5f));
            }
        }

        // Bind the object and controller, if one was provided
        if (parameters.controller)
        {
            parameters.controller->OnCreate(object);
        }

        return object;
    }

    RuntimeGameObject::RuntimeGameObject(_In_ const std::shared_ptr<RuntimeGameWorld>& world, _In_ const GameObjectCreateParameters& parameters) :
        _gameWorld(world), _className(parameters.className), _controller(parameters.controller), _visualInfos(parameters.visuals), _targetName(parameters.targetName)
    {
        _transform.SetPosition(parameters.position);
        _transform.SetRotation(parameters.rotation);
    }

    RuntimeGameObject::~RuntimeGameObject()
    {
        auto body = _body.lock();
        auto world = _gameWorld.lock();

        if (body && world)
        {
            world->GetPhysicsWorld()->DestroyBody(body);
        }
    }

    const std::wstring& RuntimeGameObject::GetClassName() const
    {
        return _className;
    }

    const Transform& RuntimeGameObject::GetTransform() const
    {
        return _transform;
    }

    Transform& RuntimeGameObject::GetTransform()
    {
        return _transform;
    }

    void RuntimeGameObject::AddImpulse(_In_ const Vector3& value)
    {
        auto body = _body.lock();
        if (body)
        {
            body->AddImpulse(value);
        }
    }

    const std::vector<GameObjectContact>& RuntimeGameObject::GetContacts() const
    {
        static std::vector<GameObjectContact> emptyContacts;
        auto body = _body.lock();
        if (body)
        {
            return body->GetContacts();
        }
        return emptyContacts;
    }

    const std::wstring& RuntimeGameObject::GetTargetName() const
    {
        return _targetName;
    }

    const std::shared_ptr<IGameObjectController>& RuntimeGameObject::GetController() const
    {
        return _controller;
    }

    const std::shared_ptr<IGameWorld> RuntimeGameObject::GetGameWorld() const
    {
        return _gameWorld.lock();
    }

    void RuntimeGameObject::Update()
    {
        if (_controller)
        {
            _controller->OnUpdate();
        }

        auto body = _body.lock();

        // if we have a body, synch to the body
        if (body)
        {
            body->SetPosition(GetTransform().GetPosition());
            body->SetRotation(GetTransform().GetRotation());
        }
    }

    void RuntimeGameObject::PostPhysicsUpdate()
    {
        auto body = _body.lock();

        // if we have a physics body, synch from body
        if (body)
        {
            // no need to read rotation. Our physics engine doesn't change it
            GetTransform().SetPosition(body->GetPosition());
        }
    }

    const std::vector<VisualInfo>& RuntimeGameObject::GetVisualInfos() const
    {
        return _visualInfos;
    }

    const CollisionPrimitive* RuntimeGameObject::GetSphere() const
    {
        CHECK_TRUE(_gameWorld.lock()->IsEditing());
        return _sphere.get();
    }

    const CollisionPrimitive* RuntimeGameObject::GetPickingMesh() const
    {
        CHECK_TRUE(_gameWorld.lock()->IsEditing());
        return _triangleMesh.get();
    }

    void RuntimeGameObject::GetProperties(_Inout_ std::map<std::wstring, std::wstring>& properties) const
    {
        properties = _initialProperties;

        properties[L"origin"] = StringUtils::ToString(_transform.GetPosition());
        properties[L"angle"] = StringUtils::ToString(_transform.GetRotation());

        if (_initialProperties.find(L"dontsave") != _initialProperties.end())
        {
            properties[L"dontsave"] = L"true";
        }

        // allow controller to insert game-specific stuff
        if (_controller)
        {
            _controller->AppendProperties(properties);
        }
    }

}
