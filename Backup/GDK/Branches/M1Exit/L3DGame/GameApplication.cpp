#include "StdAfx.h"

#include <Shlwapi.h>

#include "GameApplication.h"
#include "ScreenManager.h"
#include "SubsystemManager.h"

#include "GDKTypes.h"

using namespace Lucid3D;
using namespace CoreServices;
using namespace std;
using namespace GDK;

const wstring GameApplication::ClassName(L"L3DGame");

GameApplication::GameApplication(_In_ ConfigurationPtr& spConfiguration)
    : _spConfiguration(spConfiguration), _hwnd(nullptr)
{
    Assert(spConfiguration);
    _spScreenManager.reset(new ScreenManager(this));
    _spSubsystemManager.reset(new SubsystemManager);
}

GameApplication::~GameApplication()
{
    // Force close ContentManager before other subsystems
    _spContentManager->UnloadAndReleaseAll();
    _spContentManager = nullptr;
}

HRESULT GameApplication::Run()
{
    HRESULT hr = S_OK;

    MSG msg = {0};

    CHECKHR(Initialize());

    CHECKHR(FirstUpdate());

    while (SUCCEEDED(hr) && msg.message != WM_QUIT)
    {
        if (PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            hr = OnIdle();
        }
    }

    Shutdown();

EXIT
    return hr;
}

void GameApplication::Exit()
{
    PostQuitMessage(0);
}

HRESULT GameApplication::Initialize()
{
    HRESULT hr = S_OK;

    HINSTANCE hInstance = GetModuleHandle(nullptr);

    GraphicsSettings settings = {0};

    WNDCLASSEX wcx = {0};
    wcx.cbSize = sizeof(wcx);
    wcx.hInstance = hInstance;
    wcx.lpszClassName = ClassName.c_str();
    wcx.lpfnWndProc = WindowProc;
    wcx.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcx.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcx.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    ISTRUE(RegisterClassEx(&wcx), E_FAIL);

    int width = _spConfiguration->GetValue("ScreenWidth", 1280);
    int height = _spConfiguration->GetValue("ScreenHeight", 720);
    DebugInfo("Requested Screen Dimensions: %dx%d", width, height);

    _hwnd = CreateWindow(   ClassName.c_str(), ClassName.c_str(), 
                            WS_POPUP | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
                            CW_USEDEFAULT, CW_USEDEFAULT, 
                            width, height, 
                            nullptr, nullptr, hInstance, nullptr );

    ISNOTNULL(_hwnd, HRESULT_FROM_WIN32(GetLastError()));

    CHECKHR(ContentManager::Create(_spConfiguration->GetStringValue("ContentRoot", ""), _spContentManager));

    // Create graphics subsystem
    settings.DefaultView.WindowIdentity = _hwnd;
    settings.DefaultView.IsFullScreen = false;
    settings.DefaultView.ScreenWidth = width;
    settings.DefaultView.ScreenHeight = height;

    CHECKHR(_spSubsystemManager->CreateRenderer(_spConfiguration->GetStringValue("GraphicsSystem", "L3DGraphics.dll"), settings));
    CHECKHR(_spSubsystemManager->GetRenderer()->SetContentManager(_spContentManager));

    ShowWindow(_hwnd, SW_SHOW);
    UpdateWindow(_hwnd);

EXIT
    return hr;
}

void GameApplication::Shutdown()
{
    if (_hwnd)
        DestroyWindow(_hwnd);

    if (!UnregisterClass(ClassName.c_str(), GetModuleHandle(nullptr)))
    {
        DebugWarning("UnregisterClass of %S failed with %d", ClassName.c_str(), GetLastError());
    }
}

HRESULT GameApplication::OnIdle()
{
    return RunGame();
}

HRESULT GameApplication::FirstUpdate()
{
    HRESULT hr = S_OK;

    uint64 startingScreen = _spConfiguration->GetValue<uint64>("StartingScreen", 0);

    CHECKHR(_spScreenManager->ChangeScreen(startingScreen));

EXIT
    return hr;
}

HRESULT GameApplication::RunGame()
{
    return _spScreenManager->Update();
}

LRESULT CALLBACK GameApplication::WindowProc(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

