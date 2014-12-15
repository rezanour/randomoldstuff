#include "tinyw3d.h"

/* HACK HACK: We use windows as our host, so we have to bring in some Win code */
#include <Windows.h>

static HWND g_hwnd;

static const char* const ClassName = "TinyW3D";
static const char* const Title = "Tiny Wolfenstein 3D";

static const short ViewScale = 8;

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int showCommand)
{
    WNDCLASS wndClass = {0};
    MSG msg;

    /* Register window class */
    wndClass.hInstance = instance;
    wndClass.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
    wndClass.lpszClassName = ClassName;
    wndClass.lpfnWndProc = WndProc;
    if (!RegisterClass(&wndClass))
        exit(1);

    /* Create window */
    g_hwnd = CreateWindow(ClassName, Title, WS_POPUP, 0, 0, 160 * ViewScale, 100 * ViewScale, 0, 0, instance, 0);
    if (!g_hwnd)
        exit(2);

    ShowWindow(g_hwnd, showCommand);
    UpdateWindow(g_hwnd);

    /* Initialize Graphics */
    InitializeGraphics(g_hwnd);
    InitializeGame();

    for (;;)
    {
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                break;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            TickGame();
            Present();
        }
    }

    DestroyGraphics();

    return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_ESCAPE:
            PostQuitMessage(0);
            break;

        case VK_LEFT:
            TurnPlayer(-500);
            break;

        case VK_RIGHT:
            TurnPlayer(500);
            break;

        case VK_UP:
            MovePlayer(5);
            break;

        case VK_DOWN:
            MovePlayer(-5);
            break;

        case VK_TAB:
            ToggleDebug();
            break;
        }

        break;

    case WM_CLOSE:
        DestroyWindow(g_hwnd);
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}
