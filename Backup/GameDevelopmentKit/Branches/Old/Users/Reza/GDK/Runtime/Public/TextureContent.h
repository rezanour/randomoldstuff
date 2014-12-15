#pragma once

#include "Platform.h"

namespace GDK
{
    enum class TextureFormat
    {
        R8G8G8A8,
    };

    // TODO: Move to a common location
    struct Point2
    {
        uint32_t x;
        uint32_t y;
    };

    class TextureContent : public RuntimeObject<TextureContent>
    {
    public:
        static std::shared_ptr<TextureContent> FromStream(_Inout_ std::istream& input);

        TextureFormat GetFormat() const;
        uint32_t GetImageCount() const;
        Point2 GetSize(_In_ uint32_t image) const;
        const byte_t* GetPixels(_In_ uint32_t image) const;

    private:
        struct Image
        {
            Point2 Size;
            byte_t Pixels[1];
        };

        TextureContent(_In_ TextureFormat format, _In_ const std::vector<std::shared_ptr<Image>>& images);

        TextureFormat _format;
        std::vector<std::shared_ptr<Image>> _images;
    };
}
