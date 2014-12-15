#pragma once

#include <GDK.h>
#include <Game.h>
#include <GameObject.h>
#include <DeviceContext.h>
#include <RuntimeObject.h>
#include <ContentCache.h>

namespace Quake2
{
    class MainMenu;
    class Hud;

    enum class QuakeKey
    {
        MoveLeft,
        MoveRight,
        TurnLeft,
        TurnRight,
        MoveForward,
        MoveBackward,
        Fire,
        Jump,
        Menu,
        QuickQuit,
        MenuUp,
        MenuDown,
        MenuSelect,
        Yes,
        No,
        SelectBlaster,
        SelectShotgun,
        SelectSuperShotgun,
        SelectMachinegun,
        SelectChaingun,
        SelectGrenadeLauncher,
        SelectRocketLauncher,
        SelectHyperBlaster,
        SelectRailgun,
        SelectBFG,
        TestIt,
    };

    enum class GameControllerType
    {
        Player,
        Monster,
        Item,
        Trigger
    };

    class Quake2Game : 
        public GDK::RuntimeObject<Quake2Game>,
        public GDK::IGame
    {
    public:
        static void InitializeGameInfo();
        static std::shared_ptr<GDK::ContentCache> GetContentCache();
        static std::shared_ptr<GDK::ContentCache> _content;

        // Creation
        static std::shared_ptr<GDK::IGame> OnCreate(_In_ const GDK::DeviceContext& deviceContext);

        // IGame
        virtual void OnWorldChanged(_In_ const std::shared_ptr<GDK::IGameWorld>& gameWorld) override;
        virtual GDK::UpdateResult OnUpdateBegin(_In_ const GDK::GameTime& gameTime) override;
        virtual GDK::UpdateResult OnUpdateEnd(_In_ const GDK::GameTime& gameTime) override;
        virtual void OnDrawBegin(_Out_ GDK::Matrix* view, _Out_ GDK::Matrix* projection) override;
        virtual void OnDrawEnd(_In_ const GDK::Matrix& view, _In_ const GDK::Matrix& projection) override;

    private:
        Quake2Game(_In_ const GDK::DeviceContext& deviceContext);

        GDK::DeviceContext _deviceContext;
        std::shared_ptr<MainMenu> _mainMenu;
        std::shared_ptr<Hud> _hud;
        std::shared_ptr<GDK::IGameObject> _player;

        int _testIndex;
    };
}
