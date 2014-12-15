#include "precomp.h"
#include "controlsmenu.h"

ControlsMenu::ControlsMenu() :
Menu(MenuId::ControlsMenu, "Controls")
{

}

void ControlsMenu::OnSelected(_In_ MenuItem item)
{

}

void ControlsMenu::OnDismiss()
{
    GetGame().ShowMenu(MenuId::OptionsMenu);
}
