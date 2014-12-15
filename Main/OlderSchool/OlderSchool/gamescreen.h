#pragma once

class GameObject;

class GameScreen
{
public:
    //
    // Returns unique pointer. The lifetime of game screen is not meant to be shared.
    // The Game object always owns the lifetime of the screen. Everyone else accesses via raw pointer.
    //
    static std::unique_ptr<GameScreen> Create();

    void Update(_In_ float elapsedTime);
    void Draw();

private:
    GameScreen();

private:
    std::vector<std::shared_ptr<GameObject>> _gameObjects;
};
