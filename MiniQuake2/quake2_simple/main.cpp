#include "stdafx.h"

static HWND
InitializeAndCreateWindow(
    HINSTANCE instance,
    int screenWidth,
    int screenHeight
    );

static LRESULT CALLBACK
Quake2WndProc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam
    );

static void
HandleInput();

static GDK::Vector3 g_position;
static float g_rotation = 0;

int WINAPI
WinMain(
    HINSTANCE instance,
    HINSTANCE prevInstance,
    LPSTR commandLine,
    int showCommand
    )
{
    PakInit(L"C:\\GDK\\Games\\Quake2\\content\\Pak\\Pak0.pak");

    HWND hwnd = InitializeAndCreateWindow(instance, 1280, 720);

    UpdateWindow(hwnd);
    ShowWindow(hwnd, showCommand);

    BspLoad("maps/base1.bsp");

    g_position.y = 30;

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
            HandleInput();

            // Draw
            Render(g_position, g_rotation);
        }
    }

    RenderShutdown();
    PakShutdown();

    return 0;
}

HWND
InitializeAndCreateWindow(
    HINSTANCE instance,
    int screenWidth,
    int screenHeight
    )
{
    static const PCWSTR ClassName = L"Quake2Window";
    static const PCWSTR WindowTitle = L"Quake2 Simple";

    WNDCLASSEX wex = {};

    wex.cbSize = sizeof(wex);
    wex.lpszClassName = ClassName;
    wex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wex.hInstance = instance;
    wex.lpfnWndProc = Quake2WndProc;

    if (!RegisterClassEx(&wex))
    {
        RIP(L"Error registering window class. %d\n", GetLastError());
    }

    HWND hwnd = CreateWindow(ClassName, WindowTitle, WS_POPUP | WS_CAPTION | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, screenWidth, screenHeight, nullptr, nullptr, instance, nullptr);
    if (hwnd == nullptr)
    {
        RIP(L"Error creating the window. %d\n", GetLastError());
    }

    RenderInit(hwnd, screenWidth, screenHeight);

    return hwnd;
}

LRESULT CALLBACK
Quake2WndProc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam
    )
{
    if (msg == WM_CLOSE)
    {
        DestroyWindow(hwnd);
        return 0;
    }
    else if (msg == WM_DESTROY)
    {
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void
HandleInput()
{
    const float speed = 10.0f;
    const float rotSpeed = 0.1f;
    const SHORT keydown = 1 << 15;

    if (GetAsyncKeyState(VK_ESCAPE) & keydown)
    {
        PostQuitMessage(0);
    }

    // Translation

    GDK::Matrix rot = GDK::Matrix::CreateRotationY(g_rotation);

    GDK::Vector3 movement;

    if (GetAsyncKeyState('D') & keydown)
    {
        movement.x += speed;
    }
    if (GetAsyncKeyState('A') & keydown)
    {
        movement.x -= speed;
    }
    if (GetAsyncKeyState('W') & keydown)
    {
        movement.z += speed;
    }
    if (GetAsyncKeyState('S') & keydown)
    {
        movement.z -= speed;
    }

    if (movement.LengthSquared() > 0)
    {
        movement = GDK::Vector3::Normalize(movement.x * rot.GetRight() + movement.z * rot.GetForward()) * speed;
    }

    g_position += movement;

    // Rotation

    float rotation = 0;
    if (GetAsyncKeyState(VK_LEFT) & keydown)
    {
        rotation -= rotSpeed;
    }
    if (GetAsyncKeyState(VK_RIGHT) & keydown)
    {
        rotation += rotSpeed;
    }

    g_rotation += rotation;
}
