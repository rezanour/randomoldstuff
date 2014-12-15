#if defined(_MSC_VER)
#pragma once
#endif

#ifndef _GDK_LOGGING_H_
#define _GDK_LOGGING_H_

namespace GDK
{
    typedef void (*LogListener)(_In_ byte_t channel, _In_ const wchar_t* message);

    // Managing log listeners
    HRESULT GDK_API AddLogListener(_In_ LogListener listener);
    HRESULT GDK_API RemoveLogListener(_In_ LogListener listener);

    // Nullptr to disable file logging
    HRESULT GDK_API SetLogFile(_In_opt_ const wchar_t* filename);

    // Output information to the log
    void GDK_API Log(_In_ byte_t channel, _In_ const wchar_t* message);
    void GDK_API Log(_In_ const wchar_t* message);

    // Output information with a format string and parameters
    void __cdecl LogF(_In_ byte_t channel, _In_ const wchar_t* format, ...);
    void __cdecl LogF(_In_ const wchar_t* format, ...);

} // GDK

#endif // _GDK_LOGGING_H_
