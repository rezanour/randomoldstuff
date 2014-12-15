#include "common.h"
#include "mainwindow.h"

static const wchar_t* const c_mainWindowClassName = L"QuickPaint Main Window";

static ATOM g_classAtom = 0;
static HINSTANCE g_instance = NULL;
static HWND g_mainWindow = NULL;

static int g_windowWidth = 1024;
static int g_windowHeight = 768;

//
// private main window method declarations
//

static LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void OnPaint();


//
// public main window methods
//

BOOL CreateMainWindow(HINSTANCE hInstance, int nCmdShow)
{
    DebugOut(L"Creating main window...\n");

    if (g_instance && hInstance != g_instance)
    {
        DebugOut(L"Creating main window again from a different instance. This is not supported.\n");
        return FALSE;
    }

    g_instance = hInstance;

    if (g_classAtom)
    {
        DebugOut(L"Class registration already exists. Reusing...\n");
    }
    else
    {
        WNDCLASS wc = {0};

        wc.lpszClassName = c_mainWindowClassName;
        wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wc.hInstance = hInstance;
        wc.lpfnWndProc = MainWindowProc;

        g_classAtom = RegisterClass(&wc);
        if (!g_classAtom)
        {
            DebugOut(L"Failed to create class registration.\n");
            return FALSE;
        }
    }

    if (g_mainWindow)
    {
        DebugOut(L"Main window already exists. Returning...\n");
        return TRUE;
    }

    g_mainWindow = CreateWindow(c_mainWindowClassName, L"untitled", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, g_windowWidth, g_windowHeight, NULL, NULL, hInstance, NULL);
    if (!g_mainWindow)
    {
        DebugOut(L"Failed to create window handle.\n");
        return FALSE;
    }

    UpdateWindow(g_mainWindow);
    ShowWindow(g_mainWindow, nCmdShow);

    DebugOut(L"Main window created successfully.\n");
    return TRUE;
}

void DestroyMainWindow()
{
    if (g_mainWindow)
    {
        DestroyWindow(g_mainWindow);
        g_mainWindow = NULL;
    }

    if (g_classAtom)
    {
        UnregisterClass((LPCWSTR)(UINT)g_classAtom, g_instance);
        g_classAtom = 0;
    }

    g_instance = NULL;
}


//
// private main window methods
//

LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;

    switch (msg)
    {
    case WM_CLOSE:
        PostQuitMessage(0);
        break;

    case WM_PAINT:
        OnPaint();
        break;

    default:
        result = DefWindowProc(hwnd, msg, wParam, lParam);
        break;
    }

    return result;
}

void OnPaint()
{
    PAINTSTRUCT ps;
    BeginPaint(g_mainWindow, &ps);

    EndPaint(g_mainWindow, &ps);
}
