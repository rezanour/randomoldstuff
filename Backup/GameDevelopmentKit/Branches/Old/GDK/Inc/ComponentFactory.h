#pragma once

namespace GDK
{
    const size_t MaxComponentDisplayNameLength = 128;

    struct ComponentDescription
    {
        CLSID factoryId;
        GUID internalId;
        wchar_t displayName[MaxComponentDisplayNameLength];
    };

    //
    // Component module exports
    // All modules exporting components should export the following methods
    //
    // GetNumComponents
    // GetComponents
    // CreateComponentFactory
    //

    typedef size_t (*pfnGetNumComponents)();
    typedef HRESULT (*pfnGetComponents)(_Inout_updates_to_(maxComponents, *numComponents) ComponentDescription* components, _In_ size_t maxComponents, _Out_ size_t* numComponents);
    typedef HRESULT (*pfnCreateComponentFactory)(_In_ const ComponentDescription& component, REFIID riid, void** factory);
}

//
// Query available modules
//

extern "C"
{
    size_t ComponentFinderGetNumComponents();
    HRESULT ComponentFinderGetComponents(_Inout_updates_to_(maxComponents, *numComponents) GDK::ComponentDescription* components, _In_ size_t maxComponents, _Out_ size_t* numComponents);
    HRESULT ComponentFinderCreateComponentFactory(_In_ const GDK::ComponentDescription& component, REFIID riid, void** factory);
    void UnloadAllComponentModules();
}
