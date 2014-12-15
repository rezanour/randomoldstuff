#if defined(_MSC_VER)
#pragma once
#endif

#ifndef _GDK_ERROR_H_
#define _GDK_ERROR_H_

namespace GDK
{
    namespace Details
    {
        enum ErrorLevel
        {
            ErrorLevel_Error,
            ErrorLevel_Fatal
        };

        // Error handling infrastruction
        void GDK_API HandleError(_In_ ErrorLevel level, _In_ HRESULT hr, _In_ const wchar_t* sourceFile, _In_ const wchar_t* sourceFunction, _In_ size_t lineNumber, _In_ const wchar_t* sourceCode);
        void GDK_API FatalExit();

    } // Details
} // GDK

// Error handling macros

#ifdef CHECK
#undef CHECK
#endif
#ifdef CHECKP
#undef CHECKP
#endif
#ifdef CHECKB
#undef CHECKB
#endif
#ifdef FATAL
#undef FATAL
#endif
#ifdef FATALP
#undef FATALP
#endif
#ifdef FATALB
#undef FATALB
#endif

#ifndef NDEBUG

#define CHECK(x)                    hr = (x); if (FAILED(hr)) { GDK::Details::HandleError(GDK::Details::ErrorLevel_Error, hr, __FILEW__, __FUNCTIONW__, __LINE__, L#x); goto Exit; }
#define CHECKP(pointer, failureHr)  if (!(pointer)) { GDK::Details::HandleError(GDK::Details::ErrorLevel_Error, hr, __FILEW__, __FUNCTIONW__, __LINE__, L#pointer); hr = failureHr; goto Exit; }
#define CHECKB(boolexp, failureHr)  if (!(boolexp)) { GDK::Details::HandleError(GDK::Details::ErrorLevel_Error, hr, __FILEW__, __FUNCTIONW__, __LINE__, L#boolexp); hr = failureHr; goto Exit; }

#define FATAL(x)                    hr = (x); if (FAILED(hr)) { GDK::Details::HandleError(GDK::Details::ErrorLevel_Fatal, hr, __FILEW__, __FUNCTIONW__, __LINE__, L#x); GDK::Details::FatalExit(); }
#define FATALP(pointer, failureHr)  if (!(pointer)) { GDK::Details::HandleError(GDK::Details::ErrorLevel_Fatal, hr, __FILEW__, __FUNCTIONW__, __LINE__, L#pointer); GDK::Details::FatalExit(); }
#define FATALB(boolexp, failureHr)  if (!(boolexp)) { GDK::Details::HandleError(GDK::Details::ErrorLevel_Fatal, hr, __FILEW__, __FUNCTIONW__, __LINE__, L#boolexp); GDK::Details::FatalExit(); }

#else

#define CHECK(x)                    hr = (x); if (FAILED(hr)) { goto Exit; }
#define CHECKP(pointer, failureHr)  if (!(pointer)) { hr = failureHr; goto Exit; }
#define CHECKB(boolexp, failureHr)  if (!(boolexp)) { hr = failureHr; goto Exit; }

#define FATAL(x)                    hr = (x); if (FAILED(hr)) { GDK::Details::FatalExit(); }
#define FATALP(pointer, failureHr)  if (!(pointer)) { hr = failureHr; GDK::Details::FatalExit(); }
#define FATALB(boolexp, failureHr)  if (!(boolexp)) { hr = failureHr; GDK::Details::FatalExit(); }

#endif // NDEBUG

#include "Error.inl"

#endif // _GDK_ERROR_H_
