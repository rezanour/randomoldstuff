#include "StdAfx.h"

#include <GDK\Graphics.h>
#include "Renderer.h"

using namespace Lucid3D;
using namespace GDK;

// Our graphics factory public export
extern "C" HRESULT GDK_API CreateSubsystem(_In_ SubsystemType type, _Deref_out_ ISubsystem** ppSubsystem);

BOOL DllStartup();
void DllShutdown();

BOOL WINAPI DllMain(_In_ HINSTANCE hinstDLL, _In_ DWORD fdwReason, _In_ LPVOID lpvReserved)
{
    UNREFERENCED_PARAMETER(hinstDLL);
    UNREFERENCED_PARAMETER(lpvReserved);

    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        return DllStartup();
        break;

    case DLL_PROCESS_DETACH:
        DllShutdown();
        break;
    }

    // only matters for PROCESS_ATTACH
    return TRUE;
}

BOOL DllStartup()
{
    SetDebugVerbosity(CoreServices::DebugLevel::Info);
    DebugInfo("L3DGraphics.dll starting up...");
    return TRUE;
}

void DllShutdown()
{
    DebugInfo("L3DGraphics.dll shutting down...");
}

HRESULT GDK_API CreateSubsystem(_In_ SubsystemType type, _Deref_out_ ISubsystem** ppSubsystem)
{
    HRESULT hr = S_OK;

    ISNOTNULL(ppSubsystem, E_POINTER);

    switch (type)
    {
    case SubsystemType::GraphicsSubsystem:
        *ppSubsystem = static_cast<ISubsystem*>(new Renderer);
        break;

    default:
        hr = HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
        break;
    }

EXIT
    return hr;
}

