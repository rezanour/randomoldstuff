#include "Precomp.h"
#include "VRES.h"

using namespace VRES;

_Use_decl_annotations_
Application::Application(const wchar_t* appName) :
    _hwnd(nullptr), _appName(appName)
{
    QueryPerformanceFrequency(&_frequency);
    ZeroMemory(&_prevTime, sizeof(_prevTime));
}

Application::~Application()
{
    if (_hwnd)
    {
        DestroyWindow(_hwnd);
        _hwnd = nullptr;
    }
}

const std::wstring& Application::AppName() const
{
    return _appName;
}

HWND Application::Window() const
{
    return _hwnd;
}

bool Application::CreateAppWindow(const Point& position, const Size& size)
{
    WNDCLASSEX wcx = {};
    wcx.cbSize = sizeof(wcx);
    wcx.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcx.hInstance = (HINSTANCE)GetModuleHandle(nullptr);
    wcx.lpfnWndProc = s_WndProc;
    wcx.lpszClassName = _appName.c_str();

    if (!RegisterClassEx(&wcx))
    {
        return false;
    }

    LONG style = WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
    RECT rc = { position.X, position.Y, position.X + size.Width, position.Y + size.Height };
    AdjustWindowRect(&rc, style, FALSE);

    _hwnd = CreateWindowW(_appName.c_str(), _appName.c_str(), style,
        rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr,
        (HINSTANCE)GetModuleHandle(nullptr), nullptr);

    if (!_hwnd)
    {
        return false;
    }

    return true;
}

void Application::Run()
{
    if (!_hwnd)
    {
        assert(false);
        return;
    }

    ShowWindow(_hwnd, SW_SHOW);
    UpdateWindow(_hwnd);

    MSG msg = {};
    LARGE_INTEGER time;
    float elapsedSeconds;

    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            QueryPerformanceCounter(&time);
            if (_prevTime.QuadPart == 0)
            {
                _prevTime = time;
            }

            elapsedSeconds = (time.QuadPart - _prevTime.QuadPart) / (float)_frequency.QuadPart;
            _prevTime = time;

            if (!Tick(elapsedSeconds))
            {
                break;
            }
        }
    }
}

LRESULT CALLBACK Application::s_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CLOSE:
        PostQuitMessage(0);
        break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}
