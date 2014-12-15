#include "StdAfx.h"
#include "SubsystemManager.h"

using namespace Lucid3D;
using namespace GDK;

SubsystemManager::SubsystemManager()
{
}

SubsystemManager::~SubsystemManager()
{
}

HRESULT SubsystemManager::CreateRenderer(_In_ const std::string& module, _In_ const GraphicsSettings& settings)
{
    HRESULT hr = S_OK;

    HMODULE hModule = nullptr;

    if (!_spRenderer)
    {
        DebugInfo("Loading %s...", module.c_str());

        hModule = LoadLibraryA(module.c_str());
        ISNOTNULL(hModule, E_FAIL);

        pfnCreateSubsystem pfnFactory = (pfnCreateSubsystem)GetProcAddress(hModule, "CreateSubsystem");
        ISNOTNULL(pfnFactory, E_FAIL);

        ISubsystem* pSubsystem = nullptr;
        CHECKHR(pfnFactory(SubsystemType::GraphicsSubsystem, &pSubsystem));
        _spRenderer.attach(static_cast<IRenderer*>(pSubsystem));
        CHECKHR(_spRenderer->Initialize(settings));

        // TODO: Set the IContentManager once we have it
        // CHECKHR(_spGraphicsSystem->SetContentManager());

        // REVIEW: Currently, we never unload the DLL. It lasts until the end of the process
        // since we can't easily tell when we're done with the pointer.
    }

EXIT
    if (hModule && FAILED(hr))
    {
        _spRenderer = nullptr;
        // if we failed to create our subsystem, then free the library.
        FreeLibrary(hModule);
    }

    return hr;
}

