#pragma once

class Entity;

enum class BehaviorClass
{
    PlayerControlled,
    Patrol,
    Attack,
    Pursue,
    Flee
};

void AttackEntity(_In_ std::shared_ptr<Entity> attacker, _In_ std::shared_ptr<Entity> target);
void MoveEntity(_In_ std::shared_ptr<Entity> entity, _In_ float elapsedTime, _In_ float movementForce, _In_ float strafeForce, _In_ float rotation);
bool IsEntityKillable(_In_ std::shared_ptr<Entity> target);

class AIBehavior : public BaseObject<AIBehavior>
{
public:
    virtual void Update(_In_ const UpdateContext& context, _In_ std::shared_ptr<Entity> entity) = 0;
    BehaviorClass GetClass() { return _class; }

protected:
    BehaviorClass _class;
};

class PlayerControlledBehavior : public AIBehavior
{
public:
    static std::shared_ptr<PlayerControlledBehavior> Create();
    void Update(_In_ const UpdateContext& context, _In_ std::shared_ptr<Entity> entity) override;

private:
    PlayerControlledBehavior();
};

class PatrolBehavior : public AIBehavior
{
public:
    static std::shared_ptr<PatrolBehavior> Create();
    void Update(_In_ const UpdateContext& context, _In_ std::shared_ptr<Entity> entity) override;

private:
    PatrolBehavior();
    std::shared_ptr<Entity> _lastVisited;
};
