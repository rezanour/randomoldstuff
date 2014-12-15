#pragma once

#include <Platform.h>
#include <RuntimeObject.h>
#include <RunningGame.h>
#include <Game.h>
#include <DeviceContext.h>

namespace GDK
{
    class RuntimeGameWorld;
    struct IGame;

    class RuntimeGame : 
        public RuntimeObject<RuntimeGame>,
        public IRunningGame
    {
    public:
        ~RuntimeGame();

        static std::shared_ptr<IRunningGame> Create(_In_ const DeviceContext& deviceContext);
        static std::shared_ptr<IRunningGame> Create(_In_ const DeviceContext& deviceContext, _In_ const std::shared_ptr<GameWorldContent>& initialWorld);

        // IRunningGame
        virtual const std::wstring& GetName() const override;
        virtual UpdateResult Update(_In_ const GameTime& gameTime) override;
        virtual void Draw() override;

    private:
        RuntimeGame(_In_ const DeviceContext& deviceContext);

        void LoadWorld(_In_ const std::shared_ptr<GameWorldContent>& nextWorld);

        DeviceContext _deviceContext;
        std::shared_ptr<IGame> _game;
        std::shared_ptr<RuntimeGameWorld> _world;
    };
}
