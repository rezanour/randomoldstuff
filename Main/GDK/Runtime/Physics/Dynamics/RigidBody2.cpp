#include <GameObject.h>
#include <CollisionPrimitives.h>
#include "..\RigidBody.h"
#include "..\PhysicsWorld.h"

namespace GDK
{
    std::shared_ptr<RigidBody> RigidBody::Create(_In_ const std::shared_ptr<PhysicsWorld>& world, _In_ const std::shared_ptr<IGameObject>& owner, _In_ const RigidBodyCreateParameters& parameters)
    {
        std::shared_ptr<RigidBody> body(GDKNEW RigidBody(world, owner, parameters));

        if (body->_shape)
        {
            Vector3 aabbMin, aabbMax;
            GetAabbForPrimitive(body->_worldShape.get(), &aabbMin, &aabbMax);
            body->_proxy = world->GetBroadphase()->CreateProxy(body, aabbMin, aabbMax);
        }

        return body;
    }

    RigidBody::RigidBody(_In_ const std::shared_ptr<PhysicsWorld>& world, _In_ const std::shared_ptr<IGameObject>& owner, _In_ const RigidBodyCreateParameters& parameters) :
        _world(world), _owner(owner), _velocity(parameters.initialVelocity), _rotation(0),
        _invMass(1.0f / parameters.mass), _restitution(parameters.restitution), _gravityScale(parameters.gravityScale),
        _shape(CopyPrimitive(parameters.shape)), _worldShape(CopyPrimitive(parameters.shape)), _type(parameters.type)
    {
        _acceleration = world->GetGravity() * _gravityScale;
        SetPosition(parameters.initialPosition); // force updates the world shape
    }

    RigidBody::~RigidBody()
    {
        auto world = _world.lock();
        if (world)
        {
            auto bp = world->GetBroadphase();
            if (bp)
            {
                bp->DestroyProxy(_proxy);
            }
        }
    }

    std::shared_ptr<IGameObject> RigidBody::GetOwner() const
    {
        return _owner.lock();
    }

    float RigidBody::GetMass() const
    {
        return 1.0f / _invMass;
    }

    float RigidBody::GetRestitution() const
    {
        return _restitution;
    }

    float RigidBody::GetInvMass() const
    {
        return (_type == RigidBodyType::Kinematic) ? 0.0f : _invMass;
    }

    const Vector3& RigidBody::GetPosition() const
    {
        return _position;
    }

    const Vector3& RigidBody::GetVelocity() const
    {
        return _velocity;
    }

    const Vector3& RigidBody::GetAcceleration() const
    {
        return _acceleration;
    }

    void RigidBody::SetPosition(_In_ const Vector3& value)
    {
        _position = value;

        Matrix transform(Matrix::CreateRotationY(_rotation));
        transform.SetTranslation(_position);
        TransformPrimitive(_shape.get(), &transform, _worldShape.get());

        auto proxy = _proxy.lock();
        if (_shape && proxy)
        {
            Vector3 aabbMin, aabbMax;
            GetAabbForPrimitive(_worldShape.get(), &aabbMin, &aabbMax);
            proxy->SetAabb(aabbMin, aabbMax);
        }
    }

    void RigidBody::SetRotation(_In_ float value)
    {
        _rotation = value;

        Matrix transform(Matrix::CreateRotationY(_rotation));
        transform.SetTranslation(_position);
        TransformPrimitive(_shape.get(), &transform, _worldShape.get());

        auto proxy = _proxy.lock();
        if (_shape && proxy)
        {
            Vector3 aabbMin, aabbMax;
            GetAabbForPrimitive(_worldShape.get(), &aabbMin, &aabbMax);
            proxy->SetAabb(aabbMin, aabbMax);
        }
    }

    void RigidBody::AddImpulse(_In_ const Vector3& value)
    {
        _impulses += value;
    }

    void RigidBody::SetGravityScale(_In_ float value)
    {
        _gravityScale = value;
        _acceleration = _world.lock()->GetGravity() * _gravityScale;
    }

    RigidBodyType RigidBody::GetType() const
    {
        return _type;
    }

    bool RigidBody::IsSensor() const
    {
        return _type == RigidBodyType::Sensor;
    }

    bool RigidBody::IsKinematic() const
    {
        return _type == RigidBodyType::Kinematic;
    }

    const std::vector<GameObjectContact>& RigidBody::GetContacts() const
    {
        return _contacts;
    }

    void RigidBody::Integrate(_In_ float deltaSeconds)
    {
        if (_type == RigidBodyType::Normal)
        {
            _velocity += _acceleration * deltaSeconds + _impulses;
            _impulses = Vector3::Zero();

            _velocity *= 0.95f; // fake "drag"
            _velocity *= powf(0.95f, deltaSeconds); // fake "damping" for numeric stability
            _position += _velocity * deltaSeconds;
        }
    }

    void RigidBody::ClearContacts()
    {
        _contacts.clear();
    }

    void RigidBody::BeginResolution()
    {
        _positionAdjustments = Vector3::Zero();
    }

    void RigidBody::EndResolution()
    {
        ClearPositionAdjustments();
    }

    void RigidBody::AdjustVelocity(_In_ const Vector3& value)
    {
        _velocity += value;
    }

    void RigidBody::AdjustPosition(_In_ const Vector3& value)
    {
        _position += value;
        _positionAdjustments += value;
    }

    const Vector3& RigidBody::GetPositionAdjustments() const
    {
        return _positionAdjustments;
    }

    void RigidBody::ClearPositionAdjustments()
    {
        _positionAdjustments = Vector3::Zero();
    }

    const std::shared_ptr<CollisionPrimitive>& RigidBody::GetWorldShape() const
    {
        return _worldShape;
    }

    void RigidBody::OnContact(_In_ const GameObjectContact& contact)
    {
        _contacts.push_back(contact);
    }
}
