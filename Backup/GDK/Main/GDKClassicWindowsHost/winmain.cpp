#include "StdAfx.h"

GDK_METHOD_(void) ParseConfigurationFileFromCommandLine(_Out_ wchar_t** configurationFile);

void MyCustomErrorTracer(_In_ const wchar_t* message)
{
    MessageBox(nullptr, message, L"Engine Error!", MB_OK);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    using namespace GDK;
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);

    try
    {
        stde::ref_ptr<Application> application;
        wchar_t* configurationFile = nullptr;

        ParseConfigurationFileFromCommandLine(&configurationFile);
        Application::Create(hInstance, configurationFile, &application);

        application->Run();
    }
    catch (const std::exception& ex)
    {
        GDK::LogFailure(ex.what());
        return 1;
    }

    return 0;
}

GDK_METHOD_(void) ParseConfigurationFileFromCommandLine(_Out_ wchar_t** configurationFile)
{
    assert(configurationFile);
    *configurationFile = GetCommandLine();

    wchar_t* space = StrChr(*configurationFile, L' ');
    if (space)
    {
        // remove trailing quotes
        wchar_t* end = space;

        while (*(end - 1) == L'"')
        {
            --end;
        }

        (*configurationFile)[end - *configurationFile] = L'\0';

        // make sure the first parameter isn't our executable
        wchar_t* filename = PathFindFileName(*configurationFile);
        wchar_t moduleName[1024] = {0};
        GLEB(GetModuleFileName(nullptr, moduleName, _countof(moduleName)));

        wchar_t* moduleFileName = PathFindFileName(moduleName);

        if (StrCmpI(filename, moduleFileName) == 0)
        {
            // first parameter was just the module name, skip over it
            *configurationFile = space + 1;
        }
    }
}
