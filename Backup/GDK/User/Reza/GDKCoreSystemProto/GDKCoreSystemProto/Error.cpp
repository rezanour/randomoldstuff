#include "StdAfx.h"
#include "Error.h"

void GDK::Details::HandleError(_In_ GDK::Details::ErrorLevel level, _In_ HRESULT hr, _In_ const wchar_t* sourceFile, _In_ const wchar_t* sourceFunction, _In_ size_t lineNumber, _In_ const wchar_t* sourceCode)
{
    LogF(L"** %s ** %s failed with 0x%08x in %s, file: %s, line: %lu\n", 
        (level == ErrorLevel_Error ? L"ERROR" : L"FATAL"),
        sourceCode, hr, sourceFunction, sourceFile, lineNumber);
}

void GDK::Details::FatalExit()
{
    if (IsDebuggerPresent())
    {
        DebugBreak();
    }
    ::FatalExit(-1);
}
