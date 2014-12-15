#pragma once

class DirectXTextureResource : public IDirectXTextureResource
{
public:
    DirectXTextureResource(PCSTR name);
    virtual ~DirectXTextureResource();

    // IUnknown
    HRESULT STDMETHODCALLTYPE QueryInterface(_In_ REFIID iid, _Deref_out_ void** ppvObject);
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // ITextureResource
    HRESULT Save(_In_ IStream* pStream, _In_ TextureResourceSaveFormat outputFormat);
    HRESULT Load(_In_ IStream* pStream);
    HRESULT Load(_In_ LPCWSTR filePath);

    HRESULT Initialize( _In_ TEXTURE_RESOURCE_METADATA* pMetadata );

    HRESULT Initialize1D( _In_ TEXTURE_RESOURCE_FORMAT fmt, _In_ size_t length, _In_ size_t arraySize, _In_ size_t mipLevels );
    HRESULT Initialize2D( _In_ TEXTURE_RESOURCE_FORMAT fmt, _In_ size_t width, _In_ size_t height, _In_ size_t arraySize, _In_ size_t mipLevels );
    HRESULT Initialize3D( _In_ TEXTURE_RESOURCE_FORMAT fmt, _In_ size_t width, _In_ size_t height, _In_ size_t depth, _In_ size_t mipLevels );
    HRESULT InitializeCube( _In_ TEXTURE_RESOURCE_FORMAT fmt, _In_ size_t width, _In_ size_t height, _In_ size_t nCubes, _In_ size_t mipLevels );

    HRESULT InitializeFromImage( _In_ TEXTURE_RESOURCE_IMAGE* pImage, _In_ bool allow1D);
    HRESULT InitializeArrayFromImages( _In_count_(nImages) TEXTURE_RESOURCE_IMAGE* images, _In_ size_t nImages, _In_ bool allow1D);
    HRESULT InitializeCubeFromImages( _In_count_(nImages) TEXTURE_RESOURCE_IMAGE* images, _In_ size_t nImages );
    HRESULT Initialize3DFromImages( _In_count_(depth) TEXTURE_RESOURCE_IMAGE* images, _In_ size_t depth );

    HRESULT GetMetadata( _In_ TEXTURE_RESOURCE_METADATA* pMetadata );
    HRESULT GetImage(_In_ size_t mip, _In_ size_t item, _In_ size_t slice, TEXTURE_RESOURCE_IMAGE** ppImage);
    HRESULT GetPixels(uint8_t** ppPixels, size_t* ppixelsSize);
    HRESULT GetImageCount(size_t* pnumImages);

    HRESULT SetName(LPCSTR name);
    HRESULT GetId(uint64& id);

    // IDirectXTextureResource
    HRESULT CreateShaderResourceView(_In_ ID3D11Device* pDevice, _Deref_out_ ID3D11ShaderResourceView** ppShaderResourceView);
    HRESULT CreateHBITMAP(HBITMAP* phBitmap);

    static HRESULT Create(PCSTR name, ITextureResource** ppResource);
private:
    long _refcount;
    std::string _name;
    DirectX::ScratchImage _scratchImage;
};