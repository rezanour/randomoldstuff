#include "RuntimeGame.h"
#include "RuntimeGameWorld.h"
#include <AudioDevice.h>
#include <GameWorldContent.h>
#include <GameTime.h>
#include <UpdateResult.h>
#include <GDKError.h>
#include <Content.h>
#include <InputInternal.h>

namespace GDK
{
    static GameInfo g_gameInfo;
    static bool g_gameActive = false;

    namespace Game
    {
        void RegisterGame(_In_ const GameInfo& gameInfo)
        {
            CHECK_FALSE(g_gameActive);

            g_gameInfo = gameInfo;
        }

        std::shared_ptr<IRunningGame> Create(_In_ const DeviceContext& deviceContext)
        {
            CHECK_NOT_NULL(g_gameInfo.onCreate);
            return RuntimeGame::Create(deviceContext);
        }

        std::shared_ptr<IRunningGame> Create(_In_ const DeviceContext& deviceContext, _In_ const std::shared_ptr<GameWorldContent>& initialWorld)
        {
            CHECK_NOT_NULL(g_gameInfo.onCreate);
            return RuntimeGame::Create(deviceContext, initialWorld);
        }
    }

    std::shared_ptr<IRunningGame> RuntimeGame::Create(_In_ const DeviceContext& deviceContext)
    {
        std::shared_ptr<RuntimeGame> game(GDKNEW RuntimeGame(deviceContext));
        game->LoadWorld(Content::LoadGameWorldContent(g_gameInfo.firstWorld));
        return game;
    }

    std::shared_ptr<IRunningGame> RuntimeGame::Create(_In_ const DeviceContext& deviceContext, _In_ const std::shared_ptr<GameWorldContent>& initialWorld)
    {
        std::shared_ptr<RuntimeGame> game(GDKNEW RuntimeGame(deviceContext));
        game->LoadWorld(initialWorld);
        return game;
    }

    RuntimeGame::RuntimeGame(_In_ const DeviceContext& deviceContext) :
        _deviceContext(deviceContext), _game(g_gameInfo.onCreate(deviceContext))
    {
    }

    RuntimeGame::~RuntimeGame()
    {
        if (_deviceContext.audioDevice)
        {
            _deviceContext.audioDevice->StopAllLoopingClips();
        }
    }

    const std::wstring& RuntimeGame::GetName() const
    {
        return g_gameInfo.name;
    }

    UpdateResult RuntimeGame::Update(_In_ const GameTime& gameTime)
    {
        Input::Tick();
        _deviceContext.audioDevice->Tick();

        UpdateResult result = UpdateResult::Continue;

        result = _game->OnUpdateBegin(gameTime);

        if (result == UpdateResult::Continue)
        {
            _world->Update(gameTime);

            result = _game->OnUpdateEnd(gameTime);
        }

        return result;
    }

    void RuntimeGame::Draw()
    {
        Matrix view, projection;

        _game->OnDrawBegin(&view, &projection);

        _world->Draw(view, projection);

        _game->OnDrawEnd(view, projection);
    }

    void RuntimeGame::LoadWorld(_In_ const std::shared_ptr<GameWorldContent>& nextWorld)
    {
        _world = RuntimeGameWorld::Create(nextWorld, _deviceContext, false);
        _game->OnWorldChanged(_world);
    }
}
