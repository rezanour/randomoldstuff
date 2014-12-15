#if defined(_MSC_VER)
#pragma once
#endif

#ifndef _GDK_CONTENTMANAGER_H_
#define _GDK_CONTENTMANAGER_H_

struct IStream;

namespace GDK
{
    class Scene;
    struct IContent;
    struct IGameObject;
    struct IComponent;

    class ContentManager;
    typedef stde::ref_ptr<ContentManager> ContentManagerPtr;

    class ContentManager : private stde::non_copyable, public RefCounted<>
    {
    public:
        static GDK_METHOD_(ContentManagerPtr) Create(_In_ const wchar_t* contentRoot);

        // Generic access to a stream
        GDK_METHOD_(stde::com_ptr<IStream>) GetRawStream(_In_ const uint64_t& id) const;

        // Read and factory out types
        GDK_METHOD_(stde::ref_ptr<Scene>)       LoadScene(_In_ const uint64_t& id);
        GDK_METHOD_(stde::ref_ptr<IGameObject>) LoadGameObject(_In_ const uint64_t& id);
        GDK_METHOD_(stde::ref_ptr<IComponent>)  LoadComponent(_In_ const uint64_t& id);
        GDK_METHOD_(stde::ref_ptr<IContent>)    LoadContent(_In_ const uint64_t& id);

    private:
        ContentManager();
        ~ContentManager();

        std::wstring _contentRoot;

        typedef std::map<uint64_t, std::wstring> StorageMap;
        StorageMap _relativePaths;

        typedef std::map<uint64_t, stde::ref_ptr<IContent>> ContentMap;
        ContentMap _existingContent;
    };

} // GDK

#endif // _GDK_CONTENTMANAGER_H_
