#include "StdAfx.h"
#include "Error.h"

// Registration of handlers
namespace _internal_
{
    struct ErrorHandlerNode
    {
        ErrorHandler handler;
        ErrorHandlerNode* next;
        uint32_t cookie;
    };

    ErrorHandlerNode* g_head;
    bool g_breakOnError = false;
    uint32_t g_nextCookie = 1;

    void DefaultErrorHandler(_Inout_ ErrorInfo& error, _Out_ ContinueAction* action)
    {
        switch (error.severity)
        {
        case ErrorInfo::Severity_Info:
            // recoverable
            error.errorCode = S_OK;
            *action = ContinueAction_Recovered;
            break;

        case ErrorInfo::Severity_Error:
            // continuable, but as an error (so leave error code in tact)
            if (g_breakOnError)
            {
                __debugbreak();
            }
            *action = ContinueAction_Recovered;
            break;

        case ErrorInfo::Severity_Fatal:
        default:
            if (IsDebuggerPresent())
            {
                __debugbreak();
            }
            else
            {
#ifndef NDEBUG
                wchar_t szMessage[512] = {0};
                StringCchPrintf(szMessage, _countof(szMessage), L"Fatal error encountered:\n%s failed with 0x%08x (file: %s, line: %d)\n\nDebug the process?", 
                    (error.codeSnippet ? error.codeSnippet : L"<unknown>"),
                    error.errorCode,
                    (error.filename ? error.filename : L"<unknown>"),
                    error.lineNumber);
                if (MessageBox(nullptr, szMessage, L"Fatal Error!", MB_YESNO | MB_ICONERROR) == IDYES)
                {
                    // REVIEW: Why doesn't this work?
                    DebugActiveProcess(GetCurrentProcessId());
                }
#endif
            }
            *action = ContinueAction_Terminate;
        }
    }
}

uint32_t RegisterErrorHandler(_In_ ErrorHandler handler)
{
    using namespace _internal_;

    if (!g_head)
    {
        g_head = new ErrorHandlerNode;
        g_head->handler = handler;
        g_head->next = nullptr;
        g_head->cookie = g_nextCookie++;
    }
    else
    {
        ErrorHandlerNode* node = new ErrorHandlerNode;
        node->handler = handler;
        node->next = g_head;
        node->cookie = g_nextCookie++;
        g_head = node;
    }

    return g_head->cookie;
}

bool UnregisterErrorHandler(_In_ uint32_t registrationCookie)
{
    using namespace _internal_;

    // Since we have a forward only list, we need to find the
    // node who's next points to the node we want to delete

    // special case: it's the head node
    if (g_head->cookie == registrationCookie)
    {
        ErrorHandlerNode* newHead = g_head->next;
        delete g_head;
        g_head = newHead;

        return true;
    }
    else
    {
        ErrorHandlerNode* node = g_head;
        while (node->next)
        {
            if (node->next->cookie == registrationCookie)
            {
                ErrorHandlerNode* newNext = node->next->next;
                delete node->next;
                node->next = newNext;

                return true;
            }
        }
    }

    return false;
}

void BreakOnError(_In_ bool breakOnError)
{
    _internal_::g_breakOnError = breakOnError;
}

void RaiseError(_Inout_ ErrorInfo& error)
{
    using namespace _internal_;

    ErrorHandlerNode* node = g_head;
    ContinueAction action = ContinueAction_KeepProcessing;

    while (node)
    {
        node->handler(error, &action);
        switch (action)
        {
        case ContinueAction_Recovered:
            return;

        case ContinueAction_Terminate:
            // TODO: allow registration of shutdown code
            exit(error.errorCode);
            break;
        }

        node = node->next;
    }

    DefaultErrorHandler(error, &action);
    if (action == ContinueAction_Recovered)
        return;

    exit(error.errorCode);
}

