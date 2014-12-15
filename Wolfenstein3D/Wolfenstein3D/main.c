#include "common.h"
#include "resource.h"
#include <Psapi.h>

#define GAME_NAME           "Wolfenstein3D"
#define SCREENX             320
#define SCREENY             200

/* forward declarations */
BOOL InitializeGame();
VOID CleanupGame();
VOID OnIdle();
LRESULT WINAPI WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

VOID CompleteLevel();

VOID PrintMemoryStats(PSTR tag);

HWND g_hwnd = NULL;
FLOAT g_totalElapsedTime = 1.0f;
FLOAT g_fps = 1.0f;
FLOAT g_elapsedTime = 1.0f;
extern BOOL g_gameInProgress;
BYTE g_currentScreenId = INVALID_SCREEN;

/* viewport is always 2:1, since default is 320x160 (40 subtracted for HUD) */
static RECT validViewportSizes[] = 
{
    { 0, 0, SCREENX, SCREENY - 40 },
    { 20, 10, SCREENX - 20, SCREENY - 50 },
    { 40, 20, SCREENX - 40, SCREENY - 60 },
    { 60, 30, SCREENX - 60, SCREENY - 70 },
    { 80, 40, SCREENX - 80, SCREENY - 80 },
};

static BYTE currentViewport = 0;

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow)
{
    MSG msg;
    WNDCLASS wc;
    LARGE_INTEGER freq;
    LARGE_INTEGER start;
    LARGE_INTEGER lastStart;
    LARGE_INTEGER counter;
    ULONGLONG frameCounter = 1;

    ZeroMemory(&msg, sizeof(msg));
    ZeroMemory(&wc, sizeof(wc));

    srand( GAME_RANDOM_SEED );

    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.hInstance = hInstance;
    wc.lpfnWndProc = WndProc;
    wc.lpszClassName = GAME_NAME;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

    if (!RegisterClass(&wc))
    {
        return 1;
    }

    g_hwnd = CreateWindow(GAME_NAME, GAME_NAME, WS_THICKFRAME | WS_BORDER | WS_SYSMENU | WS_MINIMIZE, 0, 0, SCREENX * 3.5, SCREENY * 3.5, NULL, NULL, hInstance, NULL);
    if (!g_hwnd)
    {
        return 2;
    }

    if (!InitializeGame())
    {
        return 3;
    }

    UpdateWindow(g_hwnd);
    ShowWindow(g_hwnd, nCmdShow);

    ChangeScreen(WELCOME_STATS_SCREEN);

    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&start);
    QueryPerformanceCounter(&lastStart);

    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            QueryPerformanceCounter(&counter);
            g_totalElapsedTime = (FLOAT)(counter.QuadPart - start.QuadPart) / (FLOAT)freq.QuadPart;
            g_elapsedTime = (FLOAT)(counter.QuadPart - lastStart.QuadPart) / (FLOAT)freq.QuadPart;
            frameCounter++;

            g_fps = (float)frameCounter / g_totalElapsedTime;

            OnIdle();

            lastStart.QuadPart = counter.QuadPart;
        }
    }

    PrintMemoryStats("On exit");

    CleanupGame();

    PrintMemoryStats("Post cleanup");

    return 0;
}

BOOL InitializeGame()
{
    if (!W3DInitializeGraphics(g_hwnd, SCREENX, SCREENY))
    {
        return FALSE;
    }

    if (!AudioInitialize())
    {
        return FALSE;
    }

    W3DSetViewport(&validViewportSizes[currentViewport]);

    InitializeScreens();

    LevelInit();
    EnemyInit();
    PowerupInit();
    PlayerInit(FALSE);

    return TRUE;
}

VOID CleanupGame()
{
    PowerupShutdown();
    EnemyShutdown();
    LevelShutdown();

    AudioShutdown();
    W3DUninitializeGraphics();
}

