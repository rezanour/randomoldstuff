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

#define CHECK(x)            hr = (x); if (FAILED(hr)) { GDK::Details::HandleError(GDK::Details::ErrorLevel_Error, hr, __FILEW__, __FUNCTIONW__, __LINE__, L#x); return hr; }
#define CHECKP(pointer, hr) if (!(pointer)) { GDK::Details::HandleError(GDK::Details::ErrorLevel_Error, hr, __FILEW__, __FUNCTIONW__, __LINE__, L#pointer); return hr; }
#define CHECKB(boolexp, hr) if (!(boolexp)) { GDK::Details::HandleError(GDK::Details::ErrorLevel_Error, hr, __FILEW__, __FUNCTIONW__, __LINE__, L#boolexp); return hr; }

#define FATAL(x)            hr = (x); if (FAILED(hr)) { GDK::Details::HandleError(GDK::Details::ErrorLevel_Fatal, hr, __FILEW__, __FUNCTIONW__, __LINE__, L#x); GDK::Details::FatalExit(); }
#define FATALP(pointer, hr) if (!(pointer)) { GDK::Details::HandleError(GDK::Details::ErrorLevel_Fatal, hr, __FILEW__, __FUNCTIONW__, __LINE__, L#pointer); GDK::Details::FatalExit(); }
#define FATALB(boolexp, hr) if (!(boolexp)) { GDK::Details::HandleError(GDK::Details::ErrorLevel_Fatal, hr, __FILEW__, __FUNCTIONW__, __LINE__, L#boolexp); GDK::Details::FatalExit(); }

#else

#define CHECK(x)            hr = (x); if (FAILED(hr)) { return hr; }
#define CHECKP(pointer, hr) if (!(pointer)) { return hr; }
#define CHECKB(boolexp, hr) if (!(boolexp)) { return hr; }

#define FATAL(x)            hr = (x); if (FAILED(hr)) { GDK::Details::FatalExit(); }
#define FATALP(pointer, hr) if (!(pointer)) { GDK::Details::FatalExit(); }
#define FATALB(boolexp, hr) if (!(boolexp)) { GDK::Details::FatalExit(); }

#endif // NDEBUG

#endif // _GDK_ERROR_H_
