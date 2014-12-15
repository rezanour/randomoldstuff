#include "precomp.h"
#include "graphics.h"
#include "game.h"

//
// Globals
//
static HINSTANCE g_instance;
static HWND g_gameWindow;

//
// Forward declarations
//
static void CreateGameWindow();
static void DestroyGameWindow();
static LRESULT CALLBACK GameWindowProc(_In_ HWND, _In_ UINT, _In_ WPARAM, _In_ LPARAM);

//
// WinMain
//
_Use_decl_annotations_
int CALLBACK WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
    g_instance = instance;

    try // WinMain shouldn't let exceptions escape
    {
        // Override config info in Globals, if desired...

        CreateGameWindow();

        // The following scopes have been added to ensure correct order of deletion
        {
            auto graphics(Graphics::Create(g_gameWindow));
            {
                auto game(Game::Create());

                ShowWindow(g_gameWindow, SW_SHOW);
                UpdateWindow(g_gameWindow);

                MSG msg = {};
                while (msg.message != WM_QUIT)
                {
                    if (PeekMessage(&msg, g_gameWindow, 0, 0, PM_REMOVE))
                    {
                        TranslateMessage(&msg);
                        DispatchMessage(&msg);
                    }
                    else
                    {
                        game->Tick();
                    }
                }

                // Hide window for destruction
                ShowWindow(g_gameWindow, SW_HIDE);
                UpdateWindow(g_gameWindow);

            } // game is destroyed
        } // graphics is destroyed

        DestroyGameWindow();
    }
    catch (const std::exception&)
    {
        return -1;
    }

    return 0;
}

void CreateGameWindow()
{
    WNDCLASSEX wcx = {};
    wcx.cbSize = sizeof(wcx);
    wcx.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcx.hInstance = g_instance;
    wcx.lpfnWndProc = GameWindowProc;
    wcx.lpszClassName = Globals::GameTitle;

    CHECKGLE(RegisterClassEx(&wcx));

    g_gameWindow = CreateWindow(Globals::GameTitle, Globals::GameTitle, WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME,
        CW_USEDEFAULT, CW_USEDEFAULT, Globals::ScreenWidth, Globals::ScreenHeight, nullptr, nullptr, g_instance, nullptr);

    CHECKNOTNULL(g_gameWindow);
}

void DestroyGameWindow()
{
    if (g_gameWindow)
    {
        DestroyWindow(g_gameWindow);
        g_gameWindow = nullptr;
    }

    UnregisterClass(Globals::GameTitle, g_instance);
}

_Use_decl_annotations_
LRESULT CALLBACK GameWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CLOSE:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 0;
}
