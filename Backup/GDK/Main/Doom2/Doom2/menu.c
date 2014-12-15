#include "common.h"

#define MENU_TEXT_HEIGHT 16
#define FACE_ANIMATION_INTERVAL     0.25f
FLOAT g_skullAnimationTime = 0;
int g_menuSelection = 0;

BOOL g_promptingToQuit = FALSE;

#define MENU_MAIN    0
#define MENU_NEWGAME 1

byte_t g_menu = MENU_MAIN;

typedef void (*PMENUITEM_FUNC)();

typedef struct
{
    char* name;
    PMENUITEM_FUNC menuItemFunc;
} menuItem_t;

void MenuResetToMain();

void MenuNewGame();
void MenuExitGame();

void MenuSelectEasySkill();
void MenuSelectHurtMePlentySkill();
void MenuSelectRoughSkill();
void MenuSelectUltraSkill();
void MenuSelectNightmareSkill();

menuItem_t g_mainMenu[] = 
{
    {"M_NGAME",  MenuNewGame},
    {"M_OPTION", NULL},
    {"M_LOADG",  NULL},
    {"M_SAVEG",  NULL},
    {"M_QUITG",  MenuExitGame},
};

menuItem_t g_newGameMenu[] = 
{
    {"M_JKILL", MenuSelectEasySkill},
    {"M_ROUGH", MenuSelectRoughSkill},
    {"M_HURT",  MenuSelectHurtMePlentySkill},
    {"M_ULTRA", MenuSelectUltraSkill},
    {"M_NMARE", MenuSelectNightmareSkill}
};

char* g_skulls[] = 
{
    "M_SKULL1",
    "M_SKULL2",
};

char* g_quitMessage = NULL;

char* g_quitMessages[] = 
{
  "you want to quit?\nthen, thou hast lost an eighth!",
  "don't go now, there's a \ndimensional shambler waiting\nat the dos prompt!",
  "get outta here and go back\nto your boring programs.",
  "if i were your boss, i'd \n deathmatch ya in a minute!",
  "look, bud. you leave now\nand you forfeit your body count!",
  "just leave. when you come\nback, i'll be waiting with a bat.",
  "you're lucky i don't smack\nyou for thinking about leaving.",
  "(press y to quit to DOS)" // reserved
};

BOOL g_skullOn = FALSE;

resourcemanager_t g_MenuResourceManager;

resource_t MenuGetSprite(const char* name);

void MenuRenderSplashScreen()
{
    GraphicsDrawSprite(0,0, MenuGetSprite("TITLEPIC"));
}

void MenuDestroySplashScreen()
{
    MenuDestroy();
    MenuCreate();
}

void MenuSelectRandomQuitPrompt()
{
    g_quitMessage = g_quitMessages[rand() % (ARRAYSIZE(g_quitMessages) - 1)];
}

void MenuRenderText(int x, int y, char* text)
{
    int startX = x;
    size_t i = 0;
    char fontcharName[16];
    ZeroMemory(fontcharName, sizeof(fontcharName));

    if (g_quitMessage)
    {
        for (i = 0; i < strlen(text); i++)
        {
            if (text[i] == VK_SPACE)
            {
                // render nothing...
                x += FONTCHARACTER_WIDTH;
            }
            else if (text[i] == '\n')
            {
                y += FONTCHARACTER_WIDTH; // (width is also height of font)
                x = startX;
            }
            else
            {
                sprintf_s(fontcharName, sizeof(fontcharName),"STCFN0%d",toupper(text[i]));
                GraphicsDrawSprite(x,y, MenuGetSprite(fontcharName));
                x += (text[i] != 'i' ? FONTCHARACTER_WIDTH : FONTCHARACTER_WIDTH/2);
            }
        }
    }
}

void MenuRenderQuitPrompt()
{
    size_t i = 0;
    int y = 50;
    int x = 36;
    char fontcharName[16];
    ZeroMemory(fontcharName, sizeof(fontcharName));

    if (g_quitMessage)
    {
        MenuRenderText(x,y,g_quitMessage);
    }

    MenuRenderText(x, y + 32, g_quitMessages[ARRAYSIZE(g_quitMessages) - 1]);

    if (InputIsKeyJustPressed(VkKeyScan('y')))
    {
        GameExit();
    }

    if (InputIsKeyJustPressed(VkKeyScan('n')) || InputIsKeyJustPressed(VK_ESCAPE))
    {
        g_promptingToQuit = FALSE;
    }
}

BOOL MenuCreate()
{
    g_MenuResourceManager = ResourceManagerCreate();
    return (g_MenuResourceManager ? TRUE : FALSE);
}

void MenuDestroy()
{
    if (g_MenuResourceManager)
    {
        ResourceManagerDestroy(g_MenuResourceManager);
    }
}

void MenuToggle()
{
    GamePause(!GameGetPause());
}

void MenuUpdate()
{

}

