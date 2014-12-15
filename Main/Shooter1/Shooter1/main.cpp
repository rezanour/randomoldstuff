#include "precomp.h"

static HWND CreateClassicWindow();
static LRESULT CALLBACK WndProc(_In_ HWND hwnd, _In_ UINT msg, _In_ WPARAM wParam, _In_ LPARAM lParam);

//#define ONSCREEN_MEMORY_STATS

_Use_decl_annotations_
int CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    MemoryTrackerStartup();

    ConfigLoad();

    HWND hwnd = CreateClassicWindow();
    GraphicsStartup(hwnd);
    AnimationStartup();
    AssetLoaderStartup();
    InputStartup();
    GameStartup();

    // Don't show window until we're ready to start drawing to it
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    MSG msg;
    size_t allocsLastFrame = 0;
    auto& input = GetInput();
    auto& game = GetGame();
    auto& graphics = GetGraphics();
    auto& memoryTracker = GetMemoryTracker();

#if defined(ONSCREEN_MEMORY_STATS)
    auto& font = game.GetDebugFont();
    XMFLOAT4 white(1, 1, 1, 1);
#endif

    do
    {
        if (PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            allocsLastFrame = memoryTracker.StartNewFrame();

            input.Tick();
            if (!game.Tick())
            {
                break;
            }

#if defined(ONSCREEN_MEMORY_STATS)
            font.DrawFullScreen(POINT { 0, 20 }, white, "Allocs per Frame: %I64u", allocsLastFrame);
            POINT p = { 0, 40 };
            font.DrawFullScreen(p, white, "MemoryStats:");
            for (uint32_t i = 0; i < _countof(memoryTracker.TotalSize); ++i)
            {
                p.y += 20;
                font.DrawFullScreen(p, white, "  %s - %I64u", GetTagString((MemoryTag)i), memoryTracker.TotalSize[i]);
            }
#endif

            graphics.Present();
        }
    } while (msg.message != WM_QUIT);

    GameShutdown();
    InputShutdown();
    AssetLoaderShutdown();
    AnimationShutdown();
    GraphicsShutdown();
    ConfigDestroy();
    DestroyWindow(hwnd);

    GetMemoryTracker().ReportMemUsage();
    MemoryTrackerShutdown();

    return 0;
}

HWND CreateClassicWindow()
{
    WNDCLASSEX wcx = {};
    HINSTANCE instance = (HINSTANCE)GetModuleHandle(nullptr);
    auto& config = GetConfig();

    wcx.cbSize = sizeof(wcx);
    wcx.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcx.hInstance = instance;
    wcx.lpfnWndProc = WndProc;
    wcx.lpszClassName = config.Name;

    // According to MSDN, all classes are automatically unregistered on process exit,
    // so there's no need to call UnregisterClass during cleanup
    CHECKGLE(RegisterClassEx(&wcx));

    HWND hwnd = CreateWindow(config.Name, config.Name, WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME,
        CW_USEDEFAULT, CW_USEDEFAULT, config.ScreenWidth, config.ScreenHeight,
        nullptr, nullptr, instance, nullptr);

    CHECKNOTNULL(hwnd);

    return hwnd;
}

_Use_decl_annotations_
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
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
