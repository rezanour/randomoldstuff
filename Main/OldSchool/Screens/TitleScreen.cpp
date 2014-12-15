#include "Precomp.h"
#include "AssetLoading.h"
#include "Screen.h"
#include "TitleScreen.h"
#include "MainMenuScreen.h"
#include "Texture.h"
#include "GraphicsSystem.h"
#include "ScreenSystem.h"
#include "SpriteFont.h"
#include "Input.h"
#include "Game\InputHelper.h"
#include "Contexts.h"

const float AutoDismissTimeOut = 3.0f;

_Use_decl_annotations_
std::shared_ptr<TitleScreen> TitleScreen::Create(const std::shared_ptr<IGraphicsSystem>& graphics)
{
    return std::shared_ptr<TitleScreen>(new TitleScreen(graphics));
}

_Use_decl_annotations_
TitleScreen::TitleScreen(const std::shared_ptr<IGraphicsSystem>& graphics) :
     _timer(0.0f)
{
    auto assetLoader = AssetLoader::Create();
    _spriteFont = assetLoader->LoadSpriteFontFromFile(graphics, L"menufont.48pt.spritefont");
}

bool TitleScreen::SupportsFocus() const
{
    return true;
}

_Use_decl_annotations_
void TitleScreen::Update(const UpdateContext& context, bool hasFocus)
{
    if (hasFocus)
    {
        _timer += context.ElapsedTime;

        if (SelectButtonPressed(context.Input) ||
            DismissButtonPressed(context.Input))
        {
            _timer = AutoDismissTimeOut;
        }

        if (_timer > AutoDismissTimeOut)
        {
            context.ScreenSystem->PopScreen();
            context.ScreenSystem->PushScreen(MainMenuScreen::Create(context.GraphicsSystem));
        }
    }
}

_Use_decl_annotations_
void TitleScreen::Draw(const DrawContext& context)
{
    auto config = context.GraphicsSystem->GetConfig();
    const wchar_t* GameTitle = L"Old School";

    _spriteFont->Draw(context.GraphicsSystem, GameTitle, 
        (config.Width/2)-_spriteFont->GetStringLength(GameTitle)/2,config.Height/2 - _spriteFont->GetLineSpacing());
}
