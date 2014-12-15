#pragma once

#include "DirectXTextureResourceImpl.h"
#include "SpriteFontResourceImpl.h"

class TextureResourceServices
{
public:
    // Public factory
    static HRESULT FromStream(_In_ IStream* pStream, _Deref_out_ ITextureResource** ppResource)
    {
        DirectXTextureResource* pTextureResource = new DirectXTextureResource("dookie");
        *ppResource = pTextureResource;
        return pTextureResource->Load(pStream);
    }

    // Private factory
    static HRESULT FromStream(_In_ IStream* pStream, _Deref_out_ IDirectXTextureResource** ppResource)
    {
        DirectXTextureResource* pTextureResource = new DirectXTextureResource("dookie");
        *ppResource = pTextureResource;
        return pTextureResource->Load(pStream);
    }
};

class SpriteFontResourceServices
{
public:
    // Public factory
    static HRESULT FromStream(_In_ IStream* pStream, _Deref_out_ ISpriteFontResource** ppResource)
    {
        SpriteFontResource* pSpriteFontResource = new SpriteFontResource();
        *ppResource = pSpriteFontResource;
        return pSpriteFontResource->Load(pStream);
    }

    static HRESULT FromLOGFONT(_In_ LOGFONT* plogFont, bool antialias, _Deref_out_ ISpriteFontResource** ppResource)
    {
        HRESULT hr = S_OK;
        SpriteFontResource* pSpriteFontResource = new SpriteFontResource();
        hr = pSpriteFontResource->Initialize(plogFont, antialias);
        if (SUCCEEDED(hr))
        {
            *ppResource = pSpriteFontResource;
        }
        else
        {
            pSpriteFontResource->Release();
        }

        return hr;
    }
};
