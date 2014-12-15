#if defined(_MSC_VER)
#pragma once
#endif

#ifndef _GDK_ERROR_H_
#define _GDK_ERROR_H_

namespace GDK
{
    // *** GDK Error Policy ***
    //
    // If an error is nonrecoverable, then an exception should
    // be generated (after any logging). Because we don't ever catch 
    // them, this causes the program to crash (exit for Release).
    //
    // If a method can fail in some recoverable way, then
    // we use error codes (HRESULT) to indicate this.
    //

    // Attempt to log failure info, when logging facilities are available
    inline const char* __cdecl LogFailure(_In_ const char* format, ...)
    {
        static char message[500] = {0};
        va_list args;
        va_start(args, format);
        vsprintf_s(message, format, args);
        va_end(args);

        // TODO: Log message
        DebugStringA(message);

        return message;
    }

    namespace Details
    {
        // Attempt to log failure info, when logging facilities are available
        inline void LogFailure(_In_ const char* function, _In_ const size_t& line, _In_ const char* code, _In_ const HRESULT& hr)
        {
            GDK::LogFailure("%s (%d): %s failed with 0x%08x\n", function, line, code, hr);
        }

        // Handle a failed HRESULT based call, which we are treating as fatal
        inline void FatalError(_In_ const char* function, _In_ const size_t& line, _In_ const char* code, _In_ const HRESULT& hr)
        {
            throw std::exception(GDK::LogFailure("%s (%d): %s failed with 0x%08x\n", function, line, code, hr));
        }
    }

    // The CHK macro should be used to wrap any HRESULT returning method where
    // failure is considered fatal.

#ifdef CHK
#undef CHK
#endif

#ifndef NDEBUG
#define CHK(x)  { HRESULT _hr = (x); if (FAILED(_hr)) { GDK::Details::FatalError(__FUNCTION__, __LINE__, #x, _hr); } }
#else
#define CHK(x)  if (FAILED((x))) throw std::exception();
#endif

    // The GLE macro should be used to wrap any DWORD returning method which
    // reporsts errors via GetLastError, and where failure is considered fatal.

#ifdef GLE
#undef GLE
#endif

#ifndef NDEBUG
#define GLE(x)  if (x) { GDK::Details::FatalError(__FUNCTION__, __LINE__, #x, HRESULT_FROM_WIN32(GetLastError())); }
#else
#define GLE(x)  if (x) throw std::exception();
#endif

    // The GLEB macro should be used to wrap any BOOL returning method which
    // reporsts errors via GetLastError, and where failure is considered fatal.

#ifdef GLEB
#undef GLEB
#endif

#ifndef NDEBUG
#define GLEB(x)  if (!(x)) { GDK::Details::FatalError(__FUNCTION__, __LINE__, #x, HRESULT_FROM_WIN32(GetLastError())); }
#else
#define GLEB(x)  if (!(x)) throw std::exception();
#endif
}

#endif // _GDK_ERROR_H_
