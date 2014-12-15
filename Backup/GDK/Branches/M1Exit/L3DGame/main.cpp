#include "StdAfx.h"

#include <Shlwapi.h>
#include "GameApplication.h"

#include "TestList.h"

using namespace CoreServices;

HRESULT LoadConfiguration(_In_ const std::string& commandLine, _Inout_ ConfigurationPtr& spConfiguration);

#ifdef BUILD_TESTS
int RunTests();
#endif

int CALLBACK WinMain(__in HINSTANCE hInstance, __in HINSTANCE hPrevInstance, __in LPSTR lpCmdLine, __in int nCmdShow)
{
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(nCmdShow);

    HRESULT hr = S_OK;

    ConfigurationPtr spConfiguration;

    CHECKHR(LoadConfiguration(lpCmdLine, spConfiguration));

#ifdef BUILD_TESTS
    if (spConfiguration->GetValue<bool>("RunTests", false))
    {
        return RunTests();
    }
#endif

    {
        Lucid3D::GameApplication game(spConfiguration);
        CHECKHR(game.Run());
    }

EXIT
    return FAILED(hr); // return 0 if succeeded
}

HRESULT LoadConfiguration(_In_ const std::string& commandLine, _Inout_ ConfigurationPtr& spConfiguration)
{
    UNREFERENCED_PARAMETER(commandLine);

    HRESULT hr = S_OK;

    std::string settingsFile;

    // TODO: First check command line for settings file

    // If no override on command line, fall back to an ini file with the same name as the process
    wchar_t modulePath[MAX_PATH] = {0};
    ISTRUE(GetModuleFileName(nullptr, modulePath, _countof(modulePath)), HRESULT_FROM_WIN32(GetLastError()));

    wchar_t* fileName = PathFindFileName(modulePath);
    ISTRUE(PathRenameExtension(fileName, L".ini"), E_FAIL);

    settingsFile = stde::to_string(std::wstring(fileName));

    if (settingsFile.empty() || !PathFileExistsA(settingsFile.c_str()))
    {
        CHECKHR(Configuration::Create(&spConfiguration));
    }
    else
    {
        stde::com_ptr<IStream> spStream;
        CHECKHR(FileStream::Create(stde::to_wstring(settingsFile), true, &spStream));
        CHECKHR(Configuration::Load(spStream, &spConfiguration));
    }

    {
        std::string relativeContentRoot = spConfiguration->GetStringValue("ContentRoot", "");
        wchar_t contentPath[MAX_PATH] = {0};
        ISTRUE(PathRemoveFileSpec(modulePath), E_FAIL);
        ISNOTNULL(PathCombine(contentPath, modulePath, stde::to_wstring(relativeContentRoot).c_str()), E_FAIL);
        spConfiguration->SetStringValue("ContentRoot", stde::to_string(std::wstring(contentPath)));
    }

    SetDebugVerbosity(static_cast<DebugLevel>(spConfiguration->GetValue("DebugVerbosity", static_cast<int>(DebugLevel::Warning))));

EXIT
    return hr;
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