VOID ChangeScreen(BYTE nextScreen)
{
    ScreenDestroy(g_currentScreenId);

    g_currentScreenId = nextScreen;

    ScreenCreate(nextScreen);
}

VOID StartPrevLevel()
{
    // Move to previous level
    if (g_player.Level == 0)
    {
        g_player.Level = (MAX_LEVELS - 1);
    }
    else
    {
        g_player.Level--;
    }
    StartNewLevel(g_player.Level);
}

VOID StartNextLevel()
{
    // Move to next level
    g_player.Level++;
    if (g_player.Level > (MAX_LEVELS - 1))
    {
        g_player.Level = 0;
    }
    StartNewLevel(g_player.Level);
}

VOID StartNewLevel(BYTE level)
{
    BYTE ceilingColor, floorColor;

    LevelDestroy();

    if (level == 0)
    {
        PlayerInit(FALSE);
    }

    LevelCreate(level);

    GetCeilingColor(level, &ceilingColor, &floorColor);
    W3DSetLevelColors(ceilingColor, floorColor);
}

VOID CompleteLevel()
{
}

VOID OnIdle()
{
    BYTE livesCount = g_player.Lives;
    UpdateInput();
    

    if (IsKeyJustPressed(VkKeyScan('i')))
    {
        if (currentViewport > 0)
        {
            W3DSetViewport(&validViewportSizes[--currentViewport]);
        }
    }
    else if (IsKeyJustPressed(VkKeyScan('u')))
    {
        if (currentViewport < ARRAYSIZE(validViewportSizes) - 1)
        {
            W3DSetViewport(&validViewportSizes[++currentViewport]);
        }
    }

    W3DClear(127);

    if (IsGamePlayScreenActive())
    {
        /* Perform some awesome gameplay stuff */
        LevelUpdate();

        if (PlayerIsDead())
        {
            g_player.Lives = max((SHORT)g_player.Lives - 1, 0);
            g_player.Health = 100;
            livesCount = g_player.Lives;
            StartNewLevel(g_player.Level);
            g_player.Lives = livesCount;
        }

        if (!g_player.Lives)
        {
            // Play death fill in animation
            ChangeScreen(OPTIONS_SCREEN);
            g_gameInProgress = FALSE;
        }

        /* Draw some awesome stuff */
        W3DDrawLevel(g_player.Position, g_player.Direction);
    }

    DrawScreenOverlays();

    W3DPresent();
}

LRESULT WINAPI WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    switch (Msg)
    {
    case WM_CLOSE:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, Msg, wParam, lParam);
    }

    return 0;
}

VOID PrintMemoryStats(PSTR tag)
{
    CHAR memStats[200];
    PROCESS_MEMORY_COUNTERS memCounters = {0};

    memCounters.cb = sizeof(memCounters);

    GetProcessMemoryInfo(GetCurrentProcess(), &memCounters, sizeof(memCounters));

    sprintf_s(memStats, ARRAYSIZE(memStats),    "MEM: %s: \n"
                                                "    Sprites:\t%4.2f KB\n"
                                                "    SoundFX:\t%4.2f KB\n"
                                                "    Music:\t\t%4.2f KB\n"
                                                "    Map:\t\t%4.2f KB\n"
                                                "    Total:\t\t%4.2f KB\n"
                                                "    WorkingSet:\t%4.2f KB\n", 
                                                tag,
                                                MemBytesUsedBy(RESOURCE_TYPE_SPRITE) / 1024.0f, 
                                                MemBytesUsedBy(RESOURCE_TYPE_SFX) / 1024.0f, 
                                                MemBytesUsedBy(RESOURCE_TYPE_MUSIC) / 1024.0f, 
                                                MemBytesUsedBy(RESOURCE_TYPE_MAP) / 1024.0f, 
                                                MemTotalBytesUsed() / 1024.0f,
                                                memCounters.WorkingSetSize / 1024.0f);


    OutputDebugString(memStats);
}
