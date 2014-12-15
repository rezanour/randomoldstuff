#pragma once

#include <GDK\ContentManager.h>
#include <GDK\RuntimeResource.h>
#include <stde\non_copyable.h>
#include <stde\ref_counted.h>
#include "GDKTypes.h"
#include <vector>

namespace Lucid3D
{
    class ContentManager;
    typedef stde::ref_counted_ptr<ContentManager> ContentManagerPtr;

    class ContentManager : stde::non_copyable, public GDK::RefCountedBase<GDK::IContentManager>
    {
    public:
        ContentManager(_In_ const std::string& contentRoot);

        static HRESULT Create(_In_ const std::string& contentRoot, _Inout_ ContentManagerPtr& spContentManager);

        // IContentManager
        GDK_METHOD GetContentTag(_In_ uint64 id, _Out_ GDK::IContentTag** ppContentTag);
        GDK_METHOD GetStream(_In_ uint64 id, _Deref_out_ IStream** ppStream);

        GDK_METHOD RegisterRuntimeResource(_In_ GDK::IRuntimeResource* pResource);
        GDK_METHOD UnregisterRuntimeResource(_In_ GDK::IRuntimeResource* pResource);

        // Methods
        void UnloadAndReleaseAll();

    private:
        std::wstring _contentRoot;
        std::vector<IRuntimeResourcePtr> _resources;
    };
}

