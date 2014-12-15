#include "precomp.h"
#include "renderer.h"

//==============================================================================

static const wchar_t CLASS_NAME[] = L"Deferred Renderer Test";

static HWND g_hwnd;
static uint32_t g_screenWidth = 1280;
static uint32_t g_screenHeight = 720;

//==============================================================================

static void Initialize(_In_ HINSTANCE instance);
static void Shutdown();
static LRESULT CALLBACK WndProc(_In_ HWND hwnd, _In_ UINT msg, _In_ WPARAM wParam, _In_ LPARAM lParam);

//==============================================================================

_Use_decl_annotations_
int WINAPI WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int showCommand)
{
    UNREFERENCED_PARAMETER(prevInstance);
    UNREFERENCED_PARAMETER(commandLine);

    Initialize(instance);

    std::shared_ptr<Renderer> renderer(std::make_shared<Renderer>(g_hwnd, g_screenWidth, g_screenHeight));

    ShowWindow(g_hwnd, showCommand);
    UpdateWindow(g_hwnd);

    MSG msg = {};

    for (;;)
    {
        if (PeekMessage(&msg, g_hwnd, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                break;
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            renderer->RenderFrame();
        }
    }

    Shutdown();

    return 0;
}

//==============================================================================

_Use_decl_annotations_
void Initialize(HINSTANCE instance)
{
    assert(g_hwnd == nullptr);

    WNDCLASSEX wcx = {};
    wcx.cbSize = sizeof(wcx);
    wcx.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    wcx.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcx.hInstance = instance;
    wcx.lpfnWndProc = WndProc;
    wcx.lpszClassName = CLASS_NAME;

    ATOM atom = RegisterClassEx(&wcx);
    if (atom == 0)
    {
        throw std::exception("RegisterClassEx failed!");
    }

    g_hwnd = CreateWindow(CLASS_NAME, CLASS_NAME, WS_POPUPWINDOW | WS_CAPTION | WS_SYSMENU,
        CW_USEDEFAULT, CW_USEDEFAULT, g_screenWidth, g_screenHeight, nullptr, nullptr, instance, nullptr);
    if (g_hwnd == nullptr)
    {
        throw std::exception("CreateWindow failed!");
    }
}

void Shutdown()
{
    if (g_hwnd != nullptr)
    {
        DestroyWindow(g_hwnd);
        g_hwnd = nullptr;
    }

    UnregisterClass(CLASS_NAME, nullptr);
}

_Use_decl_annotations_
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CLOSE:
        PostQuitMessage(0);
        return 0;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}
