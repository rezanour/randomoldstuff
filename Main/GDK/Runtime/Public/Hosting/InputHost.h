#pragma once

#include <Input.h>

namespace GDK
{
    namespace Input
    {
        // Hosts use these methods to inject input into the system in response to OS specific events
        void OnButtonDown(_In_ Button button);
        void OnButtonUp(_In_ Button button);

#ifdef WIN32
        void SetWindow(_In_ void* hwnd);
        bool GetButtonFromWin32Keycode(_In_ byte_t keycode, _Out_ Button* outButton);
#endif
    }
}
