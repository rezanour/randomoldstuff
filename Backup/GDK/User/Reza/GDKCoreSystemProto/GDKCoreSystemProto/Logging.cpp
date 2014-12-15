#include "StdAfx.h"
#include "Logging.h"

#include <vector>

using GDK::LogListener;

namespace GDK
{
    namespace Details
    {
        static std::vector<LogListener> g_listeners;
        static HANDLE g_logFile = INVALID_HANDLE_VALUE;

        static void GDK_API LogF(_In_ byte_t channel, _In_ const wchar_t* format, _In_ va_list args);
    }
}

HRESULT GDK_API GDK::AddLogListener(_In_ LogListener listener)
{
    for (size_t i = 0, count = Details::g_listeners.size(); i < count; ++i)
    {
        if (Details::g_listeners[i] == listener)
        {
            return HRESULT_FROM_WIN32(ERROR_ALREADY_EXISTS);
        }
    }

    Details::g_listeners.push_back(listener);
    return S_OK;
}

HRESULT GDK_API GDK::RemoveLogListener(_In_ LogListener listener)
{
    for (size_t i = 0, count = Details::g_listeners.size(); i < count; ++i)
    {
        if (Details::g_listeners[i] == listener)
        {
            for (size_t j = i; j < count - 1; ++j)
            {
                Details::g_listeners[j] = Details::g_listeners[j + 1];
            }
            Details::g_listeners.pop_back();
            return S_OK;
        }
    }

    return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
}

// nullptr to disable file logging
HRESULT GDK_API GDK::SetLogFile(_In_opt_ const wchar_t* filename)
{
    if (!filename)
    {
        CHECKB(Details::g_logFile != INVALID_HANDLE_VALUE, HRESULT_FROM_WIN32(ERROR_NOT_FOUND));
        CloseHandle(Details::g_logFile);
        Details::g_logFile = INVALID_HANDLE_VALUE;
    }
    else
    {
        CHECKB(Details::g_logFile == INVALID_HANDLE_VALUE, HRESULT_FROM_WIN32(ERROR_ALREADY_EXISTS));

        Details::g_logFile = CreateFile(filename, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, 0, nullptr);
        CHECKB(Details::g_logFile != INVALID_HANDLE_VALUE, HRESULT_FROM_WIN32(GetLastError()));
    }

    return S_OK;
}

void GDK_API GDK::Log(_In_ byte_t channel, _In_ const wchar_t* message)
{
    // forward to all listeners
    for (size_t i = 0, count = Details::g_listeners.size(); i < count; ++i)
    {
        Details::g_listeners[i](channel, message);
    }

    // then write to file if we have one open
    if (Details::g_logFile != INVALID_HANDLE_VALUE)
    {
        // TODO: We probably don't want to write each time a log message comes in.
        // instead, we should buffer them and commit occassionally. Also, we can probably 
        // just do the newline processing at flush time.
        DWORD bytesWritten = 0;
        const wchar_t* p = message;
        while (*p != 0)
        {
            const wchar_t* newLine = p;
            while (*newLine != 0 && *newLine != L'\n')
            {
                ++newLine;
            }

            WriteFile(Details::g_logFile, static_cast<const void*>(p), static_cast<DWORD>((newLine - p) * sizeof(wchar_t)), &bytesWritten, nullptr);
            WriteFile(Details::g_logFile, static_cast<const void*>(L"\r\n"), (2 * sizeof(wchar_t)), &bytesWritten, nullptr);

            if (*newLine == 0)
                break;

            p = newLine + 1;
        }
    }
}

void GDK_API GDK::Log(const wchar_t* message)
{
    Log(0, message);
}

void __cdecl GDK::LogF(_In_ byte_t channel, const wchar_t* format, ...)
{
    va_list args;
    va_start(args, format);
    Details::LogF(channel, format, args);
    va_end(args);
}

void __cdecl GDK::LogF(const wchar_t* format, ...)
{
    va_list args;
    va_start(args, format);
    Details::LogF(0, format, args);
    va_end(args);
}

void GDK_API GDK::Details::LogF(_In_ byte_t channel, _In_ const wchar_t* format, _In_ va_list args)
{
    static wchar_t message[500] = {0};
    vswprintf_s(message, format, args);
    Log(channel, message);
}
