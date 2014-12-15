// Error information and error handling facilities
#pragma once

#include "Debug.h"

//
// Design Details
//
// The main purpose of this error system is to provide
// consistent, easy handling of error situations, while
// still allowing customization of how the error is handled.
//
// The main principle is that at the time you're making a call
// which might fail, you can categorize a possible failure as any of:
//   * recoverable - a failure doesn't stop the current method from continuing
//   * unrecoverable, but continuable - a failure prevents the method from completing
//       it's task, but the program might be able to continue, so we return from this method.
//   * fatal - the program cannot continue to execute and should be exited (after allowing optional cleanup)
//
// The chain of handlers that are allowed to process the errors is customizable at runtime.
// The handlers are executed in order, and each is allowed to decide whether processing should
// continue to the next handler, terminate the app, or resume execution of the program.
//
// There is a mechanism to register shutdown handlers which will be executed in the event that
// a fatal error occurs and the process needs to be exited.
//

// Error structure
struct ErrorInfo
{
    enum Severity
    {
        Severity_Info,
        Severity_Error,
        Severity_Fatal,
    };

    ErrorInfo(_In_ const wchar_t* filename, _In_ const wchar_t* codeSnippet, _In_ uint64_t lineNumber, _In_ HRESULT errorCode, _In_ Severity severity)
        : filename(filename), codeSnippet(codeSnippet), lineNumber(lineNumber), errorCode(errorCode), severity(severity)
    {
    }

    // Identification
    const wchar_t* filename;
    const wchar_t* codeSnippet;
    uint64_t lineNumber;

    // Error details
    HRESULT errorCode;
    Severity severity;
};

// Instructs the error handler processor about what to do next
enum ContinueAction
{
    ContinueAction_Recovered,
    ContinueAction_KeepProcessing,
    ContinueAction_Terminate
};

// Prototype for the error handler methods
typedef void (*ErrorHandler)(_Inout_ ErrorInfo& error, _Out_ ContinueAction* action);

uint32_t RegisterErrorHandler(_In_ ErrorHandler handler);
bool UnregisterErrorHandler(_In_ uint32_t registrationCookie);
void BreakOnError(_In_ bool breakOnError);
void RaiseError(_Inout_ ErrorInfo& error);

// Macros to participate in the error system

#ifndef NDEBUG

#define RAISE_ERROR(code, hr, severity) \
{ \
    ErrorInfo error(__FILEW__, L#code, __LINE__, hr, severity); \
    RaiseError(error); \
    hr = error.errorCode; \
}

#else

#define RAISE_ERROR(code, hr, severity) \
{ \
    ErrorInfo error(nullptr, nullptr, __LINE__, hr, severity); \
    RaiseError(error); \
    hr = error.errorCode; \
}

#endif

#define INFO(x) hr = (x); if (FAILED(hr)) { RAISE_ERROR(x, hr, ErrorInfo::Severity_Info); if (FAILED(hr)) { goto Exit; } }
#define CHECK(x) hr = (x); if (FAILED(hr)) { RAISE_ERROR(x, hr, ErrorInfo::Severity_Error); if (FAILED(hr)) { goto Exit; } }
#define FATAL(x) hr = (x); if (FAILED(hr)) { RAISE_ERROR(x, hr, ErrorInfo::Severity_Fatal); if (FAILED(hr)) { goto Exit; } }


