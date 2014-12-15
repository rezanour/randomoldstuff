#pragma once

class GameScreen;

//
// The game is the root of most gameplay related components.
// NOTE: It is expected that the subsystems are all initialized by the
// time the Game object is created.
//
class Game
{
public:
    //
    // Returns a unique pointer. Only WinMain controls this object's lifetime.
    //
    static std::unique_ptr<Game> Create();
    ~Game();

    void Tick();

private:
    Game();
    Game(const Game&);
    Game& operator=(const Game&);

private:
    std::unique_ptr<GameScreen> _screen;

    LARGE_INTEGER _frequency;
    LARGE_INTEGER _lastTime;
};
