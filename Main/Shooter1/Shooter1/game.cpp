#include "precomp.h"
#include "game.h"

static Game* g_game;

Game::Game() :
    _menuId(MenuId::Undefined),
    _exit(true),
    _elapsedTimeHistoryIndex(0)
{
    CHECKGLE(QueryPerformanceFrequency(&_frequency));
    CHECKGLE(QueryPerformanceCounter(&_lastTime));

    ShowMenu(MenuId::MainMenu);

    _audioEngine.reset(new AudioEngine(AudioEngine_Default));

    _debugFont = _systemContent.GetSpriteFont("font14.spritefont");

    ZeroMemory(_elapsedTimeHistory, sizeof(_elapsedTimeHistory));
}

void Game::Exit()
{
    _exit = false;
}

_Use_decl_annotations_
void Game::ShowMenu(MenuId menu)
{
    _menuId = menu;
}

void Game::DismissMenu()
{
    _menuId = MenuId::Undefined;
}

bool Game::Tick()
{
    LARGE_INTEGER now;
    CHECKGLE(QueryPerformanceCounter(&now));

    assert(now.QuadPart >= _lastTime.QuadPart);
    float elapsedTime = (float)((now.QuadPart - _lastTime.QuadPart) / (double)_frequency.QuadPart);

    _lastTime = now;

    GetAnimations().Update(elapsedTime);

    //
    // Use an average of the past 5 frames to compute FPS to give a less jumpy number.
    //
    _elapsedTimeHistory[_elapsedTimeHistoryIndex] = elapsedTime;
    _elapsedTimeHistoryIndex = (_elapsedTimeHistoryIndex + 1) % _countof(_elapsedTimeHistory);
    float totalElapsedTime = 0.0f;
    for (uint32_t i = 0; i < _countof(_elapsedTimeHistory); ++i)
    {
        totalElapsedTime += _elapsedTimeHistory[i];
    }
    _debugFont.DrawFullScreen(POINT { 0, 0 }, XMFLOAT4(1, 1, 1, 1), "FPS: %2.1f", _countof(_elapsedTimeHistory) / totalElapsedTime);

    // Update
    if (_gamePlay)
    {
        _gamePlay->Update(elapsedTime, (_menu == nullptr));
    }

    if (_audioEngine)
    {
        _audioEngine->Update();
    }

    if (_menu)
    {
        _menu->Update();
    }

    if ((_menu == nullptr && _menuId != MenuId::Undefined) || (_menu != nullptr && _menu->GetId() != _menuId))
    {
        _menu.reset();

        switch(_menuId)
        {
            case MenuId::MainMenu:
                _gamePlay.reset();
                _menu.reset(new MainMenu());
            break;

            case MenuId::GameMenu:
                _menu.reset(new GameMenu());
            break;

            case MenuId::OptionsMenu:
                _menu.reset(new OptionsMenu());
            break;

            case MenuId::AudioMenu:
                _menu.reset(new AudioMenu());
            break;

            case MenuId::ControlsMenu:
                _menu.reset(new ControlsMenu());
            break;

            case MenuId::HudMenu:
                _menu.reset(new HudMenu());
            break;

            case MenuId::GraphicsMenu:
                _menu.reset(new GraphicsMenu());
            break;

            default:
            break;
        }
    }

    // Draw
    if (_gamePlay)
    {
        _gamePlay->Draw();
    }

    if (_menu)
    {
        _menu->Draw();
    }

    return _exit;
}

_Use_decl_annotations_
void Game::SetState(GameState state)
{
    if (state != _gameState)
    {
        GameState oldState = _gameState;
        _gameState = state;

        // Game state has changed
        OnGameStateChanged(oldState, state);
    }
}

void Game::StartNewGame()
{
    _gamePlay.reset(new GamePlay());
    DismissMenu();
}

void Game::QuitGame()
{
    _gamePlay = nullptr;
    ShowMenu(MenuId::MainMenu);
}

_Use_decl_annotations_
void Game::OnGameStateChanged(GameState oldState, GameState newState)
{
    UNREFERENCED_PARAMETER(oldState);
    UNREFERENCED_PARAMETER(newState);
}

void GameStartup()
{
    assert(!g_game);

    delete g_game;

    g_game = new Game();
    g_game->SetState(GameState::Starting);
    DebugOut("Game initialized.\n");
}

void GameShutdown()
{
    assert(g_game);

    g_game->SetState(GameState::Exiting);

    delete g_game;
    g_game = nullptr;

    DebugOut("Game shutdown.\n");
}

Game& GetGame()
{
    assert(g_game);
    return *g_game;
}
