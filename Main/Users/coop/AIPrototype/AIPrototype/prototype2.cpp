#include "game.h"
#include "prototype2.h"

using namespace DirectX;

Prototype2::Prototype2()
{
}

Prototype2::~Prototype2()
{
}

void Prototype2::OnLoadContent()
{
    arrowTexture = GetSprite(L"arrow.png");
    squareTexture = GetSprite(L"square.png");

    for (int i = 0; i < 10; i++)
    {
        std::shared_ptr<Entity> t = std::make_shared<TestEntity>();
        t->position = Vector2((float)(rand() % GetScreenWidth()), (float)(rand() % GetScreenHeight()));
        t->maxSpeed = 0.5f;
        t->turnSpeed = .05f;
        t->orientation = 0;
        t->currentSpeed = 0.0f;
        t->Initialize(arrowTexture);

        entities.push_back(t);
    }
}

void Prototype2::OnUpdate(float elapsedTime)
{
    UNREFERENCED_PARAMETER(elapsedTime);
    for (size_t i = 0; i < entities.size(); i++)
    {
        entities[i]->Update(elapsedTime);
    }
}

void Prototype2::OnDraw()
{
    Vector2 position(100);
    float orientation = 0.0f;

    for (size_t i = 0; i < entities.size(); i++)
    {
        entities[i]->Draw(GameTimeGetElapsed(), this);
        DrawSpriteWithOrigin(squareTexture, entities[i]->position.x, entities[i]->position.y,0,1, Colors::Yellow);
        //DrawSpriteWithOrigin(arrowTexture, entities[i]->position.x, entities[i]->position.y, entities[i]->orientation, 1, Colors::Orange);
    }
}