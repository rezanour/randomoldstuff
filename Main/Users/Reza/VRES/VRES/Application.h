#pragma once

namespace VRES
{
    //
    // Applications should derive from this class to
    // implement their core behaviors.
    //
    class Application
    {
    public:
        virtual ~Application();

        const std::wstring& AppName() const;
        HWND                Window() const;

        bool CreateAppWindow(const Point& position, const Size& size);
        void Run(); // Blocks and executes program to completion

    protected:
        Application(_In_z_ const wchar_t* appName);

        // Return false to exit
        virtual bool Tick(float elapsedSeconds) = 0;

    private:
        static LRESULT CALLBACK s_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        HWND _hwnd;
        std::wstring _appName;

        LARGE_INTEGER _frequency;
        LARGE_INTEGER _prevTime;
    };
}
