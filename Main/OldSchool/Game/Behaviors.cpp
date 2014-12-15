#include "precomp.h"
#include "Contexts.h"
#include "Transform.h"
#include "Input.h"
#include "Behaviors.h"
#include "Entity.h"
#include "Weapon.h"
#include "GameWorld.h"
#include "InputHelper.h"

std::shared_ptr<PlayerControlledBehavior> PlayerControlledBehavior::Create()
{
    return std::shared_ptr<PlayerControlledBehavior>(new PlayerControlledBehavior);
}

PlayerControlledBehavior::PlayerControlledBehavior()
{
    _class = BehaviorClass::PlayerControlled;
}

_Use_decl_annotations_
void PlayerControlledBehavior::Update(const UpdateContext& context, std::shared_ptr<Entity> entity)
{
    // Determine motion drive and strafe intent. ( position/negative/nothing )
    float movementForce = (context.Input->IsKeyDown('S') ? -1.0f : context.Input->IsKeyDown('W') ? 1.0f : 0.0f );
    float strafeForce = (context.Input->IsKeyDown('A') ? -1.0f : context.Input->IsKeyDown('D') ? 1.0f : 0.0f );
    float gamePadMovementPos = 0.0f;
    float gamePadStrafePos = 0.0f;

    // Get gamepad positions
    context.Input->GetLeftThumbStickValue(&gamePadStrafePos, &gamePadMovementPos);
    movementForce += gamePadMovementPos;
    strafeForce += gamePadStrafePos;

    // Process rotation changes
    float yaw = 0.0f;
    float pitch = 0.0f;
    if (context.Input->IsKeyDown(VK_LEFT))
    {
        yaw -= 1.0f;
    }
    if (context.Input->IsKeyDown(VK_RIGHT))
    {
        yaw += 1.0f;
    }

    float gamePadYawPos = 0.0f;
    float gamePadPitchPos = 0.0f;

    // Get gamepad positions
    context.Input->GetRightThumbStickValue(&gamePadYawPos, &gamePadPitchPos);
    yaw += gamePadYawPos;
    pitch += gamePadPitchPos;

    MoveEntity(entity, context.ElapsedTime, movementForce, strafeForce, yaw);

    // Handle firing of weapons
    std::shared_ptr<Entity> target = entity->GetGameWorld()->GetEntityAsTarget(nullptr, entity, EntityClass::Any);
    if (FireButtonPressed(context.Input))
    {
        if (target != nullptr && IsEntityKillable(target))
        {
            AttackEntity(entity, target);
        }
        else
        {
            // You shot it, you wasted ammo...
            entity->GetWeapon()->Use(0);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

std::shared_ptr<PatrolBehavior> PatrolBehavior::Create()
{
    return std::shared_ptr<PatrolBehavior>(new PatrolBehavior);
}

PatrolBehavior::PatrolBehavior()
{
    _class = BehaviorClass::Patrol;
}

_Use_decl_annotations_
void PatrolBehavior::Update(const UpdateContext& context, std::shared_ptr<Entity> entity)
{
    std::shared_ptr<Entity> patrolDirChange = entity->GetGameWorld()->TriggerEntityIfIntersected(entity, EntityClass::PatrolDirChange);
    if (patrolDirChange)
    {
        Transform et = patrolDirChange->GetTransform();
        Vector4 o = entity->GetTransform().GetOrientationV();
        Transform transform = entity->GetTransform();
        if (o != et.GetOrientationV())
        {
            Vector3 pos = entity->GetTransform().GetPositionV();
            Vector3 newPos = et.GetPositionV();
            newPos.y = pos.y;

            transform.SetPosition(newPos);
        }
        transform.SetOrientation(et.GetOrientation());

        entity->SetTransform(transform);
    }
    MoveEntity(entity, context.ElapsedTime, 0.76f, 0.0f, 0.0f);
}

_Use_decl_annotations_
void MoveEntity(std::shared_ptr<Entity> entity, float elapsedTime, float movementForce, float strafeForce, float rotation)
{
    std::shared_ptr<GameWorld> gameWorld = entity->GetGameWorld();
    float decelSpeed = entity->GetSpeed(SpeedType::DecelerationSpeed);
    float accelSpeed = entity->GetSpeed(SpeedType::AccelerationSpeed);
    float speed = entity->GetSpeed(SpeedType::MovementSpeed);
    float strafeSpeed = entity->GetSpeed(SpeedType::StrafeSpeed);
    float maxSpeed = entity->GetSpeed(SpeedType::MaxSpeed);

    Vector3 velocity;

    Transform transform = entity->GetTransform();
    Vector3 pos = transform.GetPositionV();

    // If movement force has been applied, process it
    if (movementForce != 0.0f)
    {
        speed += movementForce * accelSpeed * elapsedTime;
        if(speed > maxSpeed)
        {
            speed = maxSpeed;
        }

        if (speed < -maxSpeed)
        {
            speed = -maxSpeed;
        }
    }

    // If strafe force has been applied, process it
    if (strafeForce != 0.0f)
    {
        strafeSpeed += strafeForce * accelSpeed * elapsedTime;
        if (strafeSpeed > maxSpeed)
        {
            strafeSpeed = maxSpeed;
        }

        if (strafeSpeed < -maxSpeed)
        {
            strafeSpeed = -maxSpeed;
        }
    }

    velocity += (transform.GetForwardV() * speed + transform.GetRightV() * strafeSpeed) * elapsedTime;
    gameWorld->ResolveMovement(entity, pos, velocity, entity->GetRadius());

    transform.SetPosition(pos);
    transform.Rotate(transform.GetUpV(), rotation * entity->GetSpeed(SpeedType::RotationSpeed) * elapsedTime);

    entity->SetTransform(transform);

    // Apply decelaration to the movement speed
    if (speed > 0)
    {
        speed -= decelSpeed * elapsedTime;
        if (speed < 0)
        {
            speed = 0;
        }
    }
    else if (speed < 0)
    {
        speed += decelSpeed * elapsedTime;
        if (speed > 0)
        {
            speed = 0;
        }
    }

    // Apply decelaration to the strafe speed
    if (strafeSpeed > 0)
    {
        strafeSpeed -= decelSpeed * elapsedTime;
        if (strafeSpeed < 0)
        {
            strafeSpeed = 0;
        }
    }
    else if (strafeSpeed < 0)
    {
        strafeSpeed += decelSpeed * elapsedTime;
        if (strafeSpeed > 0)
        {
            strafeSpeed = 0;
        }
    }

    // Persist the current movement and strafe speeds
    entity->SetSpeed(SpeedType::MovementSpeed, speed);
    entity->SetSpeed(SpeedType::StrafeSpeed, strafeSpeed);
}

_Use_decl_annotations_
void AttackEntity(std::shared_ptr<Entity> attacker, std::shared_ptr<Entity> target)
{
    std::shared_ptr<Weapon> weapon = attacker->GetWeapon();
    if (weapon == nullptr)
    {
        return;
    }

    uint32_t distance = (uint32_t)Vector3::Distance(attacker->GetTransform().GetPositionV(), target->GetTransform().GetPositionV());
    uint32_t damage = weapon->Use(distance);
    if (distance > weapon->GetMaxRange())
    {
        // No damage is taken because the target's weapon is out of range.
        return;
    }

    uint32_t health = target->GetHealth();
    if (damage >= health)
    {
        // If the damage applied from the weapon is more than the health left on the target
        // the target dies
        target->Die();
        return;
    }

    // Update the target's health
    target->SetHealth(health - damage);
}

_Use_decl_annotations_
bool IsEntityKillable(std::shared_ptr<Entity> target)
{
    EntityClass entityClass = target->GetClass();
    return (entityClass == EntityClass::Soldier);
}