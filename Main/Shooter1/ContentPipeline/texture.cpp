#include "precomp.h"
#include "texture.h"

_Use_decl_annotations_
std::unique_ptr<TextureContent> TextureContent::Load(const wchar_t* filename)
{
    ScratchImage image;
    if (FAILED(LoadFromWICFile(filename, WIC_FLAGS_ALL_FRAMES | WIC_FLAGS_FILTER_FANT, nullptr, image)))
    {
        if (FAILED(LoadFromTGAFile(filename, nullptr, image)))
        {
            if (FAILED(LoadFromDDSFile(filename, DDS_FLAGS_NONE, nullptr, image)))
            {
                Error("Failed to load file: %S", filename);
            }
        }
    }

    return std::unique_ptr<TextureContent>(new TextureContent(std::move(image)));
}

TextureContent::TextureContent(ScratchImage&& image) :
    _image(std::move(image))
{
}

void TextureContent::GenerateMips()
{
    if (_image.GetMetadata().mipLevels > 1)
    {
        // Already has mips
        return;
    }

    ScratchImage newImage;
    CHECKHR(GenerateMipMaps(_image.GetImages(), _image.GetImageCount(), _image.GetMetadata(), TEX_FILTER_CUBIC, 0, newImage));

    _image = std::move(newImage);
}

void TextureContent::ResizeImage(uint32_t width, uint32_t height)
{
    if (_image.GetMetadata().width == width &&
        _image.GetMetadata().height == height)
    {
        // No resize required
        return;
    }

    ScratchImage newImage;
    CHECKHR(Resize(_image.GetImages(), _image.GetImageCount(), _image.GetMetadata(), (size_t)width, (size_t)height, TEX_FILTER_CUBIC, newImage));

    _image = std::move(newImage);
}

// Save to the current location in the given file
void TextureContent::Save(Output& output) const
{
    Blob blob;
    CHECKHR(SaveToDDSMemory(_image.GetImages(), _image.GetImageCount(), _image.GetMetadata(), DDS_FLAGS_FORCE_DX10_EXT, blob));
    output.Write(blob.GetBufferPointer(), (uint32_t)blob.GetBufferSize());
}
