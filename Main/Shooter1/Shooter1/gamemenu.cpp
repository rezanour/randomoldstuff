#include "precomp.h"
#include "gamemenu.h"

#define GAMEMENU_SAVEGAME 0
#define GAMEMENU_LOADGAME 1
#define GAMEMENU_OPTIONS  2
#define GAMEMENU_EXIT     3

GameMenu::GameMenu() :
    Menu(MenuId::GameMenu, "Game")
{
    _items.push_back(MenuItem(GAMEMENU_SAVEGAME, "Save Game"));
    _items.push_back(MenuItem(GAMEMENU_LOADGAME, "Load Game"));
    _items.push_back(MenuItem(GAMEMENU_OPTIONS, "Options"));
    _items.push_back(MenuItem(GAMEMENU_EXIT,    "Main Menu"));
}

void GameMenu::OnSelected(_In_ MenuItem item)
{
    switch(item.id)
    {
        case GAMEMENU_LOADGAME:
        case GAMEMENU_SAVEGAME:
        break;
        case GAMEMENU_OPTIONS:
            GetGame().ShowMenu(MenuId::OptionsMenu);
        break;
        case GAMEMENU_EXIT:
            GetGame().QuitGame();
        break;
    }
}

void GameMenu::OnDismiss()
{
    GetGame().DismissMenu();
}
