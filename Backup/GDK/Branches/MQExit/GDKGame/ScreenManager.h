// ScreenManager is responsible for managing the loading and activating/deactivating of 'screens', the building blocks of the game.
#pragma once

#include "Screen.h"
#include "ContentManager.h"

namespace GDK
{
    class GameApplication;

    // not ref counted, only the game application holds this
    class ScreenManager : stde::non_copyable
    {
    public:
        explicit ScreenManager(_In_ GameApplication* pApplication);
        ~ScreenManager();

        HRESULT StartLoadingScreen(_In_ uint64 contentId);
        HRESULT ChangeScreen(_In_ uint64 contentId);
        HRESULT RemoveScreen(_In_ uint64 contentId);

        HRESULT Update();

        void ExitApplication();

        GDK::IRenderer* GetRenderer() const;
        ContentManagerPtr GetContentManager() const;

    private:
        GameApplication* _pApplication;
        ContentManagerPtr _spContentManager;

        typedef std::map<uint64, ScreenPtr> ScreenMap;
        ScreenMap _screens;
        ScreenPtr _activeScreen;
    };
}
