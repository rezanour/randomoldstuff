#include "StdAfx.h"
#include "Debug.h"

#ifdef _DEBUG

using namespace CoreServices;

// Preferred way of enforcing file scope (internal linkage), instead of putting static in front of everything.
// This also applies to the structs, since you can't define a struct static.
namespace
{
    DebugLevel g_currentVerbosity = DebugLevel::Warning;

    const char* const g_debugPrefixes[] =
    {
        "Info: ",
        "Warning: ",
        "Error: ",
    };

    struct CallbackHandler
    {
        Debug::DebugCallback Callback;
        void* Context;

        CallbackHandler(_In_ Debug::DebugCallback callback, _In_ void* context)
            : Callback(callback), Context(context)
        {
        }
    };

    struct CallbackHandlerW
    {
        Debug::DebugCallbackW Callback;
        void* Context;

        CallbackHandlerW(_In_ Debug::DebugCallbackW callback, _In_ void* context)
            : Callback(callback), Context(context)
        {
        }
    };

    std::vector<CallbackHandler> g_handlers;
    std::vector<CallbackHandlerW> g_handlersW;
}

void Debug::SetVerbosity(_In_ DebugLevel level)
{
    g_currentVerbosity = level;
}

std::string __cdecl FormatDebugString(_In_ const char* const format, _In_ va_list& list, _In_opt_ const char* const prefix = nullptr)
{
    // we initialize a static 2k string for expanded messages.
    // REVIEW: evaluate whether 2k is enough? too much?
    static char message[2048] = {0};

    // because we reuse the same static
    message[0] = '\0';

    if (prefix)
        strcpy_s(message, prefix);

    size_t start = strlen(message);

    vsprintf_s(&message[start], _countof(message) - start, format, list);

    strcat_s(message, "\n");

    return message;
}

void PrintAndBroadcast(_In_ DebugLevel level, _In_ const std::string& message)
{
    OutputDebugStringA(message.c_str());

    for (size_t i = 0; i < g_handlers.size(); i++)
    {
        g_handlers[i].Callback(level, message, g_handlers[i].Context);
    }

    for (size_t i = 0; i < g_handlersW.size(); i++)
    {
        g_handlersW[i].Callback(level, stde::to_wstring(message), g_handlersW[i].Context);
    }
}

void __cdecl Debug::Out(_In_ const char* const format, ...)
{
    va_list list;
    va_start(list, format);
    std::string message(FormatDebugString(format, list));
    va_end(list);

    PrintAndBroadcast(DebugLevel::None, message);
}

void __cdecl Debug::OutLevel(_In_ DebugLevel level, _In_ const char* const format, ...)
{
    if (level < g_currentVerbosity)
        return;

    // make sure the value isn't invalid
    if (level < DebugLevel::Info)
        level = DebugLevel::Info;

    if (level > DebugLevel::Error)
        level = DebugLevel::Error;

    va_list list;
    va_start(list, format);
    std::string message(FormatDebugString(format, list, g_debugPrefixes[level]));
    va_end(list);

    PrintAndBroadcast(level, message);
}

void Debug::AddCallback(_In_ DebugCallback pfnCallback, _In_ void* pContext)
{
    g_handlers.push_back(CallbackHandler(pfnCallback, pContext));
}

void Debug::AddCallbackW(_In_ DebugCallbackW pfnCallback, _In_ void* pContext)
{
    g_handlersW.push_back(CallbackHandlerW(pfnCallback, pContext));
}

void __cdecl Debug::AssertImpl(_In_ bool condition, _In_ const char* const format, ...)
{
    if (!condition)
    {
        va_list list;
        va_start(list, format);
        std::string message(FormatDebugString(format, list));
        va_end(list);

        std::string debugMessage("\nAssertion Failed!\n*** ");
        debugMessage += message;
        debugMessage += "\n";

        PrintAndBroadcast(DebugLevel::Assert, debugMessage);

        message += "\nPress Retry to debug the game.";

        int result = MessageBoxA(nullptr, message.c_str(), "Assertion Failed!", MB_ICONSTOP | MB_ABORTRETRYIGNORE | MB_DEFBUTTON1 | MB_SETFOREGROUND | MB_TASKMODAL);
        if (result == IDABORT)
        {
            raise(SIGABRT);
            exit(3);
        }
        else if (result == IDRETRY)
        {
            __debugbreak();
        }

        DebugInfo("Continuing past assertion...");
    }
}

#endif

