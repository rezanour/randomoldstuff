#pragma once

struct IModel;
class AIBehavior;
class Weapon;
class GameWorld;
enum class WeaponClass;

enum class EntityClass
{
    Any,
    Player,
    Soldier,
    SS_Soldier,
    Dog,
    Door,
    Ammo,
    Health,
    CeilingLight,
    Treasure,
    PushWall,
    PatrolDirChange,
};

enum class SpeedType
{
    RotationSpeed,
    MovementSpeed,
    StrafeSpeed,
    MaxSpeed,
    AccelerationSpeed,
    DecelerationSpeed,
    MaxSpeedType,
};

enum class CollisionType
{
    None,
    Solid,
    Sensor
};

class Entity : public BaseObject<Entity>, public IWriteableObject
{
public:
    static std::shared_ptr<Entity> Create(_In_ EntityClass entityClass, _In_ const Transform& transform, _In_ const XMFLOAT3& radius, _In_ CollisionType collisionType);

    //
    // IReadonlyObject
    //

    const Transform& GetTransform() const override;
    AABB GetAABB() const override;

    uint32_t RegisterForAABBChangeNotifications(_In_ const std::function<void()>& callback) override;
    void UnregisterFromAABBChangeNotifications(_In_ uint32_t token) override;

    //
    // IWriteableObject (some of this interface is above under IReadonlyObject)
    //

    void SetTransform(_In_ const Transform& transform) override;
    void SendAABBChangeNotifications() override;

    //
    // Entity
    //

    CollisionType GetCollisionType() const;
    const XMFLOAT3& GetRadius() const;
    void Move(_In_ const XMFLOAT3& movement);

    void Update(_In_ const UpdateContext& context);

    bool IsDead();
    void Die();

    uint32_t GetHealth() const;
    void SetHealth(_In_ uint32_t health);
    bool AddHealth(_In_ uint32_t health);

    void SetTarget(_In_ const std::shared_ptr<Entity>& target);
    EntityClass GetClass() const;

    uint32_t GetScore() const;
    void AddScore(_In_ uint32_t score);

    void SetBehavior(_In_ const std::shared_ptr<AIBehavior>& behavior);
    const std::shared_ptr<AIBehavior>& GetBehavior() const;

    void SetSpeed(_In_ SpeedType speedType, _In_ float speed);
    float GetSpeed(_In_ SpeedType speedType) const;

    float GetMaxSightDistance() const;
    void SetMaxSightDistance(_In_ float distance);

    const std::shared_ptr<Weapon>& GetWeapon() const;
    void SwitchToWeapon(_In_ WeaponClass weapon);
    void AddWeapon(_In_ const std::shared_ptr<Weapon>& weapon);

    void SetModel(_In_ const std::shared_ptr<IModel>& model);
    const std::shared_ptr<IModel>& GetModel() const;

    float GetFieldOfView() const;

    void SetGameWorld(_In_ const std::shared_ptr<GameWorld>& world);
    std::shared_ptr<GameWorld> GetGameWorld();

private:
    Entity(_In_ EntityClass entityClass, _In_ const Transform& transform, _In_ const XMFLOAT3& radius, _In_ CollisionType collisionType);

private:
    EntityClass _class;
    Transform _transform;
    CollisionType _collisionType;
    XMFLOAT3 _radius;
    XMFLOAT3 _requestedMovement;

    float _fov;
    float _maxSightDistance;
    uint32_t _health;
    uint32_t _score;
    float _speeds[static_cast<uint32_t>(SpeedType::MaxSpeedType)];
    std::shared_ptr<IModel> _model;
    std::shared_ptr<Entity> _target;
    std::vector<std::shared_ptr<Weapon>> _weapons;
    std::shared_ptr<Weapon> _weapon;
    std::shared_ptr<AIBehavior> _behavior;
    std::weak_ptr<GameWorld> _gameWorld;

    uint32_t _nextCallbackToken;
    std::map<uint32_t, std::function<void()>> _aabbChangedCallbacks;
};
