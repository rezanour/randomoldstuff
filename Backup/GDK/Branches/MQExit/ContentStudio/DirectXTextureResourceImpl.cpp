#include "stdafx.h"
#include <wincodec.h>

DirectXTextureResource::DirectXTextureResource(PCSTR name) :
    _refcount(1)
{
    _name = name;
}

DirectXTextureResource::~DirectXTextureResource()
{

}

HRESULT STDMETHODCALLTYPE DirectXTextureResource::QueryInterface(_In_ REFIID iid, _Deref_out_ void** ppvObject)
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
    else if (iid == __uuidof(ITextureResource))
    {
        *ppvObject = static_cast<ITextureResource*>(this);
        AddRef();
        return S_OK;
    }
    else if (iid == __uuidof(IDirectXTextureResource))
    {
        *ppvObject = static_cast<IDirectXTextureResource*>(this);
        AddRef();
        return S_OK;
    }

    return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE DirectXTextureResource::AddRef()
{
    return (ULONG)InterlockedIncrement(&_refcount);
}

ULONG STDMETHODCALLTYPE DirectXTextureResource::Release()
{
    ULONG res = (ULONG) InterlockedDecrement(&_refcount);
    if (res == 0) 
    {
        delete this;
    }

    return res;
}

HRESULT DirectXTextureResource::Save(_In_ IStream* pStream, _In_ TextureResourceSaveFormat outputFormat)
{
    HRESULT hr = E_NOTIMPL;

    switch(outputFormat)
    {
        case TextureResourceSaveFormat::bmp:
            return DirectX::SaveToWICStream(_scratchImage.GetImages(), _scratchImage.GetImageCount(), DirectX::WIC_FLAGS::WIC_FLAGS_ALL_FRAMES, GUID_ContainerFormatBmp ,pStream);
        
        case TextureResourceSaveFormat::png:
            return DirectX::SaveToWICStream(_scratchImage.GetImages(), _scratchImage.GetImageCount(), DirectX::WIC_FLAGS::WIC_FLAGS_ALL_FRAMES, GUID_ContainerFormatPng ,pStream);
        
        case TextureResourceSaveFormat::jpg:
            return DirectX::SaveToWICStream(_scratchImage.GetImages(), _scratchImage.GetImageCount(), DirectX::WIC_FLAGS::WIC_FLAGS_ALL_FRAMES, GUID_ContainerFormatJpeg ,pStream);
        
        case TextureResourceSaveFormat::tga:
            return DirectX::SaveToTGAStream(*_scratchImage.GetImages(), pStream);

        case TextureResourceSaveFormat::dds9:
            return DirectX::SaveToDDSStream(_scratchImage.GetImages(), _scratchImage.GetImageCount(), _scratchImage.GetMetadata(), DirectX::DDS_FLAGS_NONE, pStream);

        case TextureResourceSaveFormat::dds:
            return DirectX::SaveToDDSStream(_scratchImage.GetImages(), _scratchImage.GetImageCount(), _scratchImage.GetMetadata(), DirectX::DDS_FLAGS_FORCE_DX10_EXT, pStream);

        case TextureResourceSaveFormat::textureresource:
            return DirectX::SaveToTextureResourceStream(_name.c_str(), _scratchImage.GetImages(), _scratchImage.GetImageCount(), _scratchImage.GetMetadata(), DirectX::TR_FLAGS_NONE, pStream);
        default:
            break;
    }

    return E_NOTIMPL;
}

HRESULT DirectXTextureResource::Load(_In_ IStream* pStream)
{
    DirectX::TexMetadata metadata;
    HRESULT hr = S_OK;

    // Give TextureResource decoder a chance
    hr = DirectX::LoadFromTextureResourceStream(pStream, &metadata, _scratchImage);
    if (FAILED(hr))
    {
        CoreServices::FileStream::Seek(pStream,STREAM_SEEK_SET,0);
        // Give DDS decoder a chance
        hr = DirectX::LoadFromDDSStream(pStream, DirectX::DDS_FLAGS_FORCE_DX10_EXT, &metadata, _scratchImage);
        if (FAILED(hr))
        {
            CoreServices::FileStream::Seek(pStream,STREAM_SEEK_SET,0);
            // Give WIC decoder a chance
            hr = DirectX::LoadFromWICStream(pStream, DirectX::WIC_FLAGS_FORCE_RGB, &metadata, _scratchImage);
            if (FAILED(hr))
            {
                CoreServices::FileStream::Seek(pStream,STREAM_SEEK_SET,0);
                // Give TGA decoder a chance
                hr = DirectX::LoadFromTGAStream(pStream, &metadata, _scratchImage);
            }
        }
    }

    return hr;
}

