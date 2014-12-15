#pragma once

#include "Platform.h"
#include "RuntimeObject.h"

namespace GDK
{
    enum class TextureFormat
    {
        R8G8B8A8,
    };

    class TextureContent : public RuntimeObject<TextureContent>
    {
    public:
        static std::shared_ptr<TextureContent> Create(_In_ uint32_t width, _In_ uint32_t height, _In_ TextureFormat format, _Inout_ std::unique_ptr<byte_t[]>& pixels, _In_ uint32_t numFrames, _Inout_ std::unique_ptr<RectangleF[]>& frames);
        static std::shared_ptr<TextureContent> Create(_Inout_ std::istream &stream);

        static uint32_t BytesPerPixel(_In_ TextureFormat format);

        TextureFormat GetFormat() const;

        uint32_t GetWidth() const;
        uint32_t GetHeight() const;
        const byte_t* GetPixels() const;

        uint32_t GetNumFrames() const;
        const RectangleF* GetFrames() const;

        void Save(_Inout_ std::ostream& stream);

    private:
        TextureContent(_In_ uint32_t width, _In_ uint32_t height, _In_ TextureFormat format, _Inout_ std::unique_ptr<byte_t[]>& pixels, _In_ uint32_t numFrames, _Inout_ std::unique_ptr<RectangleF[]>& frames);

        TextureFormat _format;
        uint32_t _width;
        uint32_t _height;
        uint32_t _numFrames;
        std::unique_ptr<byte_t[]> _pixels;
        std::unique_ptr<RectangleF[]> _frames;
    };
}
