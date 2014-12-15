#include "Precomp.h"
#include "AssetLoading.h"
#include "Screen.h"
#include "GameplayScreen.h"
#include "MainMenuScreen.h"
#include "GraphicsSystem.h"
#include "ScreenSystem.h"
#include "SpriteFont.h"
#include "Game\Inputhelper.h"
#include "Contexts.h"

_Use_decl_annotations_
std::shared_ptr<GameplayScreen> GameplayScreen::Create(const std::shared_ptr<IGraphicsSystem>& graphics)
{
    return std::shared_ptr<GameplayScreen>(new GameplayScreen(graphics));
}

_Use_decl_annotations_
GameplayScreen::GameplayScreen(const std::shared_ptr<IGraphicsSystem>& graphics)
{
    auto assetLoader = AssetLoader::Create();
    _spriteFont = assetLoader->LoadSpriteFontFromFile(graphics, L"font18.spritefont");
}

bool GameplayScreen::SupportsFocus() const
{
    return true;
}

_Use_decl_annotations_
void GameplayScreen::Update(const UpdateContext& context, bool hasFocus)
{
    UNREFERENCED_PARAMETER(context);
    UNREFERENCED_PARAMETER(hasFocus);
    if (DismissButtonPressed(context.Input))
    {
        context.ScreenSystem->PopScreen();
        context.ScreenSystem->PushScreen(MainMenuScreen::Create(context.GraphicsSystem));
        return;
    }
}

_Use_decl_annotations_
void GameplayScreen::Draw(const DrawContext& context)
{
    auto config = context.GraphicsSystem->GetConfig();
    const wchar_t* st = L"( Gameplay Screen )";

    _spriteFont->Draw(context.GraphicsSystem, st, 
        (config.Width/2)-_spriteFont->GetStringLength(st)/2,config.Height/2 - _spriteFont->GetLineSpacing());
}