HRESULT DirectXTextureResource::Load(_In_ LPCWSTR filePath)
{
    if (filePath == nullptr)
    {
        return E_INVALIDARG;
    }
    HRESULT hr = S_OK;
    stde::com_ptr<IStream> spStream;
    
    hr = CoreServices::FileStream::Create(filePath, true, &spStream);
    if (SUCCEEDED(hr))
    {
        return Load(spStream);
    }

    return hr;
}

HRESULT DirectXTextureResource::Initialize( _In_ TEXTURE_RESOURCE_METADATA* pMetadata )
{
    DirectX::TexMetadata mdata;
    mdata.arraySize = pMetadata->arraySize;
    mdata.depth     = pMetadata->depth;
    mdata.dimension = (DirectX::TEX_DIMENSION)pMetadata->dimension;
    mdata.format    = (DXGI_FORMAT)pMetadata->format;
    mdata.height    = pMetadata->height;
    mdata.mipLevels = pMetadata->mipLevels;
    mdata.miscFlags = pMetadata->miscFlags;
    mdata.width     = pMetadata->width;

    return _scratchImage.Initialize(mdata);
}

HRESULT DirectXTextureResource::Initialize1D( _In_ TEXTURE_RESOURCE_FORMAT fmt, _In_ size_t length, _In_ size_t arraySize, _In_ size_t mipLevels )
{
    return _scratchImage.Initialize1D((DXGI_FORMAT)fmt, length, arraySize, mipLevels);
}

HRESULT DirectXTextureResource::Initialize2D( _In_ TEXTURE_RESOURCE_FORMAT fmt, _In_ size_t width, _In_ size_t height, _In_ size_t arraySize, _In_ size_t mipLevels )
{
    return _scratchImage.Initialize2D((DXGI_FORMAT)fmt, width, height, arraySize, mipLevels);
}

HRESULT DirectXTextureResource::Initialize3D( _In_ TEXTURE_RESOURCE_FORMAT fmt, _In_ size_t width, _In_ size_t height, _In_ size_t depth, _In_ size_t mipLevels )
{
    return _scratchImage.Initialize3D((DXGI_FORMAT)fmt, width, height, depth, mipLevels);
}

HRESULT DirectXTextureResource::InitializeCube( _In_ TEXTURE_RESOURCE_FORMAT fmt, _In_ size_t width, _In_ size_t height, _In_ size_t nCubes, _In_ size_t mipLevels )
{
    return _scratchImage.InitializeCube((DXGI_FORMAT)fmt, width, height, nCubes, mipLevels);
}

HRESULT DirectXTextureResource::InitializeFromImage( _In_ TEXTURE_RESOURCE_IMAGE* pImage, _In_ bool allow1D)
{
    DirectX::Image image;
    image.format     = (DXGI_FORMAT)pImage->format;
    image.height     = pImage->height;
    image.pixels     = pImage->pixels;
    image.rowPitch   = pImage->rowPitch;
    image.slicePitch = pImage->slicePitch;
    image.width      = pImage->width;

    return _scratchImage.InitializeFromImage(image, allow1D);
}

HRESULT DirectXTextureResource::InitializeArrayFromImages( _In_count_(nImages) TEXTURE_RESOURCE_IMAGE* images, _In_ size_t nImages, _In_ bool allow1D)
{
    return E_NOTIMPL;
}

HRESULT DirectXTextureResource::InitializeCubeFromImages( _In_count_(nImages) TEXTURE_RESOURCE_IMAGE* images, _In_ size_t nImages )
{
    return E_NOTIMPL;
}

