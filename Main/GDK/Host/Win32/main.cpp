#include <Windows.h>
#include <GDK.h>
#include <RunningGame.h>
#include <InputHost.h>
#include <Quake2.h>

#include "Timer.h"
#include "Window.h"

// Container to hold all our data together
struct HostContext
{
    HostContext() : window(nullptr), game(nullptr), graphicsDevice(nullptr), wantsToClose(false), audioDevice(nullptr) { }

    std::shared_ptr<Window> window;
    std::shared_ptr<GDK::IRunningGame> game;
    std::shared_ptr<GDK::GraphicsDevice> graphicsDevice;
    std::shared_ptr<GDK::AudioDevice> audioDevice;
    bool wantsToClose;
};

static void RunGame(_In_ HINSTANCE hInstance, _In_ int nCmdShow);
static LRESULT CALLBACK WndProc(_In_ HWND, _In_ UINT, _In_ WPARAM, _In_ LPARAM);
static void FixCurrentDirectory();

_Use_decl_annotations_
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    try
    {
        RunGame(hInstance, nCmdShow);
        return 0;
    }
    catch (const std::exception& e)
    {
        OutputDebugStringA(e.what());
        return -1;
    }
}

_Use_decl_annotations_
void RunGame(HINSTANCE hInstance, int nCmdShow)
{
    // Initialize the runtime
    GDK::Startup();

    // Initialize the game
    Quake2::Initialize();

    // Ensure working directory is location of executable
    FixCurrentDirectory();

    const uint32_t width = 1280;
    const uint32_t height = 720;

    // Mount a "content" directory in the same directory as the exe as our default volume for finding files
    GDK::FileSystem::MountVolume(L"content", L"", 100);

    // If we're F5'ed we may be able to go find the content out in the Quake2 directory
    GDK::FileSystem::MountVolume(L"../../../../Games/Quake2/content", L"", 0);

    // Initialize the host data
    HostContext context;
    context.window = Window::Create(Window::CreateParameters(hInstance, WndProc, L"Quake 2", width, height));
    context.graphicsDevice = GDK::GraphicsDevice::Create(GDK::GraphicsDevice::CreateParameters(
        GDK::GraphicsDevice::Type::DirectX,
        context.window->GetHWND(),
        width,
        height));

    context.audioDevice = GDK::AudioDevice::Create(GDK::AudioDevice::Type::OpenAL);

    GDK::DeviceContext deviceContext;
    deviceContext.graphicsDevice = context.graphicsDevice;
    deviceContext.audioDevice = context.audioDevice;
    context.game = GDK::Game::Create(deviceContext);

    // Attach the context to the window so the message pump can access it without making it global
    SetProp(context.window->GetHWND(), L"HostContext", reinterpret_cast<HANDLE>(&context));

    // Show the window
    context.window->Show(nCmdShow);

    // Create a timer to help our game loop
    Timer timer;

    ShowCursor(FALSE);
    GDK::Input::SetWindow(context.window->GetHWND());
    GDK::Input::LockMouseCursor();

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

        // Update the game
        if (context.game->Update(GDK::GameTime(timer.GetElapsedTime(), timer.GetTotalTime())) == GDK::UpdateResult::Exit)
        {
            context.wantsToClose = true;
        }

        context.graphicsDevice->Clear(GDK::Vector4(0.5f, 0.5f, 0.5f, 1.0f));
        context.graphicsDevice->ClearDepth(1.0f);

        // Render the game
        context.game->Draw();

        context.graphicsDevice->Present();
    }

    ShowCursor(TRUE);
    GDK::Input::UnlockMouseCursor();

    // Shutdown the runtime
    GDK::Shutdown();
}

_Use_decl_annotations_
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    HostContext* hostContext = reinterpret_cast<HostContext *>(GetProp(hwnd, L"HostContext"));

    switch (msg)
    {
        case WM_KEYDOWN:
            {
                GDK::Button button;
                if (GDK::Input::GetButtonFromWin32Keycode(static_cast<byte_t>(wParam), &button))
                {
                    GDK::Input::OnButtonDown(button);
                }
            }
            break;
        case WM_KEYUP:
            {
                GDK::Button button;
                if (GDK::Input::GetButtonFromWin32Keycode(static_cast<byte_t>(wParam), &button))
                {
                    GDK::Input::OnButtonUp(button);
                }
            }
            break;
        case WM_LBUTTONDOWN: 
            GDK::Input::OnButtonDown(GDK::Button::MouseLeft);
            break;
        case WM_LBUTTONUP:
            GDK::Input::OnButtonUp(GDK::Button::MouseLeft); 
            break;
        case WM_RBUTTONDOWN: 
            GDK::Input::OnButtonDown(GDK::Button::MouseRight); 
            break;
        case WM_RBUTTONUP: 
            GDK::Input::OnButtonUp(GDK::Button::MouseRight); 
            break;
        case WM_MBUTTONDOWN: 
            GDK::Input::OnButtonDown(GDK::Button::MouseMiddle); 
            break;
        case WM_MBUTTONUP: 
            GDK::Input::OnButtonUp(GDK::Button::MouseMiddle); 
            break;
        case WM_CLOSE:
            if (hostContext != nullptr)
            {
                hostContext->wantsToClose = true;
                return 0;
            }
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 0;
}

void FixCurrentDirectory()
{
    // Set the current working directory to the location of the host executable
    HMODULE hModule = GetModuleHandleW(NULL);
    wchar_t path[MAX_PATH] = {0};
    GetModuleFileNameW(hModule, path, MAX_PATH);
    int i = 0;
    for (; i < MAX_PATH && path[i] != '\0'; i++);
    for (; i >= 0 && path[i] != '\\'; i--);
    if (i >= 0 && i < MAX_PATH)
    {
        path[i] = '\0';
    }

    if (!SetCurrentDirectoryW(path))
    {
        throw std::exception();
    }
}
