#include "Precomp.h"
#include "AssetLoading.h"
#include "Transform.h"
#include "Screen.h"
#include "MainMenuScreen.h"
#include "GameplayScreen.h"
#include "GraphicsTestScreen.h"
#include "AITestScreen.h"
#include "Texture.h"
#include "GraphicsSystem.h"
#include "ScreenSystem.h"
#include "SpriteFont.h"
#include "Input.h"
#include "Game\InputHelper.h"
#include "Contexts.h"

_Use_decl_annotations_
std::shared_ptr<MainMenuScreen> MainMenuScreen::Create(const std::shared_ptr<IGraphicsSystem>& graphics)
{
    return std::shared_ptr<MainMenuScreen>(new MainMenuScreen(graphics));
}

_Use_decl_annotations_
MainMenuScreen::MainMenuScreen(const std::shared_ptr<IGraphicsSystem>& graphics) :
    _selectedItem(0)
{
    auto assetLoader = AssetLoader::Create();
    _spriteFont = assetLoader->LoadSpriteFontFromFile(graphics, L"menufont.36pt.spritefont");
    _spriteFontSelected = assetLoader->LoadSpriteFontFromFile(graphics, L"menufont.48pt.spritefont");

    // Set menu item labels
    _items[(uint32_t)MainMenuItemType::TestGraphics] = L"Test Graphics";
    _items[(uint32_t)MainMenuItemType::TestAI] = L"Test AI";
    _items[(uint32_t)MainMenuItemType::Game] = L"Game";
    _items[(uint32_t)MainMenuItemType::Exit] = L"Exit";
}

bool MainMenuScreen::SupportsFocus() const
{
    return true;
}

_Use_decl_annotations_
bool MainMenuScreen::HandleMenuSelection(const UpdateContext& context)
{
    if (NextButtonPressed(context.Input))
    {
        _selectedItem = NextItemWrapped(_selectedItem, _countof(_items));
    }

    if (PrevItemButtonPressed(context.Input))
    {
        _selectedItem = PrevItemWrapped(_selectedItem, _countof(_items));
    }

    return SelectButtonPressed(context.Input);
}

_Use_decl_annotations_
void MainMenuScreen::Update(const UpdateContext& context, bool hasFocus)
{
    _hasFocus = hasFocus;
    if (hasFocus)
    {
        if (HandleMenuSelection(context))
        {
            switch((MainMenuItemType)_selectedItem)
            {
                case MainMenuItemType::TestGraphics:
                    context.ScreenSystem->PopScreen();
                    context.ScreenSystem->PushScreen(GraphicsTestScreen::Create(context.GraphicsSystem));
                    break;
                case MainMenuItemType::TestAI:
                    context.ScreenSystem->PopScreen();
                    context.ScreenSystem->PushScreen(AITestScreen::Create(context.GraphicsSystem, context.AudioSystem));
                    break;
                case MainMenuItemType::Game:
                    context.ScreenSystem->PopScreen();
                    context.ScreenSystem->PushScreen(GameplayScreen::Create(context.GraphicsSystem));
                    break;
                case MainMenuItemType::Exit:
                    PostQuitMessage(0);
                default:
                    break;
            }
        }

        // Commonly we rip out of the screens by pressing 'escape', so
        // if we are on the main menu screen, just quit here.
        if (DismissButtonPressed(context.Input))
        {
            PostQuitMessage(0);
        }
    }
}

_Use_decl_annotations_
void MainMenuScreen::Draw(const DrawContext& context)
{
    if (!_hasFocus)
    {
        return;
    }
    auto config = context.GraphicsSystem->GetConfig();
    uint32_t maxMenuHeight = _countof(_items) * _spriteFont->GetLineSpacing();
    uint32_t yOffset = config.Height/2 - maxMenuHeight/2;
    uint32_t xOffset = 0;

    // Calculate xOffset by figuring out the max menu item width
    for (uint32_t i = 0; i < _countof(_items); i++)
    {
        xOffset = max(xOffset, _spriteFontSelected->GetStringLength(_items[i]));
    }
    xOffset = config.Width/2 - xOffset/2;

    // Render menu items
    for (uint32_t i = 0; i < _countof(_items); i++)
    {
        if (_selectedItem == i)
        {
            _spriteFontSelected->Draw(context.GraphicsSystem, _items[i], xOffset, yOffset);
            yOffset += _spriteFontSelected->GetLineSpacing();
        }
        else
        {
            _spriteFont->Draw(context.GraphicsSystem, _items[i], xOffset, yOffset);
            yOffset += _spriteFont->GetLineSpacing();
        }
    }
}
