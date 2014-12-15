#include <GDK.h>
#include <MemoryObject.h>
#include "Quake2Game.h"
#include "Menu.h"

namespace Quake2
{
    struct uint32Pair_t
    {
        uint32_t x;
        uint32_t y;
    };
    // The pixel coordinates specified here assume a 640x480 layout.
    const uint32Pair_t QuitPromptLocation   = {168, 150};
    const uint32Pair_t VerticalLogoLocation = {180, 130};
    const uint32Pair_t IdLogoLocation       = {180, 300};
    const uint32Pair_t GameMenuItemLocation = {248, 144};
    const uint32Pair_t MenuCursorLocation   = {220, 144};
    const int MenuItemSpacing = 6;

    std::shared_ptr<MainMenu> MainMenu::Create(const GDK::DeviceContext& deviceContext)
    {
        return std::shared_ptr<MainMenu>(GDKNEW MainMenu(deviceContext));
    }

    MainMenu::MainMenu(const GDK::DeviceContext& deviceContext) :
        _deviceContext(deviceContext),
        _visible(false),
        _currentSelection(0),
        _firstVisible(false),
        _promptToQuit(false),
        _animationTimer(0),
        _animationFrame(0),
        _quitGame(false)
    {
        _content = GDK::ContentCache::Create(deviceContext);

        // Load menu sounds
        wchar_t* menuSounds[] = {
            L"sound/misc/menu1.wav", // select
            L"sound/misc/menu2.wav", // selection changed
            L"sound/misc/menu3.wav", // back out of/return to
        };

        for (size_t i = 0; i < sizeof(menuSounds)/sizeof(menuSounds[0]); i++)
        {
            _menuSounds.push_back(menuSounds[i]);
        }

        // Load menu item textures
        wchar_t* menuTextures[] = {
            L"pics/m_main_game_sel.pcx.texture",
            L"pics/m_main_game.pcx.texture",
            L"pics/m_main_multiplayer_sel.pcx.texture",
            L"pics/m_main_multiplayer.pcx.texture",
            L"pics/m_main_options_sel.pcx.texture",
            L"pics/m_main_options.pcx.texture",
            L"pics/m_main_video_sel.pcx.texture",
            L"pics/m_main_video.pcx.texture",
            L"pics/m_main_quit_sel.pcx.texture",
            L"pics/m_main_quit.pcx.texture",
        };

        for (size_t i = 0; i < sizeof(menuTextures)/sizeof(menuTextures[0]); i++)
        {
            _menuItems.push_back(menuTextures[i]);
        }

        // Load cursors
        for (int i = 0; i < 15; i++)
        {
            wchar_t num[4] = {0};
            errno_t itowerror = 0;
            std::wstring cursorName = L"pics/m_cursor";
            itowerror = _itow_s(i, num, sizeof(num)/sizeof(wchar_t), 10);
            cursorName.append(num);
            cursorName.append(L".pcx.texture");
            _cursor.push_back(cursorName);
        }
    }

    bool MainMenu::Update(_In_ const GDK::GameTime& gameTime)
    {
        // Handle Game Quit prompt
        if (_promptToQuit)
        {
            if (GDK::Input::WasButtonPressed(QuakeKey::Yes))
            {
                return false;
            }

            if (GDK::Input::WasButtonPressed(QuakeKey::No))
            {
                _promptToQuit = false;
            }

            return true;
        }

        // Animate Cursor
        _animationTimer += gameTime.deltaTime;
        if (_animationTimer >= 0.1f)
        {
            _animationFrame = (_animationFrame + 1) % _cursor.size();
            _animationTimer -= 0.1f;
        }

        // Play Menu down selection sound
        if (GDK::Input::WasButtonPressed(QuakeKey::MenuDown))
        {
            _currentSelection++;
            if (_currentSelection > 4)
            {
                _currentSelection = 0;
            }
            _deviceContext.audioDevice->PlayClip(_content->GetAudioClip(_menuSounds[1]));
        }

        // Play Menu up selection sound
        if (GDK::Input::WasButtonPressed(QuakeKey::MenuUp))
        {
            if (_currentSelection > 0)
            {
                _currentSelection--;
            }
            else
            {
                _currentSelection = 4;
            }
            _deviceContext.audioDevice->PlayClip(_content->GetAudioClip(_menuSounds[1]));
        }

        // Play Menu item selected sound and perform selected option
        if (GDK::Input::WasButtonPressed(QuakeKey::MenuSelect))
        {
            _deviceContext.audioDevice->PlayClip(_content->GetAudioClip(_menuSounds[0]));

            // If Quit was selected, prompt the user
            if (_currentSelection == 4)
            {
                _promptToQuit = true;
            }
        }

        // Play Menu dismissed sound
        if (GDK::Input::WasButtonPressed(QuakeKey::Menu))
        {
            _deviceContext.audioDevice->PlayClip(_content->GetAudioClip(_menuSounds[2]));
        }

        // Play Menu invoked sound
        if (_firstVisible)
        {
            _deviceContext.audioDevice->PlayClip(_content->GetAudioClip(_menuSounds[1]));
            _firstVisible = false;
        }

        return true;
    }

    void MainMenu::Render()
    {
        _deviceContext.graphicsDevice->Set2DWorkArea(640, 480);

        if (_promptToQuit)
        {
            // Render Quit prompt
            _deviceContext.graphicsDevice->Draw2D(_content->GetTexture(L"pics/quit.pcx.texture"), QuitPromptLocation.x, QuitPromptLocation.y);
        }
        else
        {
            // Render Vertical Logo
            _deviceContext.graphicsDevice->Draw2D(_content->GetTexture(L"pics/m_main_plaque.pcx.texture"), VerticalLogoLocation.x, VerticalLogoLocation.y);

            // Render Id Logo
            _deviceContext.graphicsDevice->Draw2D(_content->GetTexture(L"pics/m_main_logo.pcx.texture"), IdLogoLocation.x, IdLogoLocation.y);

            // Render Menu items
            uint32Pair_t menuItemStartLocation = GameMenuItemLocation;
            for (size_t i = 0; i < _menuItems.size()/2; i++)
            {
                // Draw item
                if (i == _currentSelection)
                {
                    _deviceContext.graphicsDevice->Draw2D(_content->GetTexture(_cursor[_animationFrame]), MenuCursorLocation.x, menuItemStartLocation.y);
                    _deviceContext.graphicsDevice->Draw2D(_content->GetTexture(_menuItems[i*2]), menuItemStartLocation.x, menuItemStartLocation.y);
                }
                else
                {
                    _deviceContext.graphicsDevice->Draw2D(_content->GetTexture(_menuItems[(i*2) + 1]), menuItemStartLocation.x, menuItemStartLocation.y);
                }
                menuItemStartLocation.y += (MenuItemSpacing + _content->GetTexture(_menuItems[i])->GetHeight());
            }
        }
    }

    bool MainMenu::IsVisible()
    {
        return _visible;
    }

    void MainMenu::Show(bool show)
    {
        // Track first visible flag so we know that the menu was invoked from a not visible state
        _firstVisible = (!_visible && show);
        _visible = show;

        // Ensure that the menu is properly reset if dismissed
        if (!show)
        {
            _content->Clear();
            _promptToQuit = false;
        }
    }

};