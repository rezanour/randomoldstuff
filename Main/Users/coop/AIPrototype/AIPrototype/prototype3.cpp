#include "game.h"
#include "prototype3.h"

using namespace DirectX;

Prototype3::Prototype3()
{
}

Prototype3::~Prototype3()
{
    delete context.world;
}

void Prototype3::OnLoadContent()
{
    context.graphics = this;
    context.world = new GameWorld();
    context.world->Load(&context);
}

void Prototype3::OnUpdate(float elapsedTime)
{
    context.elapsedTime = elapsedTime;
    if (context.world)
    {
        context.world->Update(&context);
    }
    if (Keyboard::IsKeyJustPressed(VK_SPACE))
    {
        context.world->Load(&context);
    }
}

void Prototype3::OnDraw()
{
    if (context.world)
    {
        context.world->Draw(&context);
    }
}