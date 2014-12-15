#pragma once

#include <d3d11.h>
#include "ResourceBase.h"

namespace Lucid3D
{
    class Renderer;

    class Texture;
    typedef stde::ref_counted_ptr<Texture> TexturePtr;

    class Texture : public ResourceBase<Texture>
    {
    public:
        Texture(_In_ Renderer* pRenderer, _In_ IContentManagerPtr& spContentManager, _In_ uint64 contentId);
        
        // We don't hand out the SRV if we're going to be serious about managing the resource. Get the context, and we set it internally
        HRESULT BindTexture(_In_ ContextPtr& spContext, _In_ UINT slot);

    protected:

        GDK_IMETHOD OnLoad(_In_ Renderer* pRenderer, _In_ IContentManagerPtr spContentManager, _In_ uint64 contentId);
        GDK_IMETHOD OnUnload();

    private:
        ShaderResourcePtr _spSRV;
    };
}

