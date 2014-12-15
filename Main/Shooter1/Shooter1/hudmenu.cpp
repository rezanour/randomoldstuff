#include "precomp.h"
#include "hudmenu.h"

HudMenu::HudMenu() :
Menu(MenuId::ControlsMenu, "HUD")
{

}

void HudMenu::OnSelected(_In_ MenuItem item)
{

}

void HudMenu::OnDismiss()
{
    GetGame().ShowMenu(MenuId::OptionsMenu);
}
