#pragma once

class WaveFrontModelExtension : public IContentStudioDataExtension
{
public:
    WaveFrontModelExtension();
    virtual ~WaveFrontModelExtension();

    // IUnknown
    HRESULT STDMETHODCALLTYPE QueryInterface(_In_ REFIID iid, _Deref_out_ void** ppvObject);
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // IContentStudioDataExtension
    HRESULT GetSupportedFileDescriptions(FileExtensionDescription** ppDescriptions, _Out_ size_t& numDescriptions);
    HRESULT Import(_In_ LPCWSTR fileName, _In_ IContentStudioResourceFactory* pFactory, _In_ IContentStudioProperties* pProperties);
    HRESULT Bake(_In_ ITextureResource* pResource, _In_ IStream* pStream);
    HRESULT Bake(_In_ IGeometryResource* pResource, _In_ IStream* pStream);
    
private:
    long _refcount;
};

