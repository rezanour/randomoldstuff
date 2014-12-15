#include "StdAfx.h"

using GDK::Application;

Application* Application::s_instance = nullptr;
const wchar_t* const Application::s_ClassName = L"GDK Game";

GDK_METHOD_(void) Application::Create(_In_ HINSTANCE hInstance, _In_opt_ const wchar_t* configurationFile, _Out_ Application** application)
{
    assert(application);
    *application = nullptr;

    assert(!s_instance);

    *application = new Application;

    (*application)->_Initialize(hInstance, configurationFile);
    s_instance = *application;
}

Application::Application()
    : _hwnd(nullptr), _engine(nullptr), _debugPrintEnabled(true)
{
    assert(!s_instance && "Trying to create a new instance of an existing singleton");
}

Application::~Application()
{
    _Uninitialize();
    SafeRelease(_engine);
}

GDK_METHOD_(void) Application::Run()
{
    // Initial update?

    ShowWindow(_hwnd, SW_SHOW);
    UpdateWindow(_hwnd);

    MSG msg = {0};
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            _OnIdle();
        }
    }
}

GDK_IMETHODIMP_(GDK::HostType) Application::GetType() const
{
    return HostType_Windows;
}

GDK_IMETHODIMP_(void) Application::Exit(_In_ uint32_t code)
{
    PostQuitMessage(code);
}

GDK_IMETHODIMP_(void) Application::EnableDebugPrint(_In_ bool enabled)
{
    _debugPrintEnabled = enabled;
}

GDK_IMETHODIMP_(void) Application::SetLogFile(_In_opt_ const wchar_t* name)
{
    UNREFERENCED_PARAMETER(name);
}

void __cdecl Application::LogMessageF(_In_ const wchar_t* format, ...)
{
    static wchar_t message[1000] = {0};

    va_list args;
    va_start(args, format);
    vswprintf_s(message, format, args);
    va_end(args);

    LogMessage(message);
}

GDK_IMETHODIMP_(void) Application::LogMessage(_In_ const wchar_t* message)
{
    if (_debugPrintEnabled)
    {
        DebugString(message);
    }

    // TODO: logging to file, etc...
}

GDK_IMETHODIMP_(void) Application::CreateStorageItem(_In_ const wchar_t* storageItem, _Deref_out_ IStream** stream)
{
    CHK(SHCreateStreamOnFile(storageItem, STGM_WRITE | STGM_CREATE, stream));
}

GDK_IMETHODIMP_(void) Application::LoadStorageItem(_In_ const wchar_t* storageItem, _Deref_out_ IStream** stream)
{
    GLEB(PathFileExists(storageItem));
    CHK(SHCreateStreamOnFile(storageItem, STGM_READ, stream));
}

GDK_METHOD_(void) Application::_Initialize(_In_ HINSTANCE hInstance, _In_opt_ const wchar_t* configurationFile)
{
    assert(!_engine);

    _CreateClassAndWindow(hInstance);
    _CreateEngine(configurationFile);
}

GDK_METHOD_(void) Application::_Uninitialize()
{
    SafeRelease(_engine);
    DestroyWindow(_hwnd);
    _hwnd = nullptr;
}

GDK_METHOD_(void) Application::_CreateClassAndWindow(_In_ HINSTANCE hInstance)
{
    WNDCLASSEX wex = {0};

    wex.cbSize = sizeof(wex);
    wex.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    wex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wex.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wex.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
    wex.hInstance = hInstance;
    wex.lpfnWndProc = Application::s_WndProc;
    wex.lpszClassName = s_ClassName;

    GLEB(RegisterClassEx(&wex));

    _hwnd = CreateWindowEx(0, s_ClassName, s_ClassName, WS_CAPTION|WS_BORDER|WS_SYSMENU|WS_MINIMIZEBOX, 0, 0, 1280, 720, nullptr, nullptr, hInstance, nullptr);
    GLEB(_hwnd);
}

GDK_METHOD_(void) Application::_CreateEngine(_In_ const wchar_t* configFile)
{
    // TODO: We should create all our subsystems ahead of time and pass them
    // to the engine so that there's never a state where the engine is half-initialized.
    stde::com_ptr<IStream> configData;
    configFile;
    LoadStorageItem(configFile, &configData);
    CHK(CreateEngine(this, configData, &_engine));

    _CreateAndBindSubsystems();
}

GDK_METHOD_(void) Application::_CreateAndBindSubsystems()
{
    GDK::IRenderer* renderer = nullptr;
    CHK(CreateRenderer(_hwnd, &renderer));

    //_engine->AddRenderer(renderer);

    SafeRelease(renderer);
}

GDK_METHOD_(void) Application::_OnIdle()
{
    _engine->OnIdle();
}

LRESULT CALLBACK Application::_WndProc(_In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CLOSE:
        PostQuitMessage(0);
        break;

    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE)
        {
            SendMessage(_hwnd, WM_CLOSE, 0, 0);
        }
        break;

    default:
        return DefWindowProc(_hwnd, uMsg, wParam, lParam);
    }

    return TRUE;
}

LRESULT CALLBACK Application::s_WndProc(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
    Application* app = s_instance;

    if (app && hwnd == app->_hwnd)
    {
        return app->_WndProc(uMsg, wParam, lParam);
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
