#include "precomp.h"
#include "game.h"
#include "prototype4.h"

using namespace DirectX;

Prototype4::Prototype4()
{
}

Prototype4::~Prototype4()
{
    delete context.world;
}

void Prototype4::OnLoadContent()
{
    context.graphics = this;
    context.world = new GameWorld();
    context.iworld = context.world;
    context.world->Load(&context);
}

void Prototype4::OnUpdate(float elapsedTime)
{
    context.elapsedTime = elapsedTime;
    if (context.world)
    {
        context.world->Update(&context);
    }
    if (Keyboard::IsKeyJustPressed(VK_RETURN))
    {
        context.world->Load(&context);
    }
}

void Prototype4::OnDraw()
{
    if (context.world)
    {
        context.world->Draw(&context);
    }
}