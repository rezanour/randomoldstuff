#include "precomp.h"
#include "optionsmenu.h"

#define OPTIONSMENU_AUDIO    0
#define OPTIONSMENU_CONTROLS 1
#define OPTIONSMENU_HUD      2
#define OPTIONSMENU_GRAPHICS 3

OptionsMenu::OptionsMenu() :
    Menu(MenuId::OptionsMenu, "Options")
{
    _items.push_back(MenuItem(OPTIONSMENU_AUDIO,    "Audio"));
    _items.push_back(MenuItem(OPTIONSMENU_CONTROLS, "Controls"));
    _items.push_back(MenuItem(OPTIONSMENU_HUD,      "HUD"));
    _items.push_back(MenuItem(OPTIONSMENU_GRAPHICS, "Graphics"));
}

void OptionsMenu::OnSelected(_In_ MenuItem item)
{
    switch(item.id)
    {
        case OPTIONSMENU_AUDIO:
            GetGame().ShowMenu(MenuId::AudioMenu);
        break;
        case OPTIONSMENU_CONTROLS:
            GetGame().ShowMenu(MenuId::ControlsMenu);
        break;
        case OPTIONSMENU_HUD:
            GetGame().ShowMenu(MenuId::HudMenu);
        break;
        case OPTIONSMENU_GRAPHICS:
            GetGame().ShowMenu(MenuId::GraphicsMenu);
        break;
    }
}

void OptionsMenu::OnDismiss()
{
    if (GetGame().IsPlaying())
    {
        GetGame().ShowMenu(MenuId::GameMenu);
    }
    else
    {
        GetGame().ShowMenu(MenuId::MainMenu);
    }
}
