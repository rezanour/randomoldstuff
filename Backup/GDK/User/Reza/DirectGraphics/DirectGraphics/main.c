#include "common.h"
#include "graphics.h"

static const char* const c_ClassName = "DirectGraphics";

static LRESULT WINAPI WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
static void OnIdle();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    HWND hwnd;
    MSG msg = {0};
    WNDCLASS wc = {0};

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.hInstance = hInstance;
    wc.lpfnWndProc = WndProc;
    wc.lpszClassName = c_ClassName;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClass(&wc))
    {
        return 1;
    }

    hwnd = CreateWindow(c_ClassName, c_ClassName, WS_THICKFRAME | WS_BORDER | WS_SYSMENU | WS_MINIMIZE, 0, 0, 800, 600, NULL, NULL, hInstance, NULL);
    if (!hwnd)
    {
        return 2;
    }

    UpdateWindow(hwnd);
    ShowWindow(hwnd, nCmdShow);

    if (!InitializeGraphics(hwnd, 800, 600))
    {
        DestroyWindow(hwnd);
        return 3;
    }

    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    UninitializeGraphics();

    return 0;
}

LRESULT WINAPI WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    switch (Msg)
    {
    case WM_CLOSE:
        PostQuitMessage(0);
        break;

    case WM_PAINT:
        GraphicsRefresh();
        break;

    default:
        return DefWindowProc(hWnd, Msg, wParam, lParam);
    }

    return 0;
}

void OnIdle()
{
}
