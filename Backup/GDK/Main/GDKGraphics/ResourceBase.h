#pragma once

#include "GraphicsTypes.h"

#include <stde\com_ptr.h>
#include <stde\ref_counted.h>
#include <stde\non_copyable.h>

#include <GDK\RuntimeResource.h>

#include <map>

namespace Lucid3D
{
    class Renderer;

    template <typename DerivedT>
    class ResourceBase : stde::non_copyable, public GDK::RefCounted<GDK::IRuntimeResource>
    {
    public:
        virtual ~ResourceBase();

        static HRESULT Create(  _In_ Renderer* pRenderer,
                                _In_ IContentManagerPtr& spContentManager,
                                _In_ uint64 contentId,
                                _In_ bool loadNow,
                                _Out_ stde::ref_counted_ptr<DerivedT>& spResource);

        // IRuntimeResource
        GDK_IMETHOD_(uint64) GetContentId() const;
        GDK_IMETHOD_(bool) IsLoaded() const;

        GDK_IMETHOD Load();
        GDK_IMETHOD Unload();

    protected:
        ResourceBase(_In_ Renderer* pRenderer, _In_ IContentManagerPtr& spContentManager, _In_ uint64 contentId);

        // Methods
        IContentManagerPtr GetContentManager() const { return _spContentManager; }
        GDK_IMETHOD OnLoad(_In_ Renderer* pRenderer, _In_ IContentManagerPtr spContentManager, _In_ uint64 contentId) = 0;
        GDK_IMETHOD OnUnload() = 0;

    private:
        Renderer* _pRenderer;
        IContentManagerPtr _spContentManager;
        uint64 _contentId;
        bool _isLoaded;

        // Cache
        typedef std::map<uint64, stde::ref_counted_ptr<DerivedT>> Cache;
        static Cache s_cache;
    };
}

#include "ResourceBase.inl"