#include "TextureContent.h"

namespace GDK
{
#pragma pack(push, 1)
    struct TextureHeader
    {
        uint32_t    Version;
        char        Name[64];
        uint64_t    Id;
        uint64_t    Width;
        uint64_t    Height;     // Should be 1 for 1D textures
        uint64_t    Depth;      // Should be 1 for 1D or 2D textures
        uint64_t    ArraySize;  // For cubemap, this is a multiple of 6
        uint64_t    MipLevels;
        uint32_t    MiscFlags;
        uint32_t    Format;
        uint32_t    Dimension;
    };

    struct ImageResource
    {
        uint64_t    Width;
        uint64_t    Height;
        uint32_t    Format;
        uint64_t    RowPitch;
        uint64_t    SlicePitch;
        uint32_t    PixeslSizeInBytes;
        uint64_t    Reserved; // until we fix existing images
    };

#pragma pack(pop)

    const uint32_t TextureResourceVersion = 0x20315254; // "TR1 "

    std::shared_ptr<TextureContent> TextureContent::FromStream(_Inout_ std::istream& input)
    {
        TextureHeader header;
        input.read(reinterpret_cast<char*>(&header), sizeof(header));

        if (header.Version != TextureResourceVersion)
        {
            throw std::invalid_argument("input");
        }

        // Read images
        std::vector<std::shared_ptr<Image>> images;
        for (size_t i = 0; i < header.ArraySize; ++i)
        {
            ImageResource imageResource;
            input.read(reinterpret_cast<char*>(&imageResource), sizeof(imageResource));

            std::shared_ptr<Image> image(reinterpret_cast<Image*>(new byte_t[sizeof(Image) + imageResource.PixeslSizeInBytes]));
            image->Size.x = static_cast<uint32_t>(imageResource.Width);
            image->Size.y = static_cast<uint32_t>(imageResource.Height);
            input.read(reinterpret_cast<char*>(image->Pixels), imageResource.PixeslSizeInBytes);

            images.push_back(image);
        }

        return std::shared_ptr<TextureContent>(new (__FILEW__, __LINE__) TextureContent(TextureFormat::R8G8G8A8, images));
    }

    TextureContent::TextureContent(_In_ TextureFormat format, _In_ const std::vector<std::shared_ptr<Image>>& images) :
        _format(format),
        _images(images)
    {
    }

    TextureFormat TextureContent::GetFormat() const
    {
        return _format;
    }

    uint32_t TextureContent::GetImageCount() const
    {
        return static_cast<uint32_t>(_images.size());
    }

    Point2 TextureContent::GetSize(_In_ uint32_t image) const
    {
        return _images[image]->Size;
    }

    const byte_t* TextureContent::GetPixels(_In_ uint32_t image) const
    {
        return _images[image]->Pixels;
    }
}
