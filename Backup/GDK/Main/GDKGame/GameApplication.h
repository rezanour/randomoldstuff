// GameApplication is the core app model for the runtime game
#pragma once

#include "ContentManager.h"

namespace GDK
{
    class ScreenManager;
    class SubsystemManager;

    // Not a ref counted type, there should only ever be one and it's not passed around
    class GameApplication : stde::non_copyable
    {
    public:
        explicit GameApplication(_In_ CoreServices::ConfigurationPtr& spConfiguration);
        ~GameApplication();

        HRESULT Run();
        void Exit();

        const SubsystemManager* GetSubsystemManager() const { return _spSubsystemManager.get(); }
        ContentManagerPtr GetContentManager() const { return _spContentManager; }

    private:
        HRESULT Initialize();
        void Shutdown();

        HRESULT OnIdle();

        HRESULT FirstUpdate();
        HRESULT RunGame();

        static LRESULT CALLBACK WindowProc(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);

        HWND _hwnd;
        CoreServices::ConfigurationPtr _spConfiguration;
        std::unique_ptr<ScreenManager> _spScreenManager;
        std::unique_ptr<SubsystemManager> _spSubsystemManager;
        ContentManagerPtr _spContentManager;

#ifdef BUILD_TESTS
        friend bool RunAndExitTest();
#endif
    };
}
