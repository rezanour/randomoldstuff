#if defined(_MSC_VER)
#pragma once
#endif

#ifndef _CLASSIC_HOST_APPLICATION_H_
#define _CLASSIC_HOST_APPLICATION_H_

namespace GDK
{
    class Application : private stde::non_copyable, public RefCounted<IGameHost>
    {
    public:
        // Creation & singleton access
        static GDK_METHOD_(void) Create(_In_ HINSTANCE hInstance, _In_opt_ const wchar_t* configurationFile, _Deref_out_ Application** application);
        static Application* GetApplication();

        // Basic operation
        GDK_METHOD_(void) Run();

        // IGameHost
        GDK_IMETHOD_(HostType) GetType() const;
        GDK_IMETHOD_(void) Exit(_In_ uint32_t code);
        GDK_IMETHOD_(void) EnableDebugPrint(_In_ bool enabled);
        GDK_IMETHOD_(void) SetLogFile(_In_opt_ const wchar_t* name);
        virtual void __cdecl LogMessageF(_In_ const wchar_t* format, ...);
        GDK_IMETHOD_(void) LogMessage(_In_ const wchar_t* message);

        GDK_IMETHOD_(void) CreateStorageItem(_In_ const wchar_t* storageItem, _Deref_out_ IStream** stream);
        GDK_IMETHOD_(void) LoadStorageItem(_In_ const wchar_t* storageItem, _Deref_out_ IStream** stream);

    private:
        Application();
        ~Application();

        GDK_METHOD_(void) _Initialize(_In_ HINSTANCE hInstance, _In_opt_ const wchar_t* configurationFile);
        GDK_METHOD_(void) _Uninitialize();
        GDK_METHOD_(void) _CreateClassAndWindow(_In_ HINSTANCE hInstance);
        GDK_METHOD_(void) _CreateEngine(_In_ const wchar_t* configFile);
        GDK_METHOD_(void) _CreateAndBindSubsystems();

        GDK_METHOD_(void) _OnIdle();

        static LRESULT CALLBACK s_WndProc(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);
        LRESULT CALLBACK _WndProc(_In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);

        static Application* s_instance;
        static const wchar_t* const s_ClassName;

        ATOM _registration;
        HWND _hwnd;
        IEngine* _engine;
        bool _debugPrintEnabled;
    };
}

#endif // _CLASSIC_HOST_APPLICATION_H_
