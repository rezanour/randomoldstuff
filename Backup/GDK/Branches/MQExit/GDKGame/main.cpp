#include "StdAfx.h"

#include "GameApplication.h"
#include "TestList.h"

using namespace CoreServices;

HRESULT LoadConfiguration(_In_ const std::wstring& commandLine, _Inout_ ConfigurationPtr& spConfiguration);
void DebugOutErrorHandler(_Inout_ ErrorInfo& error, _Out_ ContinueAction* action);
void LogFileErrorHandler(_Inout_ ErrorInfo& error, _Out_ ContinueAction* action);

#ifdef BUILD_TESTS
int RunTests();
#endif

int CALLBACK WinMain(__in HINSTANCE hInstance, __in HINSTANCE hPrevInstance, __in LPSTR lpCmdLine, __in int nCmdShow)
{
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(nCmdShow);

    HRESULT hr = S_OK;

    // REVIEW: What should we do if these fail?
    RegisterErrorHandler(&DebugOutErrorHandler);
    RegisterErrorHandler(&LogFileErrorHandler);
    BreakOnError(false);

    ConfigurationPtr spConfiguration;

    FATAL(LoadConfiguration(stde::to_wstring(std::string(lpCmdLine)), spConfiguration));

#ifdef BUILD_TESTS

    if (spConfiguration->GetValue<bool>("RunTests", false))
    {
        return RunTests();
    }
#endif

    {
        GDK::GameApplication game(spConfiguration);
        FATAL(game.Run());
    }

EXIT
    return FAILED(hr); // return 0 if succeeded
}

HRESULT LoadConfiguration(_In_ const std::wstring& commandLine, _Inout_ ConfigurationPtr& spConfiguration)
{
    HRESULT hr = S_OK;

    wchar_t settingsFile[2000] = {0};

    // first check command line
    if (!commandLine.empty())
    {
        FATAL(StringCchCopy(settingsFile, _countof(settingsFile), commandLine.c_str()));
        if (GetFileAttributes(settingsFile) == INVALID_FILE_ATTRIBUTES)
        {
            DebugError("Specified an .ini file that doesn't exist! %s", settingsFile);
            FATAL(E_FAIL);
        }
    }

    if (settingsFile[0] == L'\0')
    {
        // If no override on command line, fall back to an ini file with the same name as the process
        wchar_t modulePath[2000] = {0};
        ISTRUE(GetModuleFileName(nullptr, modulePath, _countof(modulePath)), HRESULT_FROM_WIN32(GetLastError()));

        // replace the extension with .ini
        wchar_t* ext = wcsrchr(modulePath, L'.');
        if (ext)
        {
            modulePath[ext - modulePath] = L'\0';
        }

        FATAL(StringCchCat(modulePath, _countof(modulePath), L".ini"));

        FATAL(StringCchCopy(settingsFile, _countof(settingsFile), modulePath));
    }

    if (GetFileAttributes(settingsFile) == INVALID_FILE_ATTRIBUTES)
    {
        FATAL(Configuration::Create(&spConfiguration));
    }
    else
    {
        stde::com_ptr<IStream> spStream;
        FATAL(FileStream::Create(settingsFile, true, &spStream));
        FATAL(Configuration::Load(spStream, &spConfiguration));
    }

    {
        std::string relativeContentRoot = spConfiguration->GetStringValue("ContentRoot", "");
        wchar_t contentPath[MAX_PATH] = {0};

        // remove the file info
        wchar_t* lastSlash = wcsrchr(settingsFile, L'\\');
        if (lastSlash)
        {
            settingsFile[lastSlash - settingsFile] = L'\0';
        }

        FATAL(StringCchPrintf(contentPath, _countof(contentPath), L"%s\\%s", settingsFile, stde::to_wstring(relativeContentRoot).c_str()));
        spConfiguration->SetStringValue("ContentRoot", stde::to_string(std::wstring(contentPath)));
    }

    SetDebugVerbosity(static_cast<DebugLevel>(spConfiguration->GetValue("DebugVerbosity", static_cast<int>(DebugLevel::Warning))));

EXIT
    return hr;
}

void DebugOutErrorHandler(_Inout_ ErrorInfo& error, _Out_ ContinueAction* action)
{
    *action = ContinueAction_KeepProcessing;

    switch (error.severity)
    {
    case ErrorInfo::Severity_Info:
        DebugInfo("%S failed with hr = 0x%08x (file: %S, line: %d)\n", 
            (error.codeSnippet ? error.codeSnippet : L"<unknown>"), 
            error.errorCode,
            (error.filename ? error.filename : L"<unknown>"),
            error.lineNumber);
        break;

    case ErrorInfo::Severity_Error:
        DebugWarning("%S failed with hr = 0x%08x (file: %S, line: %d)\n", 
            (error.codeSnippet ? error.codeSnippet : L"<unknown>"), 
            error.errorCode,
            (error.filename ? error.filename : L"<unknown>"),
            error.lineNumber);
        break;

    case ErrorInfo::Severity_Fatal:
        DebugError("%S failed with hr = 0x%08x (file: %S, line: %d)\n", 
            (error.codeSnippet ? error.codeSnippet : L"<unknown>"), 
            error.errorCode,
            (error.filename ? error.filename : L"<unknown>"),
            error.lineNumber);
        break;
    }
}

void LogFileErrorHandler(_Inout_ ErrorInfo& error, _Out_ ContinueAction* action)
{
    // TODO: implement this
    UNREFERENCED_PARAMETER(error);
    UNREFERENCED_PARAMETER(action);
}

#ifdef BUILD_TESTS

int RunTests()
{
    size_t cPassed = 0;
    size_t cTotal = 0;

    DebugOut("*** Running in-game test suite...\n");

    for (size_t i = 0; i < _countof(pfnTestList); i++)
    {
        if (pfnTestList[i]())
            cPassed++;

        cTotal++;
    }

    char szSummary[128] = {0};
    sprintf_s(szSummary, "*** Test Summary: %d / %d tests passed.\n", cPassed, cTotal);
    DebugOut("*** Test Summary: %d / %d tests passed.\n", cPassed, cTotal);

    // drain the WM_QUIT out of the message queue since it prevents our message box from showing...
    MSG msg = {0};
    while (GetMessage(&msg, nullptr, 0, 0)) {}
    MessageBoxA(nullptr, szSummary, "Test Summary", MB_OK | MB_ICONINFORMATION);

    return 0;
}

#endif

