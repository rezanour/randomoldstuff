#include "Precomp.h"
#include <ComponentFactory.h>

using GDK::ComponentDescription;
using GDK::Content::ContentArchive;

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

    components[0].factoryId = __uuidof(GDK::Content::IContentFactory);
    components[0].internalId = __uuidof(GDK::Content::ContentArchiveFactory);

    size_t displayNameLength = wcslen(L"Content Archive") + 1;
    assert(displayNameLength < GDK::MaxComponentDisplayNameLength);

    if (displayNameLength > GDK::MaxComponentDisplayNameLength)
    {
        return E_FAIL;
    }

    wcscpy_s(components[0].displayName, L"Content Archive");

    return S_OK;
}

HRESULT CreateComponentFactory(_In_ const ComponentDescription& component, REFIID riid, void** factory)
{
    if (component.factoryId == __uuidof(GDK::Content::IContentFactory))
    {
        if (component.internalId == __uuidof(GDK::Content::ContentArchiveFactory))
        {
            return GDK::Make<GDK::Content::ContentArchiveFactory>().CopyTo(riid, factory);
        }
    }

    return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
}

} // extern "C"
