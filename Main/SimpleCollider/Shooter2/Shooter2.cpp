#include "Precomp.h"

static const wchar_t GameName[] = L"Shooter2";
static const uint32_t ScreenWidth = 1280;
static const uint32_t ScreenHeight = 720;

static HWND CreateGameWindow(_In_ HINSTANCE instance, uint32_t width, uint32_t height);
static LRESULT CALLBACK WndProc(_In_ HWND hwnd, uint32_t msg, WPARAM wParam, LPARAM lParam);

_Use_decl_annotations_
int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
    HWND hwnd = nullptr;

    // Can't let exceptions escape WinMain
    try
    {
        hwnd = CreateGameWindow(instance, ScreenWidth, ScreenHeight);

        RendererConfig config = {};
        config.Window = hwnd;
        config.Width = ScreenWidth;
        config.Height = ScreenHeight;
        config.DebugDevice = false;

        std::unique_ptr<Renderer> renderer(new Renderer(config));

        {
            // Ensure game is destroyed before renderer
            std::unique_ptr<Game> game(new Game(renderer.get()));

            ShowWindow(hwnd, SW_SHOW);
            UpdateWindow(hwnd);

            MSG msg = {};
            while (msg.message != WM_QUIT)
            {
                if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
                else
                {
                    game->Tick();
                }
            }
        }
    }
    catch (const std::exception&)
    {
    }

    DestroyWindow(hwnd);
    return 0;
}

_Use_decl_annotations_
HWND CreateGameWindow(HINSTANCE instance, uint32_t width, uint32_t height)
{
    WNDCLASSEX wcx = {};
    wcx.cbSize = sizeof(wcx);
    wcx.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    wcx.hInstance = instance;
    wcx.lpfnWndProc = WndProc;
    wcx.lpszClassName = GameName;

    if (!RegisterClassEx(&wcx))
    {
        throw std::exception();
    }

    DWORD style = WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);

    RECT rc = { 0, 0, width, height };
    AdjustWindowRect(&rc, style, FALSE);

    HWND hwnd = CreateWindow(GameName, GameName, style, CW_USEDEFAULT, CW_USEDEFAULT,
        rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, instance, nullptr);

    if (!hwnd) throw std::exception();

    return hwnd;
}

_Use_decl_annotations_
LRESULT CALLBACK WndProc(HWND hwnd, uint32_t msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CLOSE:
        PostQuitMessage(0);
        break;

    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE)
        {
            PostQuitMessage(0);
        }
        break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}
