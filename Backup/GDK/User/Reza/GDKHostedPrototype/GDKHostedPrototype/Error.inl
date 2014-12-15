#if defined(_MSC_VER)
#pragma once
#endif

#ifndef _GDK_ERROR_INL_
#define _GDK_ERROR_INL_

#include "Platform.h"

namespace GDK
{
    namespace Details
    {
        const wchar_t* GetErrorMessage(_In_ HRESULT hr);

        inline void GDK_API HandleError(_In_ GDK::Details::ErrorLevel level, _In_ HRESULT hr, _In_ const wchar_t* sourceFile, _In_ const wchar_t* sourceFunction, _In_ size_t lineNumber, _In_ const wchar_t* sourceCode)
        {
            //LogF(L"** %s ** %s failed with 0x%08x in %s, file: %s, line: %lu\n", 
            //    (level == ErrorLevel_Error ? L"ERROR" : L"FATAL"),
            //    sourceCode, hr, sourceFunction, sourceFile, lineNumber);

            static wchar_t message[500] = {0};

            wsprintf(message, L"** %s ** %s failed with %s in %s, file: %s, line: %lu\n", 
                (level == ErrorLevel_Error ? L"ERROR" : L"FATAL"),
                sourceCode, GetErrorMessage(hr), sourceFunction, sourceFile, lineNumber);

            DebugString(message);
        }

        inline void GDK_API GDK::Details::FatalExit()
        {
            if (IsDebuggerPresent())
            {
                DebugBreak();
            }
            ::FatalExit(-1);
        }

        inline const wchar_t* GetErrorMessage(_In_ HRESULT hr)
        {
            static wchar_t defaultMessage[200] = {0};

#ifdef STAMP_HRESULT2
#undef STAMP_HRESULT2
#endif
#define STAMP_HRESULT2(hr, x) case hr:    return x;

#ifdef STAMP_HRESULT
#undef STAMP_HRESULT
#endif
#define STAMP_HRESULT(hr) STAMP_HRESULT2(hr, L#hr);

            switch (hr)
            {
            STAMP_HRESULT(E_FAIL)
            STAMP_HRESULT(E_INVALIDARG)
            STAMP_HRESULT(E_POINTER)
            STAMP_HRESULT(E_UNEXPECTED)
            STAMP_HRESULT(E_NOTIMPL)
            }

#undef STAMP_HRESULT
#undef STAMP_HRESULT2

            wsprintf(defaultMessage, L"0x%08x", hr);
            return defaultMessage;
        }
    } // Details
} // GDK

#endif // _GDK_ERROR_INL_
