#include "stdafx.h"

FileExtensionDescription g_inboxSupportedFileDescriptions[] = {
    {L"dds",            L"Direct Draw Surface"},
    {L"jpg",            L"JPEG"},
    {L"png",            L"Portable Network Graphics"},
    {L"bmp",            L"Bitmap"},
    {L"tga",            L"TARGA"},
    {L"textureresource",L"Texture Resource"},
};

TextureResourceExtension::TextureResourceExtension() :
    _refcount(1)
{

}

TextureResourceExtension::~TextureResourceExtension()
{

}

// IUnknown
HRESULT STDMETHODCALLTYPE TextureResourceExtension::QueryInterface(_In_ REFIID iid, _Deref_out_ void** ppvObject)
{
    if (ppvObject == nullptr)
    {
        return E_INVALIDARG;
    }

    *ppvObject = nullptr;

    if (iid == __uuidof(IUnknown))
    {
        *ppvObject = static_cast<IUnknown*>(this);
        AddRef();
        return S_OK;
    }

    return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE TextureResourceExtension::AddRef()
{
    return (ULONG)InterlockedIncrement(&_refcount);
}

ULONG STDMETHODCALLTYPE TextureResourceExtension::Release()
{
    ULONG res = (ULONG) InterlockedDecrement(&_refcount);
    if (res == 0) 
    {
        delete this;
    }

    return res;
}

HRESULT TextureResourceExtension::GetSupportedFileDescriptions(FileExtensionDescription** ppDescriptions, _Out_ size_t& numDescriptions)
{
    *ppDescriptions = g_inboxSupportedFileDescriptions;
    numDescriptions = ARRAYSIZE(g_inboxSupportedFileDescriptions);
    return S_OK;
}

HRESULT TextureResourceExtension::Import(_In_ LPCWSTR filePath, _In_ IContentStudioResourceFactory* pFactory, _In_ IContentStudioProperties* pProperties)
{
    HRESULT hr = S_OK;

    ITextureResource* pTextureResource = nullptr;
    hr = pFactory->CreateTextureResource("dookie" ,&pTextureResource);
    
    if (SUCCEEDED(hr))
    {
        hr = pTextureResource->Load(filePath);
    }

    if (SUCCEEDED(hr))
    {
        hr = pProperties->SetInterface("ImportedTextureResource", pTextureResource);
    }

    if (pTextureResource != nullptr)
    {
        pTextureResource->Release();
    }

    return hr;
}

HRESULT TextureResourceExtension::Bake(_In_ ITextureResource* pResource, _In_ IStream* pStream)
{
    return E_NOTIMPL;
}

HRESULT TextureResourceExtension::Bake(_In_ IGeometryResource* pResource, _In_ IStream* pStream)
{
    return E_NOTIMPL;
}
