#include "StdAfx.h"

class MockHost : public GDK::RefCounted<GDK::IGDKHost>
{
public:
    // IGDKHost
    GDK_IMETHOD_(GDK::PlatformType) GetPlatformType() const
    {
        return GDK::PlatformType_Windows;
    }

    GDK_IMETHOD GetTitle(_Inout_count_(cchMax) wchar_t* name, _In_ size_t cchMax, _Out_ size_t* cchName)
    {
        if (name && cchMax > 5)
        {
            wcscpy_s(name, cchMax, L"Gooz");
            *cchName = 4;
            return S_OK;
        }

        return E_INVALIDARG;
    }

    GDK_IMETHOD_(void) Exit(_In_ int32_t exitCode)
    {
        exit(exitCode);
    }
};


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nShowCmd);

    HRESULT hr = S_OK;

    MockHost* host = new MockHost;
    GDK::IGDKEngineCore* engine = nullptr;

    CHECK(GDK::CreateEngine(host, &engine));
    //CHECK(engine->EnableLogging(true));

    GDK::Message msg;
    msg.Type = GDK::MessageType_System;
    msg.Category = GDK::MessageCategory_Application;
    msg.Subject = 5; // exit
    msg.Data = 0; // errorCode

    GDK::IGDKMemoryTracker* memoryTracker = nullptr;
    //CHECK(engine->GetMemoryTracker(&memoryTracker));

    //uint64_t startMemory = 0;
    //uint64_t endMemory = 0;
    //CHECK(memoryTracker->GetUsedSystemMemory(&startMemory));

    for (int i = 0; i < 5; ++i)
    {
        CHECK(engine->Tick());
    }

    //CHECK(memoryTracker->GetUsedSystemMemory(&endMemory));

    GDK::SafeRelease(memoryTracker);
    GDK::SafeRelease(engine);
    GDK::SafeRelease(host);

    CHECK(GDK::ObjectTest());

Exit:
    return 0;
}