void MenuRenderMainMenu()
{
    int menuId = 0;
    int x = 0;
    int y = 68;

    // Handle skull blink animation timer
    g_skullAnimationTime += GameTimeGetElapsed();
    if (g_skullAnimationTime > FACE_ANIMATION_INTERVAL)
    {
        g_skullOn = !g_skullOn;
        g_skullAnimationTime = 0;
    }

    // The original game shows the menu's DOOM II title over top the splash screen which already
    // has a big DOOM II logo.  Uncomment the below line to not show double DOOM II logos.
    //if (GameGetLevel())
    {
        GraphicsDrawSprite(100,3, MenuGetSprite("M_DOOM"));
    }

    for (menuId = 0; menuId < ARRAYSIZE(g_mainMenu); menuId++)
    {
        GraphicsDrawSprite(100,y + MENU_TEXT_HEIGHT * menuId, MenuGetSprite(g_mainMenu[menuId].name));
        if (g_menuSelection == menuId)
        {
            GraphicsDrawSprite(70, (y + MENU_TEXT_HEIGHT * menuId) - 2, MenuGetSprite(g_skulls[g_skullOn]));
        }
    }

    // Handle cursor for menu
    if (InputIsKeyJustPressed(VK_DOWN))
    {
        g_menuSelection++;
        if (g_menuSelection > (ARRAYSIZE(g_mainMenu) - 1))
        {
            g_menuSelection = 0;
        }
    }

    if (InputIsKeyJustPressed(VK_UP))
    {
        g_menuSelection--;
        if (g_menuSelection < 0)
        {
            g_menuSelection = (ARRAYSIZE(g_mainMenu) - 1);
        }
    }

    if (InputIsKeyJustPressed(VK_RETURN))
    {
        if (g_mainMenu[g_menuSelection].menuItemFunc)
        {
            g_mainMenu[g_menuSelection].menuItemFunc();
        }
    }
}

void MenuRenderNewGameMenu()
{
    int menuId = 0;
    int x = 0;
    int y = 75;

    // Handle skull blink animation timer
    g_skullAnimationTime += GameTimeGetElapsed();
    if (g_skullAnimationTime > FACE_ANIMATION_INTERVAL)
    {
        g_skullOn = !g_skullOn;
        g_skullAnimationTime = 0;
    }

    GraphicsDrawSprite(97,10, MenuGetSprite("M_NEWG"));
    GraphicsDrawSprite(57,36, MenuGetSprite("M_SKILL"));

    for (menuId = 0; menuId < ARRAYSIZE(g_newGameMenu); menuId++)
    {
        GraphicsDrawSprite(55,y + MENU_TEXT_HEIGHT * menuId, MenuGetSprite(g_newGameMenu[menuId].name));
        if (g_menuSelection == menuId)
        {
            GraphicsDrawSprite(25, (y + MENU_TEXT_HEIGHT * menuId) - 2, MenuGetSprite(g_skulls[g_skullOn]));
        }
    }

    // Handle cursor for menu
    if (InputIsKeyJustPressed(VK_DOWN))
    {
        g_menuSelection++;
        if (g_menuSelection > (ARRAYSIZE(g_newGameMenu) - 1))
        {
            g_menuSelection = 0;
        }
    }

    if (InputIsKeyJustPressed(VK_UP))
    {
        g_menuSelection--;
        if (g_menuSelection < 0)
        {
            g_menuSelection = (ARRAYSIZE(g_newGameMenu) - 1);
        }
    }

    if (InputIsKeyJustPressed(VK_RETURN))
    {
        if (g_newGameMenu[g_menuSelection].menuItemFunc)
        {
            g_newGameMenu[g_menuSelection].menuItemFunc();
        }

        // Reset to MAIN MENU before exiting
        MenuResetToMain();
    }

    if (InputIsKeyJustPressed(VK_ESCAPE))
    {
        // Reset to MAIN MENU before exiting
        MenuResetToMain();
    }
}

void MenuResetToMain()
{
    g_menu = MENU_MAIN;
    g_menuSelection = 0;
}

void MenuRender()
{
    if (g_promptingToQuit)
    {
        MenuRenderQuitPrompt();
    }
    else
    {
        if (GameGetPause())
        {
            switch(g_menu)
            {
            case MENU_MAIN:
                MenuRenderMainMenu();
                break;
            case MENU_NEWGAME:
                MenuRenderNewGameMenu();
                break;
            default:
                break;
            }
        }
    }
}

resource_t MenuGetSprite(const char* name)
{
    return ResourceManagerGet(g_MenuResourceManager, resource_type_sprite, name);
}

void MenuNewGame()
{
    g_menu = MENU_NEWGAME;
    g_menuSelection = 0;
}

void MenuExitGame()
{
    MenuSelectRandomQuitPrompt();
    g_promptingToQuit = TRUE;
}

void MenuSelectEasySkill()
{
    GameLoadLevel(1);
    GamePause(FALSE);
}

void MenuSelectHurtMePlentySkill()
{
    GameLoadLevel(1);
    GamePause(FALSE);
}

void MenuSelectRoughSkill()
{
    GameLoadLevel(1);
    GamePause(FALSE);
}

void MenuSelectUltraSkill()
{
    GameLoadLevel(1);
    GamePause(FALSE);
}

void MenuSelectNightmareSkill()
{
    GameLoadLevel(1);
    GamePause(FALSE);
}