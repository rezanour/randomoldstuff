#pragma once

namespace Lucid3D
{
    template <typename DerivedT>
    std::map<uint64, stde::ref_counted_ptr<DerivedT>> ResourceBase<DerivedT>::s_cache;

    template <typename DerivedT>
    ResourceBase<DerivedT>::ResourceBase(_In_ Renderer* pRenderer, _In_ IContentManagerPtr& spContentManager, _In_ uint64 contentId)
        : _pRenderer(pRenderer), _spContentManager(spContentManager), _contentId(contentId), _isLoaded(false)
    {
        Assert(pRenderer);
        Assert(spContentManager);
    }

    template <typename DerivedT>
    ResourceBase<DerivedT>::~ResourceBase()
    {
        if (_spContentManager)
            _spContentManager->UnregisterRuntimeResource(this);
    }

    template <typename DerivedT>
    HRESULT ResourceBase<DerivedT>::Create( _In_ Renderer* pRenderer, 
                                            _In_ IContentManagerPtr& spContentManager, 
                                            _In_ uint64 contentId,
                                            _In_ bool loadNow,
                                            _Out_ stde::ref_counted_ptr<DerivedT>& spResource)
    {
        HRESULT hr = S_OK;

        ISNOTNULL(spContentManager, E_INVALIDARG);

        // TODO: Validate this stuff
        //ISTRUE(IsSupportedType(spContentTag->GetType()), E_INVALIDARG);

        {
            // See if we already have this object cached
            Cache::iterator it = s_cache.find(contentId);
            if (it != s_cache.end())
            {
                spResource.reset(it->second);
            }
            else
            {
                spResource.attach(new DerivedT(pRenderer, spContentManager, contentId));
                ISNOTNULL(spResource, E_OUTOFMEMORY);

                CHECKHR(spContentManager->RegisterRuntimeResource(spResource));

                // cache it
                s_cache[contentId] = spResource;
            }
        }

        if (loadNow && !spResource->IsLoaded())
        {
            CHECKHR(spResource->Load());
        }

    EXIT
        if (FAILED(hr))
        {
            // don't return half initialized object on failure
            spResource.reset();
        }

        return hr;
    }

    // IRuntimeResource
    template <typename DerivedT>
    GDK_IMETHODIMP_(uint64) ResourceBase<DerivedT>::GetContentId() const
    {
        return _contentId;
    }

    template <typename DerivedT>
    GDK_IMETHODIMP_(bool) ResourceBase<DerivedT>::IsLoaded() const
    {
        return _isLoaded;
    }

    template <typename DerivedT>
    GDK_IMETHODIMP ResourceBase<DerivedT>::Load()
    {
        if (_isLoaded)
            return S_OK;

        HRESULT hr = OnLoad(_pRenderer, _spContentManager, _contentId);
        if (SUCCEEDED(hr))
        {
            _isLoaded = true;
        }
        return hr;
    }

    template <typename DerivedT>
    GDK_IMETHODIMP ResourceBase<DerivedT>::Unload()
    {
        if (!_isLoaded)
            return S_OK;

        HRESULT hr = OnUnload();
        if (SUCCEEDED(hr))
        {
            _isLoaded = false;
        }
        return hr;
    }
}
