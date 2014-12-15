#pragma once

namespace GDK
{
    class ContentManager;
    typedef stde::ref_counted_ptr<ContentManager> ContentManagerPtr;

    class ContentManager : stde::non_copyable, public GDK::RefCountedBase<GDK::IContentManager>
    {
    public:
        ContentManager(_In_ const std::string& contentRoot);

        static HRESULT Create(_In_ const std::string& contentRoot, _Inout_ ContentManagerPtr& spContentManager);

        // IContentManager
        GDK_IMETHOD GetContentTag(_In_ uint64 id, _Out_ GDK::IContentTag** ppContentTag);
        GDK_IMETHOD GetStream(_In_ uint64 id, _Deref_out_ IStream** ppStream);

        GDK_IMETHOD RegisterRuntimeResource(_In_ GDK::IRuntimeResource* pResource);
        GDK_IMETHOD UnregisterRuntimeResource(_In_ GDK::IRuntimeResource* pResource);

        // Methods
        void UnloadAndReleaseAll();

    private:
        std::wstring _contentRoot;
        std::vector<IRuntimeResourcePtr> _resources;
    };
}

