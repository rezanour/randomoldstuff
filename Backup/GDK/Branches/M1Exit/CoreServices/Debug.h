// Debug output, which can be optionally hooked (for additional output), and forwarded to logging
#pragma once

#include <Windows.h>
#include <string>

#ifdef _DEBUG

#define SetDebugVerbosity(level)                    CoreServices::Debug::SetVerbosity(level)
#define DebugOut(format, ...)                       CoreServices::Debug::Out(__FUNCTION__ ": " format, __VA_ARGS__)
#define DebugOutLevel(level, format, ...)           CoreServices::Debug::OutLevel(level, __FUNCTION__ ": " format, __VA_ARGS__)
#define AddDebugCallback(pfnCallback, pContext)     CoreServices::Debug::AddCallback(pfnCallback, pContext)
#define AddDebugCallbackW(pfnCallbackW, pContext)   CoreServices::Debug::AddCallbackW(pfnCallbackW, pContext)

#define Assert(condition)                           CoreServices::Debug::AssertImpl(!!(condition), "Condition: %s, File: %s, Line: %d", #condition, __FILE__, __LINE__)

#else

#define SetDebugVerbosity(level)                    {}
#define DebugOut(format, ...)                       {}
#define DebugOutLevel(level, format, ...)           {}
#define AddDebugCallback(pfnCallback, pContext)     {}
#define AddDebugCallbackW(pfnCallbackW, pContext)   {}

#define Assert(condition)                           {}

#endif

// since these are composed of existing macros above, they don't need preprocessor guarding
#define DebugInfo(format, ...)                      DebugOutLevel(CoreServices::DebugLevel::Info, format, __VA_ARGS__)
#define DebugWarning(format, ...)                   DebugOutLevel(CoreServices::DebugLevel::Warning, format, __VA_ARGS__)
#define DebugError(format, ...)                     DebugOutLevel(CoreServices::DebugLevel::Error, format, __VA_ARGS__)

// error handling helpers
#define CHECKHR(x) { hr = (x); if (FAILED(hr)) { DebugError("%s failed with hr = 0x%08x", #x, hr); goto Exit; } }

#define ISNOTNULL(x, hrOnError) { if (!(x)) { DebugError("%s cannot be null", #x); hr = hrOnError; goto Exit; } }
#define ISTRUE(x, hrOnError) { if (!(x)) { DebugError("%s must be true", #x); hr = hrOnError; goto Exit; } }
#define ISFALSE(x, hrOnError) { if (x) { DebugError("%s must be false", #x); hr = hrOnError; goto Exit; } }

#define EXIT Exit: if (FAILED(hr)) DebugInfo("Returning with hr = 0x%08x", hr); 

namespace CoreServices
{
    enum DebugLevel
    {
        Info = 0,
        Warning,
        Error,
        None,
        Assert,
    };

#ifdef _DEBUG
    class Debug
    {
    public:
        // Control Verbosity
        static void SetVerbosity(_In_ DebugLevel level);

        // Output
        static void __cdecl Out(_In_ const char* const format, ...);
        static void __cdecl OutLevel(_In_ DebugLevel level, _In_ const char* const format, ...);

        // Callback
        typedef void (* DebugCallback)(_In_ DebugLevel level, _In_ const std::string& message, _In_ void* pContext);
        typedef void (* DebugCallbackW)(_In_ DebugLevel level, _In_ const std::wstring& message, _In_ void* pContext);

        static void AddCallback(_In_ DebugCallback pfnCallback, _In_ void* pContext);
        static void AddCallbackW(_In_ DebugCallbackW pfnCallback, _In_ void* pContext);

        // Assert
        static void __cdecl AssertImpl(_In_ bool condition, _In_ const char* const format, ...);
    };
#endif
}

