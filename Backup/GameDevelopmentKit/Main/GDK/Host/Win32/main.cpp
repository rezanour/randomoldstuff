#include <Windows.h>
#include <GDK.h>
#include <GDKError.h>
#include <Quake2Game.h>
#include <Content.h>

#include "Timer.h"
#include "Window.h"

// Container to hold all our data together
struct HostContext
{
    HostContext() : window(nullptr), game(nullptr), graphicsDevice(nullptr), wantsToClose(false) { }

    std::shared_ptr<Window> window;
    std::shared_ptr<GDK::Game> game;
    std::shared_ptr<GDK::GraphicsDevice> graphicsDevice;
    bool wantsToClose;
};

static LRESULT CALLBACK WndProc(_In_ HWND, _In_ UINT, _In_ WPARAM, _In_ LPARAM);

_Use_decl_annotations_
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    const uint32_t width = 1280;
    const uint32_t height = 720;
    
    // Initialize the host data
    HostContext context;
    context.window = Window::Create(Window::CreateParameters(hInstance, WndProc, L"Quake 2", width, height));
    context.graphicsDevice = GDK::GraphicsDevice::Create(GDK::GraphicsDevice::CreateParameters(
        GDK::GraphicsDevice::Type::DirectX,
        context.window->GetHWND(),
        width,
        height));
    context.game = Quake2::Quake2Game::Create(GDK::Game::CreateParameters());

    // Attach the context to the window so the message pump can access it without making it global
    SetProp(context.window->GetHWND(), L"HostContext", reinterpret_cast<HANDLE>(&context));

    // Show the window
    context.window->Show(nCmdShow);

    // Create a timer to help our game loop
    Timer timer;

    while (!context.wantsToClose)
    {
        // Handle Windows messages
        MSG msg;
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Early out of this frame if the game is done due to a message
        if (context.wantsToClose)
            break;

        // Tick the timer
        timer.Tick();

        context.graphicsDevice->Clear(GDK::Vector4(1, 0, 0, 1));

        context.graphicsDevice->Present();
    }

    return 0;
}

_Use_decl_annotations_
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    HostContext* hostContext = reinterpret_cast<HostContext *>(GetProp(hwnd, L"HostContext"));

    switch (msg)
    {
        case WM_CLOSE:
            if (hostContext != nullptr)
            {
                hostContext->wantsToClose = true;
            }
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 0;
}