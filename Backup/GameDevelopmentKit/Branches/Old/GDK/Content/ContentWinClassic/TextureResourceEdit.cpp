#include "Precomp.h"

using Microsoft::WRL::ComPtr;

namespace GDK {
namespace Content {

// IPersistResource
HRESULT GDKAPI TextureResourceEdit::Save(_In_ GDK::IStream* output)
{
    UNREFERENCED_PARAMETER(output);
    return E_NOTIMPL;
}

void TextureResourceEdit::FreeImages()
{
    _images.clear();
}

HRESULT GDKAPI TextureResourceEdit::Load(_In_ GDK::IStream* input)
{    
    if ( !input )
    {
        return E_INVALIDARG;
    }
    
    HRESULT hr = S_OK;
    ULONG cbRead = 0;

    FreeImages();

    // Read header
    hr = input->Read(&_header, sizeof(_header), &cbRead);
    if (FAILED(hr))
    {
        return hr;
    }

    if (_header.header.Version != TEXTURE_RESOURCE_VERSION)
    {
        return E_INVALIDARG;
    }
    
    // Read images
    for (size_t i = 0; i < _header.metadata.arraySize; i++)
    {
        TEXTURE_RESOURCE_IMAGE resourceImage;
        // must be careful not to stomp over the vector, since that invalidates it's internal structure
        hr = input->Read(&resourceImage, sizeof(resourceImage) - sizeof(resourceImage.pixels), &cbRead);
        if (SUCCEEDED(hr))
        {
            resourceImage.pixels.resize(resourceImage.slicePitch);
            hr = input->Read(resourceImage.pixels.data(), (ULONG)resourceImage.slicePitch, &cbRead);
            if (FAILED(hr))
            {
                break;
            }
            _images.push_back(resourceImage);
        }
        
    }

    return hr;
}

HRESULT GDKAPI TextureResourceEdit::SetName(_In_ const wchar_t* name)
{
    if (!name)
    {
        return E_INVALIDARG;
    }

    _resourceName = name;

    return S_OK;
}

// ITextureResourceEdit
HRESULT GDKAPI TextureResourceEdit::CreateTextureResource(_COM_Outptr_ ITextureResource** resource)
{
    UNREFERENCED_PARAMETER(resource);
    return E_NOTIMPL;
}

// ITextureResource
HRESULT GDKAPI TextureResourceEdit::GetName(_Out_ const wchar_t** name)
{
    if (!name)
    {
        return E_INVALIDARG;
    }

    *name = _resourceName.c_str();

    return S_OK;
}

HRESULT GDKAPI TextureResourceEdit::GetFormat(_Out_ uint32_t* format)
{
    if (!format)
    {
        return E_INVALIDARG;
    }

    *format = _header.metadata.format;

    return S_OK;
}

HRESULT GDKAPI TextureResourceEdit::GetImageCount(_Out_ uint32_t* count)
{
    if (!count)
    {
        return E_INVALIDARG;
    }

    *count = (uint32_t)_header.metadata.arraySize;

    return S_OK;
}

HRESULT GDKAPI TextureResourceEdit::GetSize(_In_ uint32_t index, _Out_ uint32_t* width, uint32_t* height)
{
    if (index >= _header.metadata.arraySize)
    {
        return E_INVALIDARG;
    }

    if (width)
    {
        *width = (uint32_t)_images[index].width;
    }

    if (height)
    {
        *height = (uint32_t)_images[index].height;
    }

    return S_OK;
}

HRESULT GDKAPI TextureResourceEdit::GetPixels(_In_ uint32_t index, _Out_ const byte_t** pixels)
{
    if (index >= _header.metadata.arraySize)
    {
        return E_INVALIDARG;
    }

    if(!pixels)
    {
        return E_INVALIDARG;
    }

    *pixels = _images[index].pixels.data();
    
    return S_OK;
}

TextureResourceEdit::TextureResourceEdit()
{

}

TextureResourceEdit::~TextureResourceEdit()
{
    FreeImages();
}

// ITextureResource

} // Content
} // GDK
