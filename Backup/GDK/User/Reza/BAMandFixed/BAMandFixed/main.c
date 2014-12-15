#include "common.h"

static PCWSTR const DEFAULT_TITLE = L"GameTitle";
static const int DEFAULT_SCREENX = 800;
static const int DEFAULT_SCREENY = 600;

static HWND g_gameWindow = NULL;

static LRESULT WINAPI GameWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
static BOOL GameInitialize();
static void GameUninitialize();
static void GameTick();
static void GameExit();
static void GameRender();
static float GameGetTargetElapsedTime() { 1.0f / 60.0f; }
HWND GameGetWindow() { return g_gameWindow; }

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    MSG msg;
    WNDCLASS wc;

    ZeroMemory(&msg, sizeof(msg));
    ZeroMemory(&wc, sizeof(wc));

    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.hInstance = hInstance;
    wc.lpfnWndProc = GameWndProc;
    wc.lpszClassName = L"GameWindowClass";
    wc.hIcon = NULL;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    if (!RegisterClass(&wc))
    {
        return 1;
    }

    g_gameWindow = CreateWindow(wc.lpszClassName, DEFAULT_TITLE, WS_THICKFRAME | WS_BORDER | WS_SYSMENU | WS_MINIMIZE, 0, 0, DEFAULT_SCREENX, DEFAULT_SCREENY, NULL, NULL, hInstance, NULL);
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

    return 0;
}

BOOL GameInitialize()
{
    if (!GraphicsInitialize(DEFAULT_SCREENX, DEFAULT_SCREENY))
    {
        return FALSE;
    }

    if (!InputInitialize())
    {
        return FALSE;
    }

    // TESTS
    {
        fixed_t a = Fixed(-5.5);
        fixed_t b = Fixed(10.25);
        fixed_t c = FixedDiv(a, b);
        float f = FixedFloat(c);
        short whole = FixedWhole(c);
        float frac = FixedFrac(c);
        angle_t ang = Angle(Fixed(PI_RADIANS));
        wprintf(L"number = %d.%d\n", whole, frac);
    }

    return TRUE;
}

void GameUninitialize()
{
    InputUninitialize();
    GraphicsUninitialize();
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
        GameExit();
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
        return DefWindowProc(hWnd, Msg, wParam, lParam);
    }
    return 0;
}

void GameExit()
{
    PostQuitMessage(0);
}

void GameRender()
{
    GraphicsClear();
    GraphicsPresent();
}