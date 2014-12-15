#if defined(_MSC_VER)
#pragma once
#endif

#ifndef _GDK_DEBUGGING_H_
#define _GDK_DEBUGGING_H_

namespace GDK
{
    typedef void (*DebugListener)(_In_ byte_t channel, _In_ const wchar_t* message);

    // Echo all log output to the debug console
    HRESULT GDK_API InitializeDebugLogListener();

    // Managing debug listeners
    HRESULT GDK_API AddDebugListener(_In_ DebugListener listener);
    HRESULT GDK_API RemoveDebugListener(_In_ DebugListener listener);

    // Output debug string
    void GDK_API DebugString(_In_ byte_t channel, _In_ const wchar_t* message);
    void GDK_API DebugString(_In_ const wchar_t* message);

    // Output debug string with custom formatting
    void __cdecl DebugStringF(_In_ byte_t channel, _In_ const wchar_t* format, ...);
    void __cdecl DebugStringF(_In_ const wchar_t* format, ...);
} // GDK

#endif // _GDK_DEBUGGING_H_
