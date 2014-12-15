#include "precomp.h"
#include "game.h"
#include "graphics.h"
#include "gamescreen.h"

std::unique_ptr<Game> Game::Create()
{
    std::unique_ptr<Game> game(new Game());

    // Set global
    Globals::Game = game.get();

    return game;
}

Game::Game() :
    _screen(GameScreen::Create())
{
    QueryPerformanceFrequency(&_frequency);
    QueryPerformanceCounter(&_lastTime);
}

Game::~Game()
{
    // Remove global
    Globals::Game = nullptr;
}

void Game::Tick()
{
    //
    // Compute elapsed time
    //
    LARGE_INTEGER time;
    QueryPerformanceCounter(&time);

    float elapsed = (float)((time.QuadPart - _lastTime.QuadPart) / (double)_frequency.QuadPart);

    //
    // Update
    //
    _screen->Update(elapsed);

    //
    // Draw
    //
    Globals::Graphics->Clear(XMFLOAT4(0, 0, 0, 1));

    _screen->Draw();

    Globals::Graphics->Present();
}
