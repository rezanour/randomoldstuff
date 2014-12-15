#include "StdAfx.h"
#include "Debugging.h"

using GDK::DebugListener;

#ifndef NDEBUG

#include <vector>

namespace GDK
{
    namespace Details
    {
        static void __cdecl DebugStringF(_In_ byte_t channel, _In_ const wchar_t* format, _In_ va_list args);

        static std::vector<DebugListener> g_listeners;
        static void DebugLogListener(_In_ byte_t channel, _In_ const wchar_t* message);
    }
} // GDK

HRESULT GDK_API GDK::InitializeDebugLogListener()
{
    return AddLogListener(Details::DebugLogListener);
}

HRESULT GDK_API GDK::AddDebugListener(_In_ DebugListener listener)
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

HRESULT GDK_API GDK::RemoveDebugListener(_In_ DebugListener listener)
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

void GDK_API GDK::DebugString(_In_ byte_t channel, _In_ const wchar_t* message)
{
    // send to debug output
    OutputDebugString(message);

    // and to all listeners
    for (size_t i = 0, count = Details::g_listeners.size(); i < count; ++i)
    {
        Details::g_listeners[i](channel, message);
    }
}

void GDK_API GDK::DebugString(_In_ const wchar_t* message)
{
    DebugString(0, message);
}

void __cdecl GDK::DebugStringF(_In_ byte_t channel, _In_ const wchar_t* format, ...)
{
    va_list args;
    va_start(args, format);
    Details::DebugStringF(channel, format, args);
    va_end(args);
}

void __cdecl GDK::DebugStringF(_In_ const wchar_t* format, ...)
{
    va_list args;
    va_start(args, format);
    Details::DebugStringF(0, format, args);
    va_end(args);
}

void __cdecl GDK::Details::DebugStringF(_In_ byte_t channel, _In_ const wchar_t* format, _In_ va_list args)
{
    static wchar_t message[1000] = {0};
    vswprintf_s(message, format, args);

    DebugString(channel, message);
}

void GDK::Details::DebugLogListener(_In_ byte_t channel, _In_ const wchar_t* message)
{
    DebugString(channel, message);
}

#else

void  GDK_API GDK::DebugString(_In_ byte_t channel, _In_ const wchar_t* message)
{
    UNREFERENCED_PARAMETER(channel);
    UNREFERENCED_PARAMETER(format);
}

void GDK_API GDK::DebugString(_In_ const wchar_t* message)
{
    UNREFERENCED_PARAMETER(format);
}

void __cdecl GDK::DebugStringF(_In_ byte_t channel, _In_ const wchar_t* format, ...)
{
    UNREFERENCED_PARAMETER(channel);
    UNREFERENCED_PARAMETER(format);
}

void __cdecl GDK::DebugStringF(_In_ const wchar_t* format, ...)
{
    UNREFERENCED_PARAMETER(format);
}

HRESULT GDK_API GDK::AddDebugListener(_In_ DebugListener listener)
{
    UNREFERENCED_PARAMETER(listener);
    UNREFERENCED_PARAMETER(cookie);
    return S_OK;
}

HRESULT GDK_API GDK::RemoveDebugListener(_In_ DebugListener listener)
{
    UNREFERENCED_PARAMETER(cookie);
    return S_OK;
}

#endif
