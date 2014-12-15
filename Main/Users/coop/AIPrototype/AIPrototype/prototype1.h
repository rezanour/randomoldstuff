#pragma once

typedef enum AiState
{
    Idle,
    Chasing,
    Caught,
    Evading,
    Wander
};

class TestObject : public GameObject
{
public:
    TestObject(float x, float y, float width, float height) :
        GameObject(x,y,width,height)
    {
        position.x = x;
        position.y = y;
        orientation = (float)(rand() % 3);
        wanderDirection.x = (float)(rand() % 100);
        wanderDirection.y = (float)(rand() % 100);
        speed = 0.0f;
    };
    Vector2 position;
    AiState state;
    Vector2 wanderDirection;
    float speed;
    float maxSpeed;
};

class Prototype1 : public Game
{
public:
    Prototype1();
    ~Prototype1();
    void OnLoadContent();
    void OnUpdate(float elapsedTime);
    void OnDraw();
    void UpdateBlackTestObject(float elapsedTime);
    void UpdateTestObjects(float elapsedTime);
    void UpdateGreenTestObject(float elapsedTime);
    void Wander(Vector2 position, Vector2& wanderDirection,
            float& orientation, float turnSpeed);

    std::vector<std::shared_ptr<TestObject> > objects;
    std::shared_ptr<TestObject> blackTestObject;
    std::shared_ptr<TestObject> greenTestObject;

    std::shared_ptr<Sprite> arrowTexture;
    std::shared_ptr<Sprite> squareTexture;


};
