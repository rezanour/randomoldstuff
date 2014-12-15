#pragma once

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

typedef enum AI_State
{
    AI_Idle,
    AI_Wander
};

class Entity;

class Behavior
{
public:
    virtual void Update(_In_ float elapsedTime, Entity* entity) = 0;
};

class Entity
{
public:
    void Initialize(_In_ std::shared_ptr<Sprite> texture)
    {
        this->texture = texture;
    }

    void Update(_In_ float elapsedTime) 
    { 
        ChooseBehavior(elapsedTime);

        if (behavior != nullptr)
        {
            behavior->Update(elapsedTime, this);
        }

        Vector2 heading = Vector2((float)cos(orientation), (float)sin(orientation));

        position += heading * currentSpeed;
        position = ClampToLevelBoundary(position);
    }

    void Draw(_In_ float elapsedTime, Game* game)
    {
        game->DrawSpriteWithOrigin(texture, position.x, position.y,orientation,1, Colors::Yellow);
    }

    Vector2 ClampToLevelBoundary(Vector2 vector)
    {
        vector.x = Clamp(vector.x, 0, (float)640);
        vector.y = Clamp(vector.y, 0, (float)480);
        return vector;
    }

    virtual void ChooseBehavior(_In_ float elapsedTime) { UNREFERENCED_PARAMETER(elapsedTime); }
    void SetAIState(AI_State state)
    {
        prevAiState = aiState;
        aiState = state;
    }

    std::shared_ptr<Sprite> texture;
    float maxSpeed;
    float turnSpeed;
    Vector2 position;
    float orientation;
    float currentSpeed;
    AI_State aiState;
    AI_State prevAiState;
    std::shared_ptr<Behavior> behavior;
};

class WanderBehavior : public Behavior
{
public:
    WanderBehavior()
    {

    }
    void Update(_In_ float elapsedTime, Entity* entity)
    {
        entity->currentSpeed = 0.05f;

        wanderDirection.x += Lerp(-.25f, .25f, (float)rand());
        wanderDirection.y += Lerp(-.25f, .25f, (float)rand());

        Vector2 zeroV(0.0f);
        if (wanderDirection != zeroV)
        {
            wanderDirection.Normalize();
        }

        entity->orientation = TurnToFace(entity->position, entity->position + wanderDirection, entity->orientation,
            .15f * entity->turnSpeed);

        Vector2 screenCenter((float)640 / 2.0f, (float)480 / 2.0f);

        float distanceFromScreenCenter = Vector2::Distance(screenCenter, entity->position);
        float MaxDistanceFromScreenCenter = min(screenCenter.y, screenCenter.x);
        float normalizedDistance = distanceFromScreenCenter / MaxDistanceFromScreenCenter;
        float turnToCenterSpeed = .3f * normalizedDistance * normalizedDistance * entity->turnSpeed;

        entity->orientation = TurnToFace(entity->position, screenCenter, entity->orientation, turnToCenterSpeed);
    }

    Vector2 wanderDirection;
};

class IdleBehavior : public Behavior
{
public:
    IdleBehavior()
    {
        lookSwitch = true;
        lookTimer = 0.0f;
    }

    void Update(_In_ float elapsedTime, Entity* entity)
    {
        if (entity->prevAiState != AI_State::AI_Idle)
        {
            entity->prevAiState = AI_State::AI_Idle;
            lookSwitch = true;
            lookTimer = 0.0f;
        }

        entity->currentSpeed = 0.0f;

        lookTimer += elapsedTime;
        if (lookTimer > .5f)
        {
            lookSwitch = !lookSwitch;
            lookTimer = 0.0f;
        }

        if (lookSwitch)
        {
            entity->orientation -= .001f;
        }
        else
        {
            entity->orientation += .001f;
        }
    }

    Vector2 searchDirection;
    float lookTimer;
    bool lookSwitch;
};

class TestEntity : public Entity
{
public:
    TestEntity()
    {
        aiState = AI_State::AI_Idle;

        wanderBehavior = std::make_shared<WanderBehavior>();
        wanderBehavior->wanderDirection = Vector2(0,0);
        idleBehavior = std::make_shared<IdleBehavior>();

        stateTimer = 0.0f;
        behavior = idleBehavior;
        stateSwitchTime = rand() % 6;
    }

    void ChooseBehavior(_In_ float elapsedTime)
    {
        stateTimer += elapsedTime;
        if (stateTimer > stateSwitchTime)
        {
            stateTimer = 0.0f;
            if (aiState == AI_State::AI_Wander)
            {
                SetAIState(AI_State::AI_Idle);
                behavior = idleBehavior;
            }
            else
            {
                SetAIState(AI_State::AI_Wander);
                behavior = wanderBehavior;
            }
        }
    }

    std::shared_ptr<WanderBehavior> wanderBehavior;
    std::shared_ptr<IdleBehavior> idleBehavior;
    float stateTimer;
    float stateSwitchTime;
};

class Prototype2 : public Game
{
public:
    Prototype2();
    ~Prototype2();
    void OnLoadContent();
    void OnUpdate(float elapsedTime);
    void OnDraw();

    std::shared_ptr<Sprite> arrowTexture;
    std::shared_ptr<Sprite> squareTexture;
    std::vector<std::shared_ptr<Entity>> entities;
};