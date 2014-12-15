#include "Precomp.h"
#include "Transform.h"
#include "Contexts.h"
#include "Entity.h"
#include "Weapon.h"
#include "Model.h"
#include "Behaviors.h"

_Use_decl_annotations_
std::shared_ptr<Entity> Entity::Create(EntityClass entityClass, const Transform& transform, const XMFLOAT3& radius, CollisionType collisionType)
{
    return std::shared_ptr<Entity>(new Entity(entityClass, transform, radius, collisionType));
}

_Use_decl_annotations_
Entity::Entity(EntityClass entityClass, const Transform& transform, const XMFLOAT3& radius, CollisionType collisionType) :
    _class(entityClass), _transform(transform), _radius(radius), _collisionType(collisionType),
    _health(99999), _fov(60), _maxSightDistance(100), _nextCallbackToken(1)
{
    ZeroMemory(_speeds, sizeof(_speeds));
    SetSpeed(SpeedType::RotationSpeed, 2.5f);
    SetSpeed(SpeedType::MaxSpeed, 20.0f);
    SetSpeed(SpeedType::AccelerationSpeed, 100.0f);
    SetSpeed(SpeedType::DecelerationSpeed, 75.0f);
}

const Transform& Entity::GetTransform() const
{
    return _transform;
}

AABB Entity::GetAABB() const
{
    return AABB(_transform.GetPositionV(), XMLoadFloat3(&_radius));
}

_Use_decl_annotations_
uint32_t Entity::RegisterForAABBChangeNotifications(const std::function<void()>& callback)
{
    uint32_t token = _nextCallbackToken++;
    _aabbChangedCallbacks[token] = callback;
    return token;
}

_Use_decl_annotations_
void Entity::UnregisterFromAABBChangeNotifications(uint32_t token)
{
    auto it = _aabbChangedCallbacks.find(token);
    if (it != std::end(_aabbChangedCallbacks))
    {
        _aabbChangedCallbacks.erase(it);
    }
}

_Use_decl_annotations_
void Entity::SetTransform(const Transform& transform)
{
    _transform = transform;
    SendAABBChangeNotifications();
}

void Entity::SendAABBChangeNotifications()
{
    for (auto& pair : _aabbChangedCallbacks)
    {
        pair.second();
    }
}

CollisionType Entity::GetCollisionType() const
{
    return _collisionType;
}

const XMFLOAT3& Entity::GetRadius() const
{
    return _radius;
}

void Entity::Move(_In_ const XMFLOAT3& movement)
{
    _requestedMovement.x += movement.x;
    _requestedMovement.y += movement.y;
    _requestedMovement.z += movement.z;
}

_Use_decl_annotations_
void Entity::Update(const UpdateContext& context)
{
    if (_behavior)
    {
        _behavior->Update(context, this->shared_from_this());
    }

    // Process movement
    CollisionQuery query;
    query.WorldPosition = _transform.GetPositionV();
    query.WorldMovement = XMLoadFloat3(&_requestedMovement);
    query.eRadius = XMLoadFloat3(&_radius);

    // Convert to Ellipsoid Space
    query.BasePoint = query.WorldPosition / query.eRadius;
    query.Velocity = query.WorldMovement / query.eRadius;
    query.VelocityLen2 = XMVector3LengthSq(query.Velocity);
    query.Collided = false;

    // TODO: loop for collision & resolution

    // Convert back into World Space
    query.WorldPosition = query.BasePoint * query.eRadius;
    query.WorldMovement = query.Velocity * query.eRadius;

    _transform.SetPosition(query.WorldPosition);

    // Clear out movement for next frame
    XMStoreFloat3(&_requestedMovement, XMVectorZero());
}

const std::shared_ptr<Weapon>& Entity::GetWeapon() const
{
    return _weapon;
}

const std::shared_ptr<AIBehavior>& Entity::GetBehavior() const
{
    return _behavior;
}

_Use_decl_annotations_
void Entity::AddWeapon(const std::shared_ptr<Weapon>& weapon)
{
    bool newWeapon = true;
    for (auto w : _weapons)
    {
        if (w->GetClass() == weapon->GetClass())
        {
            w->AddAmmo(weapon->GetAmmoCount());
            newWeapon = false;
            break;
        }
    }

    // Add the weapon if it is a new weapon.  Also
    // as an added bonus, auto-switch to the new weapon so
    // the entity can try it out!
    if (newWeapon)
    {
        _weapons.push_back(weapon);
        SwitchToWeapon(weapon->GetClass());
    }
}

EntityClass Entity::GetClass() const
{
    return _class;
}

float Entity::GetFieldOfView() const
{
    return _fov;
}

float Entity::GetMaxSightDistance() const
{
    return _maxSightDistance;
}

const std::shared_ptr<IModel>& Entity::GetModel() const
{
    return _model;
}

_Use_decl_annotations_
void Entity::SetHealth(uint32_t health)
{
    _health = health;
}

_Use_decl_annotations_
bool Entity::AddHealth(uint32_t health)
{
    if (_health >= 100)
    {
        return false;
    }

    _health += health;
    if (_health > 100)
    {
        _health = 100;
    }

    return true;
}

uint32_t Entity::GetScore() const
{
    return _score;
}

_Use_decl_annotations_
void Entity::AddScore(uint32_t score)
{
    _score += score;
}

_Use_decl_annotations_
void Entity::SetBehavior(const std::shared_ptr<AIBehavior>& behavior)
{
    _behavior = behavior;
}

_Use_decl_annotations_
void Entity::SetTarget(const std::shared_ptr<Entity>& target)
{
    _target = target;
}

bool Entity::IsDead()
{
    return !(_health > 0);
}

void Entity::Die( )
{ 
    _health = 0;
}

_Use_decl_annotations_
void Entity::SwitchToWeapon(WeaponClass weapon)
{
    for (auto w : _weapons)
    {
        if (w->GetClass() == weapon)
        {
            _weapon = w;
            break;
        }
    }
}

_Use_decl_annotations_
void Entity::SetSpeed(SpeedType speedType, float speed)
{
    uint32_t i = static_cast<uint32_t>(speedType);
    assert(i < static_cast<uint32_t>(SpeedType::MaxSpeedType));
    _speeds[i] = speed;
}

_Use_decl_annotations_
float Entity::GetSpeed(SpeedType speedType) const
{
    uint32_t i = static_cast<uint32_t>(speedType);
    assert(i < static_cast<uint32_t>(SpeedType::MaxSpeedType));
    return _speeds[i];
}

uint32_t Entity::GetHealth() const
{
    return _health;
}

_Use_decl_annotations_
void Entity::SetMaxSightDistance(float distance)
{
    _maxSightDistance = distance;
}

_Use_decl_annotations_
void Entity::SetModel(const std::shared_ptr<IModel>& model)
{
    if (_model != nullptr)
    {
        _model->SetOwner(nullptr);
    }

    _model = model;

    if (_model != nullptr)
    {
        _model->SetOwner(shared_from_this());
    }
}

_Use_decl_annotations_
void Entity::SetGameWorld(const std::shared_ptr<GameWorld>& world)
{
    _gameWorld = world;
}

std::shared_ptr<GameWorld> Entity::GetGameWorld()
{
    return _gameWorld.lock();
}
