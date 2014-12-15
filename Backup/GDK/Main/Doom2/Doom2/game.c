#include "common.h"

#define DEFAULT_TITLE       "DOOM II"
#define DEFAULT_SCREENX     320
#define DEFAULT_SCREENY     200
#define DEFAULT_FRAMERATE   60

HWND g_gameWindow = NULL;
float g_targetElapsedTime = 1.0f / DEFAULT_FRAMERATE;
float g_totalFrameTime = 0.0f;
BOOL  g_gamePaused = FALSE;
gamestate_t g_gameState = gamestate_splash;

// DEBUG DEBUG
int g_debugMapXOffset = 0;
int g_debugMapYOffset = 0;
float g_debugMapScale = 0.125f;
BOOL g_debugMapEnabled = FALSE;
byte_t g_debugMusicId = 0;
byte_t g_musicVolume = 100;

#define MAX_DOOM_LEVELS 32
byte_t g_gameLevel = 0;

BOOL GameInitialize();
void GameUninitialize();
void GameTick();
void GameUpdate();
void GameRender();
void GameDrawSplash();
void GameDestroySplash();

LRESULT WINAPI GameWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    MSG msg;
    WNDCLASS wc;

    ZeroMemory(&msg, sizeof(msg));
    ZeroMemory(&wc, sizeof(wc));

    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.hInstance = hInstance;
    wc.lpfnWndProc = GameWndProc;
    wc.lpszClassName = "GameWindowClass";
    wc.hIcon = NULL;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    if (!RegisterClass(&wc))
    {
        return 1;
    }

    g_gameWindow = CreateWindow(wc.lpszClassName, DEFAULT_TITLE, WS_THICKFRAME | WS_BORDER | WS_SYSMENU | WS_MINIMIZE, 0, 0, DEFAULT_SCREENX * 3, DEFAULT_SCREENY * 3, NULL, NULL, hInstance, NULL);
    if (!g_gameWindow)
    {
        return 2;
    }

    if (!GameInitialize())
    {
        DestroyWindow(g_gameWindow);
        return 3;
    }

    UpdateWindow(GameGetWindow());
    ShowWindow(GameGetWindow(), nCmdShow);

    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            GameTick();
        }
    }

    GameUninitialize();

    DebugMemoryReport();

    return 0;
}

BOOL GameInitialize()
{
    if (!MemoryManagerInitialize())
    {
        return FALSE;
    }

    // Open .WAD for access
    if (!WadOpen(CONTENT_ROOT"raw\\doom2.wad"))
    {
        return FALSE;
    }

    // Audio can initialization for systems that do not have 
    // audio output devices attached.  This is ok, audio calls
    // will noop.
    AudioInitialize();

    if (!GraphicsInitialize(DEFAULT_SCREENX, DEFAULT_SCREENY))
    {
        return FALSE;
    }

    if (!RendererInitialize())
    {
        return FALSE;
    }

    if (!InputInitialize())
    {
        return FALSE;
    }

    if (!MenuCreate())
    {
        return FALSE;
    }

    HudCreate();

    //AudioPlayMusic("D_RUNNIN");

    return TRUE;
}

BOOL GameLoadLevel(BYTE level)
{
    char mapName[6];
    ZeroMemory(mapName, sizeof(mapName));

    if (level < 10)
    {
        sprintf_s(mapName, sizeof(mapName),"MAP0%d",level);
    }
    else
    {
        sprintf_s(mapName, sizeof(mapName), "MAP%d",level);
    }

    if (MapLoad(mapName))
    {
        g_gameLevel = level;

        // Only reset the player if the level is level 1
        if (level == 1)
        {
            PlayerReset();
        }

        return TRUE;
    }

    return FALSE;
}

byte_t GameGetLevel()
{
    return g_gameLevel;
}

void GameUninitialize()
{

    MapDestroy();
    HudDestroy();
    MenuDestroy();

    RendererUninitialize();
    GraphicsUninitialize();
    AudioUninitialize();

    InputUninitialize();
    WadClose();

    MemoryManagerUninitialize();
}

void GameTick()
{
    GameTimeUpdate();

    // Ensure target framerate
    if (GameTimeGetElapsed() < GameGetTargetElapsedTime())
    {
        Sleep((unsigned long)((GameGetTargetElapsedTime() - GameTimeGetElapsed()) * 1000.f));
    }

    InputUpdate();

    // Handle special menu key
    if (InputIsKeyJustPressed(VK_ESCAPE))
    {
        MenuToggle();
    }

    if (!GameGetPause())
    {
        GameUpdate();
    }

    GameRender();
}

LRESULT WINAPI GameWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    switch (Msg)
    {
    case WM_CLOSE:
        GameExit();
        break;

    default:
        AudioHandleMessage(Msg, wParam, lParam);
        return DefWindowProc(hWnd, Msg, wParam, lParam);
    }
    return 0;
}

HWND GameGetWindow()
{
    return g_gameWindow;
}

BOOL GameSetTitle(const char* title)
{
    HWND window = NULL;

    if (!title)
    {
        return FALSE;
    }

    window = GameGetWindow();
    if (!window)
    {
        return FALSE;
    }

    if (!SetWindowTextA(window, title))
    {
        return FALSE;
    }

    return TRUE;
}

