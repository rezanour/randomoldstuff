#include <TextureContent.h>
#include <GDKError.h>
#include <istream>

namespace GDK
{
    const uint32_t TextureContentVersion = 0x20315254; // "TR1 "

    #pragma pack(push,1)

    struct TextureContentHeader
    {
        uint32_t        Version;
        uint32_t        Id;
        TextureFormat   Format;
        uint32_t        Width;
        uint32_t        Height;
        uint32_t        NumFrames;
        uint32_t        ImageSize;
    };

    //
    // Texture Content File Specification
    // 
    // [TextureContentHeader]   <-- File header, NumFrames specifies how large the frames buffer is after the data buffer
    // [Texture Data Buffer ]   <-- ImageSize from header
    // [Frames Buffer ]         <-- NumFrames * sizeof(RectangleF)

    #pragma pack(pop)

    std::shared_ptr<TextureContent> TextureContent::Create(_In_ uint32_t width, _In_ uint32_t height, _In_ TextureFormat format, _Inout_ std::unique_ptr<byte_t[]>& pixels, _In_ uint32_t numFrames, _Inout_ std::unique_ptr<RectangleF[]>& frames)
    {
        return std::shared_ptr<TextureContent>(GDKNEW TextureContent(width, height, format, pixels, numFrames, frames));
    }

    std::shared_ptr<TextureContent> TextureContent::Create(_Inout_ std::istream &stream)
    {
        TextureContentHeader header;
        stream.read(reinterpret_cast<char*>(&header), sizeof(header));
        CHECK_TRUE(header.Version == TextureContentVersion);

        uint32_t frameDataSize = header.NumFrames * sizeof(RectangleF);

        std::unique_ptr<byte_t[]> pixels(new byte_t[header.ImageSize]);
        std::unique_ptr<RectangleF[]> frames(new RectangleF[header.NumFrames]);

        stream.read(reinterpret_cast<char*>(pixels.get()), header.ImageSize);
        stream.read(reinterpret_cast<char*>(frames.get()), frameDataSize);

        return TextureContent::Create(header.Width, header.Height, header.Format, pixels, header.NumFrames, frames);
    }

    void TextureContent::Save(_Inout_ std::ostream& stream)
    {
        TextureContentHeader header;
        header.Version      = TextureContentVersion;
        header.Id           = 0; // TODO: Generate a unique id for every texture saved
        header.Width        = _width;
        header.Height       = _height;
        header.Format       = _format;
        header.NumFrames    = _numFrames;
        header.ImageSize    = _width * _height * BytesPerPixel(_format);

        stream.write(reinterpret_cast<char*>(&header), sizeof(header));
        stream.write(reinterpret_cast<char*>(_pixels.get()), header.ImageSize);
        stream.write(reinterpret_cast<char*>(_frames.get()), _numFrames * sizeof(RectangleF));
    }

    TextureContent::TextureContent(_In_ uint32_t width, _In_ uint32_t height, _In_ TextureFormat format, _Inout_ std::unique_ptr<byte_t[]>& pixels, _In_ uint32_t numFrames, _Inout_ std::unique_ptr<RectangleF[]>& frames) :
        _width(width),
        _height(height),
        _format(format),
        _numFrames(numFrames)
    {
        _pixels.swap(pixels);
        _frames.swap(frames);
    }

    uint32_t TextureContent::BytesPerPixel(_In_ TextureFormat format)
    {
        switch (format)
        {
        case TextureFormat::R8G8B8A8:
            return 4;

        default:
            throw std::invalid_argument("Unsupported TextureFormat");
        }
    }

    TextureFormat TextureContent::GetFormat() const
    {
        return _format;
    }

    uint32_t TextureContent::GetWidth() const
    {
        return _width;
    }

    uint32_t TextureContent::GetHeight() const
    {
        return _height;
    }

    const byte_t* TextureContent::GetPixels() const
    {
        return _pixels.get();
    }

    uint32_t TextureContent::GetNumFrames() const
    {
        return _numFrames;
    }

    const RectangleF* TextureContent::GetFrames() const
    {
        return _frames.get();
    }
}
