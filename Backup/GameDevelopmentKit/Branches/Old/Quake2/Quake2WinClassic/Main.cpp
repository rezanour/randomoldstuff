#include "Precomp.h"
#include "Quake2Game.h"
#include <ComponentFactory.h>

using Microsoft::WRL::ComPtr;
using namespace GDK;

extern "C"
{

size_t GetNumComponents()
{
    return 1;
}

HRESULT GetComponents(_Inout_updates_to_(maxComponents, *numComponents) ComponentDescription* components, _In_ size_t maxComponents, _Out_ size_t* numComponents)
{
    if (!components || !numComponents)
    {
        return E_INVALIDARG;
    }

    if (maxComponents < GetNumComponents())
    {
        return HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
    }

    *numComponents = GetNumComponents();

    components[0].factoryId = __uuidof(GDK::IGameFactory);
    components[0].internalId = __uuidof(Quake2::Quake2GameFactory);

    size_t displayNameLength = wcslen(Quake2::Quake2Game::DisplayName) + 1;
    assert(displayNameLength < GDK::MaxComponentDisplayNameLength);

    if (displayNameLength > GDK::MaxComponentDisplayNameLength)
    {
        return E_FAIL;
    }

    wcscpy_s(components[0].displayName, Quake2::Quake2Game::DisplayName);

    return S_OK;
}

HRESULT CreateComponentFactory(_In_ const ComponentDescription& component, REFIID riid, void** factory)
{
    if (component.factoryId == __uuidof(GDK::IGameFactory))
    {
        if (component.internalId == __uuidof(Quake2::Quake2GameFactory))
        {
            return GDK::Make<Quake2::Quake2GameFactory>().CopyTo(riid, factory);
        }
    }

    return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
}

} // extern "C"