void GameUpdate()
{
    // pre-transformed movement
    short forward = 0;
    short sideways = 0;
    float rotation = 0.0f;

    if (InputIsKeyJustPressed(VkKeyScanA('=')))
    {
        byte_t size = GraphicsGetViewSize();
        size = min(size + 1, 10);
        GraphicsSetViewSize(size);
    }
    else if (InputIsKeyJustPressed(VkKeyScanA('-')))
    {
        byte_t size = GraphicsGetViewSize();
        size = max((int)size - 1, 1);
        GraphicsSetViewSize(size);
    }

    // Wolfenstein 3D cheat code I-L-M
    if (InputIsKeyDown(VkKeyScan('i')) && InputIsKeyDown(VkKeyScan('l')) && InputIsKeyDown(VkKeyScan('m')))
    {
        PlayerGiveAll();
        HudPrintMessage("Wolfenstein 3D Cheater!!");
        return;
    }

    if (InputIsKeyDown(VK_CONTROL))
    {
        PlayerShootWeapon();
    }

    if (InputIsKeyJustPressed(VkKeyScanA('g')))
    {
        PlayerSetGodMode(!PlayerInGodMode());
    }

    if (InputIsKeyJustPressed(VkKeyScanA('1')))
    {
        PlayerSwitchWeaponById(1);
    }

    if (InputIsKeyJustPressed(VkKeyScanA('2')))
    {
        PlayerSwitchWeaponById(2);
    }

    if (InputIsKeyJustPressed(VkKeyScanA('3')))
    {
        PlayerSwitchWeaponById(3);
    }

    if (InputIsKeyJustPressed(VkKeyScanA('4')))
    {
        PlayerSwitchWeaponById(4);
    }

    if (InputIsKeyJustPressed(VkKeyScanA('5')))
    {
        PlayerSwitchWeaponById(5);
    }

    if (InputIsKeyJustPressed(VkKeyScanA('6')))
    {
        PlayerSwitchWeaponById(6);
    }

    if (InputIsKeyJustPressed(VkKeyScanA('7')))
    {
        PlayerSwitchWeaponById(7);
    }

    // DEBUG DEBUG
    if(InputIsKeyJustPressed(VK_SPACE))
    {
        PlayerTakeDamage(10);
    }

    // we don't use else ifs here because holding down
    // left & right should cancel each other, etc...
    if (InputIsKeyDown(VK_LEFT))
    {
        rotation += TORADIANS(2);
    }
    if (InputIsKeyDown(VK_RIGHT))
    {
        rotation -= TORADIANS(2);
    }
    if (InputIsKeyDown(VK_UP))
    {
        ++forward;
    }
    if (InputIsKeyDown(VK_DOWN))
    {
        --forward;
    }

    if (forward != 0 || sideways != 0)
    {
        vector2_t movement = Vector2Add(Vector2Scale(Vector2Perp(PlayerGetDirection()), sideways), Vector2Scale(PlayerGetDirection(), forward));
        short playerSpeed = 3;

        movement = Vector2Normalize(movement);

        PlayerMove(Vector2Scale(movement, playerSpeed));
    }
    if (rotation != 0.0f)
    {
        PlayerRotate(rotation);
    }

    if (InputIsKeyJustPressed(VK_TAB))
    {
        g_debugMapEnabled = !g_debugMapEnabled;
    }
    else if (InputIsKeyJustPressed(VkKeyScan('n')))
    {
        GameLoadPreviousLevel();
    }
    else if (InputIsKeyJustPressed(VkKeyScan('m')))
    {
        GameLoadNextLevel();
    }

    if (g_debugMapEnabled)
    {
        if (InputIsKeyDown(VkKeyScan('a')))
        {
            g_debugMapScale -= 0.001f;

            if (g_debugMapScale < 0.0f)
            {
                g_debugMapScale = 0.0f;
            }
        }
        else if (InputIsKeyDown(VkKeyScan('s')))
        {
            g_debugMapScale += 0.001f;
        }
    }
}

BOOL GameLoadNextLevel()
{
    g_gameLevel++;
    if (g_gameLevel > MAX_DOOM_LEVELS)
    {
        g_gameLevel = 1;
    }
    return GameLoadLevel(g_gameLevel);
}

BOOL GameLoadPreviousLevel()
{
    g_gameLevel--;
    if (g_gameLevel == 0)
    {
        g_gameLevel = MAX_DOOM_LEVELS;
    }
    return GameLoadLevel(g_gameLevel);
}

void GameRender()
{
    if (GameGetLevel())
    {
        if (g_debugMapEnabled)
        {
            // DEBUG DEBUG
            GraphicsClear(0);
            DebugMapDraw(g_debugMapXOffset, g_debugMapYOffset, g_debugMapScale);
        }
        else
        {
            LARGE_INTEGER start, end, freq;

            QueryPerformanceFrequency(&freq);

            // No clear needed
            GraphicsClear(0);
            GraphicsClearViewport(0);
            QueryPerformanceCounter(&start);
            Render3DView();
            QueryPerformanceCounter(&end);
            DebugOut("Frame time: %4.4f", (end.QuadPart - start.QuadPart) / (double)freq.QuadPart);
            HudRender();
        }
    }
    else
    {
        // No clear needed
        GameDrawSplash();
    }

    MenuRender();

    GraphicsPresent();
}

float GameGetTargetElapsedTime()
{
    return g_targetElapsedTime;
}

void GamePause(BOOL pause)
{
    g_gamePaused = pause;
}

BOOL GameGetPause()
{
    return g_gamePaused;
}

void GameExit()
{
    PostQuitMessage(0);
}

void SetTargetElapsedTime(float elapsedTime)
{
    g_targetElapsedTime = elapsedTime;
}

gamestate_t GameGetState()
{
    return g_gameState;
}

void GameSetState(gamestate_t gamestate)
{
    g_gameState = gamestate;
}

void GameDrawSplash()
{
    MenuRenderSplashScreen();
}

void GameDestroySplash()
{
    MenuDestroySplashScreen();
}