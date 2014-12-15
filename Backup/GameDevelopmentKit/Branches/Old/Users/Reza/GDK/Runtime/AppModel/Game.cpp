#include <Game.h>

namespace GDK
{
    Game::Game(_In_ const std::wstring& name, _In_ const GameCreationParameters& parameters) :
        _name(name),
        _content(parameters.Content),
        _graphicsDevice(parameters.GraphicsDevice),
        _audioDevice(parameters.AudioDevice),
        _inputDevice(parameters.InputDevice)
    {
    }

    const std::wstring& Game::GetName() const
    {
        return _name;
    }

    void Game::Update(_In_ double elapsedSeconds)
    {
        OnBeginUpdate(elapsedSeconds);
        OnEndUpdate(elapsedSeconds);
    }

    void Game::Draw()
    {
    }

    void Game::OnBeginUpdate(_In_ double elapsedSeconds)
    {
        UNREFERENCED_PARAMETER(elapsedSeconds);
    }

    void Game::OnEndUpdate(_In_ double elapsedSeconds)
    {
        UNREFERENCED_PARAMETER(elapsedSeconds);
    }
}
