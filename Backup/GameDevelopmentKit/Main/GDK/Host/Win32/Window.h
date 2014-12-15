#pragma once

#include <Windows.h>
#include <NonCopyable.h>


// Simple wrapper on an HWND to make management a little easier
class Window : GDK::NonCopyable
{
public:
    struct CreateParameters
    {
        CreateParameters() : hInstance(nullptr), wndProc(nullptr), title(L""), width(0), height(0) { }
        CreateParameters(_In_ HINSTANCE hInstance, _In_ WNDPROC wndProc, _In_ LPCWSTR title, _In_ uint32_t width, _In_ uint32_t height) :
            hInstance(hInstance), wndProc(wndProc), title(title), width(width), height(height) { }

        HINSTANCE hInstance;
        WNDPROC wndProc;
        LPCWSTR title;
        uint32_t width;
        uint32_t height;
    };

    static std::shared_ptr<Window> Create(_In_ const CreateParameters& parameters)
    {
        LPCWSTR windowClassName = L"GDKWindowClass";

        // register the window class
        WNDCLASSEX wc;
        ZeroMemory(&wc, sizeof(wc));
        wc.cbSize = sizeof(WNDCLASSEX);
        wc.style = CS_BYTEALIGNCLIENT | CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = parameters.wndProc;
        wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
        wc.lpszMenuName = nullptr;
        wc.lpszClassName = windowClassName;
        wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
        CHECK_TRUE(RegisterClassEx(&wc));

        // create the window
        DWORD windowStyle = WS_POPUPWINDOW | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
        DWORD windowStyleEx = WS_EX_APPWINDOW;
        RECT windRect;
        windRect.left = 0;
        windRect.top = 0;
        windRect.right = parameters.width;
        windRect.bottom = parameters.height;
        AdjustWindowRectEx(&windRect, windowStyle, false, windowStyleEx);
        int windowWidth = windRect.right - windRect.left;
        int windowHeight = windRect.bottom - windRect.top;
        int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        int screenHeight = GetSystemMetrics(SM_CYSCREEN);
        int windowX = screenWidth / 2 - windowWidth / 2;
        int windowY = screenHeight / 2 - windowHeight / 2;

        HWND window = CreateWindowEx(
            windowStyleEx, 
            windowClassName, 
            parameters.title, 
            windowStyle, 
            windowX,
            windowY, 
            windowWidth, 
            windowHeight, 
            nullptr, 
            nullptr, 
            parameters.hInstance, 
            nullptr);
        CHECK_NOT_NULL(window);

        return std::shared_ptr<Window>(new Window(window));
    }

    ~Window()
    {
        if (_window != nullptr)
        {
            CloseWindow(_window);
            DestroyWindow(_window);
        }
    }

    void Show(_In_ int showWindow)
    {
        ShowWindow(_window, showWindow);
        UpdateWindow(_window);
    }

    HWND GetHWND() { return _window; }
        
private:
    Window(_In_ HWND window) : _window(window) { }

    HWND _window;
};