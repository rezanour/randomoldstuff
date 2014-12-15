#include "stdafx.h"

FileExtensionDescription g_waveFrontSupportedFileDescriptions[] = {
    {L"obj",            L"Wave Front Model"},
};

WaveFrontModelExtension::WaveFrontModelExtension() :
    _refcount(1)
{

}

WaveFrontModelExtension::~WaveFrontModelExtension()
{

}

// IUnknown
HRESULT STDMETHODCALLTYPE WaveFrontModelExtension::QueryInterface(_In_ REFIID iid, _Deref_out_ void** ppvObject)
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

ULONG STDMETHODCALLTYPE WaveFrontModelExtension::AddRef()
{
    return (ULONG)InterlockedIncrement(&_refcount);
}

ULONG STDMETHODCALLTYPE WaveFrontModelExtension::Release()
{
    ULONG res = (ULONG) InterlockedDecrement(&_refcount);
    if (res == 0) 
    {
        delete this;
    }

    return res;
}

HRESULT WaveFrontModelExtension::GetSupportedFileDescriptions(FileExtensionDescription** ppDescriptions, _Out_ size_t& numDescriptions)
{
    *ppDescriptions = g_waveFrontSupportedFileDescriptions;
    numDescriptions = ARRAYSIZE(g_waveFrontSupportedFileDescriptions);
    return S_OK;
}

HRESULT WaveFrontModelExtension::Import(_In_ LPCWSTR fileName, _In_ IContentStudioResourceFactory* pFactory, _In_ IContentStudioProperties* pProperties)
{
    return E_NOTIMPL;
}

HRESULT WaveFrontModelExtension::Bake(_In_ ITextureResource* pResource, _In_ IStream* pStream)
{
    return E_NOTIMPL;
}

HRESULT WaveFrontModelExtension::Bake(_In_ IGeometryResource* pResource, _In_ IStream* pStream)
{
    return E_NOTIMPL;
}
