#include "Precomp.h"

using Microsoft::WRL::ComPtr;

namespace GDK {
namespace Content {

TextureResource::TextureResource(_In_ std::istream* stream)
{
    // Read header
    stream->read((char *)&_header, sizeof(_header));

    if (_header.header.Version != TEXTURE_RESOURCE_VERSION)
    {
        GDK_THROW(E_INVALIDARG, L"Header is invalid");
    }

    // Read images
    for (size_t i = 0; i < _header.metadata.arraySize; i++)
    {
        TEXTURE_RESOURCE_IMAGE resourceImage = {0};
        stream->read((char *)&resourceImage, sizeof(resourceImage));
        resourceImage.pixels.reserve(resourceImage.slicePitch);
        stream->read((char *)&resourceImage.pixels.front(), resourceImage.slicePitch);
        _images.push_back(resourceImage);
    }
}

// ITextureResource
HRESULT GDKAPI TextureResource::GetName(_Out_ const wchar_t** name)
{
    if (!name)
    {
        return E_INVALIDARG;
    }

    *name = _resourceName.c_str();

    return S_OK;
}

HRESULT GDKAPI TextureResource::GetFormat(_Out_ uint32_t* format)
{
    if (!format)
    {
        return E_INVALIDARG;
    }

    *format = _header.metadata.format;

    return S_OK;
}

HRESULT GDKAPI TextureResource::GetImageCount(_Out_ uint32_t* count)
{
    if (!count)
    {
        return E_INVALIDARG;
    }

    *count = (uint32_t)_header.metadata.arraySize;

    return S_OK;
}

HRESULT GDKAPI TextureResource::GetSize(_In_ uint32_t index, _Out_ uint32_t* width, uint32_t* height)
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

HRESULT GDKAPI TextureResource::GetPixels(_In_ uint32_t index, _Out_ byte_t** pixels)
{
    if (index >= _header.metadata.arraySize)
    {
        return E_INVALIDARG;
    }

    if(!pixels)
    {
        return E_INVALIDARG;
    }

    *pixels = &_images[index].pixels.front();
    
    return S_OK;
}

} // Content
} // GDK
