// ContentManager is responsible for resolving ContentTag's to data streams at runtime
#pragma once

#include "Platform.h"
#include "RefCounted.h"
#include "ContentTypes.h"
#include "ContentTag.h"

namespace GDK
{
    struct IRuntimeResource;

    struct IContentManager : public IRefCounted
    {
        GDK_IMETHOD GetContentTag(_In_ uint64 id, _Out_ IContentTag** ppContentTag) = 0;
        GDK_IMETHOD GetStream(_In_ uint64 id, _Deref_out_ IStream** ppStream) = 0;

        GDK_IMETHOD RegisterRuntimeResource(_In_ IRuntimeResource* pResource) = 0;
        GDK_IMETHOD UnregisterRuntimeResource(_In_ IRuntimeResource* pResource) = 0;
    };
}

