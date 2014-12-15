#pragma once

#include <GDK.h>
#include <RuntimeObject.h>
#include <ContentCache.h>

namespace Quake2
{
    class MainMenu : public GDK::RuntimeObject<MainMenu>
    {
    public:
        static std::shared_ptr<MainMenu> Create(const GDK::DeviceContext& deviceContext);

        bool Update(_In_ const GDK::GameTime& gameTime);
        void Render();
        bool IsVisible();
        void Show(bool show);

    private:
        MainMenu(const GDK::DeviceContext& deviceContext);

        bool _visible;
        bool _firstVisible;

        std::shared_ptr<GDK::ContentCache> _content;

        std::vector<std::wstring> _menuItems;
        std::vector<std::wstring> _cursor;

        std::vector<std::wstring> _menuSounds;

        GDK::DeviceContext _deviceContext;
        int _currentSelection;

        bool _promptToQuit;
        bool _quitGame;

        float _animationTimer;
        int _animationFrame;
    };
};
