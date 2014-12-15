#include "StdAfx.h"
#include "Texture.h"
#include <CoreServices\L3DGraphicsSerializers.h>
#include "Renderer.h"

using namespace Lucid3D;
using namespace GDK;

Texture::Texture(_In_ Renderer* pRenderer, _In_ IContentManagerPtr& spContentManager, _In_ uint64 contentId)
    : ResourceBase(pRenderer, spContentManager, contentId)
{
}

GDK_METHODIMP Texture::OnLoad(_In_ Renderer* pRenderer, _In_ IContentManagerPtr spContentManager, _In_ uint64 contentId)
{
    HRESULT hr = S_OK;

    stde::com_ptr<IStream> spStream;
    DevicePtr spDevice = pRenderer->GetDevice();

    CHECKHR(spContentManager->GetStream(contentId, &spStream));

    _spSRV = nullptr;
    CHECKHR(L3DGraphics::TextureData::Load(spStream, spDevice, &_spSRV));

EXIT
    return hr;
}

GDK_METHODIMP Texture::OnUnload()
{
    HRESULT hr = S_OK;

    // Release resources
    _spSRV = nullptr;

EXIT
    return hr;
}

HRESULT Texture::BindTexture(_In_ ContextPtr& spContext, _In_ UINT slot)
{
    if (!IsLoaded())
    {
        IContentTagPtr spContentTag;
        GetContentManager()->GetContentTag(GetContentId(), &spContentTag);
        DebugWarning("Trying to bind unloaded texture: %s", spContentTag->GetName());
        return S_OK;
    }

    HRESULT hr = S_OK;

    spContext->VSSetShaderResources(slot, 1, &_spSRV);
    spContext->PSSetShaderResources(slot, 1, &_spSRV);

EXIT
    return hr;
}

