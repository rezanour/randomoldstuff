#pragma once

#include "gamestates.h"

class Menu;
class GamePlay;
enum class MenuId;

class Game : public TrackedObject<MemoryTag::Game>
{
public:
    bool Tick();
    void SetState(_In_ GameState state);
    void ShowMenu(_In_ MenuId menu);
    void DismissMenu();

    void StartNewGame();
    void QuitGame();
    void Exit();

    bool IsPlaying()
    {
        return (_gamePlay != nullptr);
    }

    GamePlay& GetGamePlay()
    {
        // Nobody should call this outside of the lifetime of the gameplay.
        // If so, we should audit the caller
        assert(_gamePlay != nullptr);
        return *_gamePlay; 
    }

    AudioEngine& GetAudioEngine()
    {
        assert(_audioEngine != nullptr);
        return *_audioEngine;
    }

    Content& GetSystemContent()
    {
        return _systemContent;
    }

    const SpriteFont& GetDebugFont() const { return _debugFont; }

private:
    friend void GameStartup();
    Game();
    Game(const Game&);
    void OnGameStateChanged(_In_ GameState oldState, _In_ GameState newState);

private:
    LARGE_INTEGER _frequency;
    LARGE_INTEGER _lastTime;
    float _elapsedTimeHistory[10];
    uint32_t _elapsedTimeHistoryIndex;

    MenuId _menuId;
    std::unique_ptr<Menu> _menu;
    std::unique_ptr<GamePlay> _gamePlay;
    std::unique_ptr<AudioEngine> _audioEngine;
    Content _systemContent;
    GameState _gameState;
    SpriteFont _debugFont;
    bool _exit;
};

void GameStartup();
void GameShutdown();

Game& GetGame();
