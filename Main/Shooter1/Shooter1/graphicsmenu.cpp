#include "precomp.h"
#include "graphicsmenu.h"

GraphicsMenu::GraphicsMenu() :
Menu(MenuId::GraphicsMenu, "Graphics")
{

}

void GraphicsMenu::OnSelected(_In_ MenuItem item)
{

}

void GraphicsMenu::OnDismiss()
{
    GetGame().ShowMenu(MenuId::OptionsMenu);
}