HRESULT DirectXTextureResource::Initialize3DFromImages( _In_count_(depth) TEXTURE_RESOURCE_IMAGE* images, _In_ size_t depth )
{
    return E_NOTIMPL;
}

HRESULT DirectXTextureResource::GetMetadata( _In_ TEXTURE_RESOURCE_METADATA* pMetadata )
{
    HRESULT hr = S_OK;
    DirectX::TexMetadata mdata;

    mdata = _scratchImage.GetMetadata();

    pMetadata->arraySize = mdata.arraySize;
    pMetadata->depth     = mdata.depth;
    pMetadata->dimension = (TEXTURE_RESOURCE_DIMENSION)mdata.dimension;
    pMetadata->format    =  (TEXTURE_RESOURCE_FORMAT)mdata.format;
    pMetadata->height    = mdata.height;
    pMetadata->mipLevels = mdata.mipLevels;
    pMetadata->miscFlags = mdata.miscFlags;
    pMetadata->width     = mdata.width;

    return S_OK;
}

HRESULT DirectXTextureResource::GetImage(_In_ size_t mip, _In_ size_t item, _In_ size_t slice, TEXTURE_RESOURCE_IMAGE** ppImage)
{
    if (!ppImage)
    {
        return E_INVALIDARG;
    }

    if (_scratchImage.GetImage(mip, item, slice) == nullptr)
    {
        return E_UNEXPECTED;
    }

    TEXTURE_RESOURCE_IMAGE* pImage = *ppImage;

    pImage->format     = (TEXTURE_RESOURCE_FORMAT)_scratchImage.GetImage(mip, item, slice)->format;
    pImage->height     = _scratchImage.GetImage(mip, item, slice)->height;
    pImage->pixels     = _scratchImage.GetImage(mip, item, slice)->pixels;
    pImage->rowPitch   = _scratchImage.GetImage(mip, item, slice)->rowPitch;
    pImage->slicePitch = _scratchImage.GetImage(mip, item, slice)->slicePitch;
    pImage->width      = _scratchImage.GetImage(mip, item, slice)->width;

    return S_OK;
}

HRESULT DirectXTextureResource::GetPixels(uint8_t** ppPixels, size_t* ppixelsSize)
{
    if (!ppPixels || !ppixelsSize)
    {
        return E_INVALIDARG;
    }

    *ppPixels = _scratchImage.GetPixels();
    *ppixelsSize = _scratchImage.GetPixelsSize();

    return S_OK;
}

HRESULT DirectXTextureResource::GetImageCount(size_t* pnumImages)
{
    if (!pnumImages)
    {
        return E_INVALIDARG;
    }

    *pnumImages = _scratchImage.GetImageCount();

    return S_OK;
}

HRESULT DirectXTextureResource::SetName(LPCSTR name)
{
    if (name == nullptr)
    {
        return E_INVALIDARG;
    }

    _name = name;

    return S_OK;
}

HRESULT DirectXTextureResource::GetId(uint64& id)
{
    id = _scratchImage.GetId();
    return S_OK;
}

HRESULT DirectXTextureResource::CreateShaderResourceView(_In_ ID3D11Device* pDevice, _Deref_out_ ID3D11ShaderResourceView** ppShaderResourceView)
{
    if (!pDevice || !ppShaderResourceView)
    {
        return E_INVALIDARG;
    }

    return DirectX::CreateShaderResourceView(pDevice, _scratchImage.GetImages(), _scratchImage.GetImageCount(), _scratchImage.GetMetadata(), ppShaderResourceView);
}

HRESULT DirectXTextureResource::CreateHBITMAP(HBITMAP* phBitmap)
{
    return DirectX::SaveToHBITMAP(_scratchImage.GetImages(), _scratchImage.GetImageCount(), phBitmap);
}

HRESULT DirectXTextureResource::Create(PCSTR name, ITextureResource** ppResource)
{
    if (ppResource == nullptr)
    {
        return E_INVALIDARG;
    }
    
    DirectXTextureResource* pResource = new DirectXTextureResource(name);
    if (pResource == nullptr)
    {
        return E_OUTOFMEMORY;
    }
    
    *ppResource = pResource;

    return S_OK;
}