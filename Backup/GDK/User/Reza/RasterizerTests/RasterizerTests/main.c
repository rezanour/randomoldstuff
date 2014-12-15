#include "common.h"

HWND g_hwnd = NULL;
color_t* g_pixelBuffer = NULL;
int g_screenWidth = 640;
int g_screenHeight = 480;

static const wchar_t* const c_windowClass = L"RasterizerTests";

static HDC g_memoryDC = NULL;
static HINSTANCE g_instance = NULL;

static BOOL Initialize(HINSTANCE hInstance, int nCmdShow);
static void Uninitialize();

static LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void OnIdle();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    MSG msg = {0};

    if (!Initialize(hInstance, nCmdShow))
    {
        return 1;
    }

    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            OnIdle();
        }
    }

    Uninitialize();

    return 0;
}

BOOL Initialize(HINSTANCE hInstance, int nCmdShow)
{
    WNDCLASS wc = {0};
    HDC hdc;

    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hInstance = hInstance;
    wc.lpfnWndProc = WinProc;
    wc.lpszClassName = c_windowClass;

    g_instance = hInstance;

    if (!RegisterClass(&wc))
    {
        return FALSE;
    }

    g_hwnd = CreateWindow(c_windowClass, c_windowClass, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, g_screenWidth, g_screenHeight, NULL, NULL, hInstance, NULL);
    if (!g_hwnd)
    {
        return FALSE;
    }

    UpdateWindow(g_hwnd);
    ShowWindow(g_hwnd, nCmdShow);

    hdc = GetDC(g_hwnd);
    {
        BITMAPINFO bmi = {0};
        HBITMAP bitmap;

        g_memoryDC = CreateCompatibleDC(hdc);

        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = g_screenWidth;
        bmi.bmiHeader.biHeight = -g_screenHeight;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 24;
        bmi.bmiHeader.biCompression = BI_RGB;

        bitmap = CreateDIBSection(g_memoryDC, &bmi, DIB_RGB_COLORS, (PVOID*)&g_pixelBuffer, NULL, 0);
        SelectObject(g_memoryDC, bitmap);
        DeleteObject(bitmap);
    }

    ReleaseDC(g_hwnd, hdc);

    return TRUE;
}

void Uninitialize()
{
    if (g_memoryDC)
    {
        DeleteDC(g_memoryDC);
        g_memoryDC = NULL;
    }

    UnregisterClass(c_windowClass, g_instance);
}

LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;

    switch (msg)
    {
    case WM_CLOSE:
        PostQuitMessage(0);
        break;

    default:
        result = DefWindowProc(hwnd, msg, wParam, lParam);
        break;
    }

    return result;
}

void OnIdle()
{
    RECT client;
    color_t bgColor = { 0, 0, 0 };
    color_t color = { 255, 0, 0 };
    HDC hdc = GetDC(g_hwnd);
    GetClientRect(g_hwnd, &client);

    RClearBuffer(bgColor);

    RDrawLine(100, 200, 200, 50, color);

    BitBlt(hdc, 0, 0, g_screenWidth, g_screenHeight, g_memoryDC, 0, 0, SRCCOPY);
    //StretchBlt(hdc, 0, 0, client.right - client.left, client.bottom - client.top, g_memoryDC, 0, 0, g_screenWidth, g_screenHeight, SRCCOPY);

    ReleaseDC(g_hwnd, hdc);
}
