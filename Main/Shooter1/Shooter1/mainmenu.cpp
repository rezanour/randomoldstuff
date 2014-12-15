#include "precomp.h"
#include "mainmenu.h"

#define MAINMENU_NEWGAME 0
#define MAINMENU_OPTIONS 1
#define MAINMENU_EXIT    2

MainMenu::MainMenu() :
    Menu(MenuId::MainMenu, "Main Menu")
{
    _items.push_back(MenuItem(MAINMENU_NEWGAME, "New Game"));
    _items.push_back(MenuItem(MAINMENU_OPTIONS, "Options"));
    _items.push_back(MenuItem(MAINMENU_EXIT,    "Exit"));
}

void MainMenu::OnSelected(_In_ MenuItem item)
{
    switch(item.id)
    {
        case MAINMENU_NEWGAME:
            GetGame().StartNewGame();
        break;
        case MAINMENU_OPTIONS:
            GetGame().ShowMenu(MenuId::OptionsMenu);
        break;
        case MAINMENU_EXIT:
            GetGame().Exit();
        break;
    }
}

void MainMenu::OnDismiss()
{

}
