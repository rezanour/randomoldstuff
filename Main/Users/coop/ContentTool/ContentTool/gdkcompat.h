#pragma once

namespace GDK
{
    class GeometryContent
    {
    public:
#pragma pack(push, 1)

        enum class AttributeName
        {
            None,
            Position,
            Normal,
            TexCoord
        };

        enum class AttributeType
        {
            Null,
            Float2,
            Float3
        };

        struct AttributeDesc
        {
            AttributeDesc() : offset(0xFF), type(AttributeType::Null), semanticName(AttributeName::None), semanticIndex(0) {}
            AttributeDesc(_In_ uint8_t offset, _In_ AttributeType type, _In_ AttributeName name, _In_ uint8_t semanticIndex) : offset(offset), type(type), semanticName(name), semanticIndex(semanticIndex) {}

            uint8_t offset;
            AttributeType type;
            AttributeName semanticName;
            uint8_t semanticIndex;
        };

        static const uint32_t MaxAnimationNameLength = 32;
        struct Animation
        {
            char Name[MaxAnimationNameLength];
            uint32_t StartFrame;
            uint32_t EndFrame;
        };

#pragma pack(pop)

        static std::shared_ptr<GeometryContent> Create(_Inout_ std::istream &stream);
        static std::shared_ptr<GeometryContent> Create(_In_ uint32_t numAttributes, _Inout_ std::unique_ptr<AttributeDesc[]>& attributes, _In_ uint32_t vertexStride, _In_ uint32_t numVertices, _Inout_ std::unique_ptr<uint8_t[]>& vertices, _In_ uint32_t numIndices, _Inout_ std::unique_ptr<uint32_t[]>& indices, _In_ uint32_t numFrames, _In_ uint32_t numAnimations, _Inout_ std::unique_ptr<Animation[]>& animations);

        uint32_t GetNumAttributes() const;
        const AttributeDesc* GetAttributes() const;

        // Raw attribute data is somewhat interesting, but more interesting is to pull pointers to attributes you care about
        const uint8_t* GetAttributeData(_In_ AttributeType type, _In_ AttributeName name, _In_ uint8_t semanticIndex) const;

        uint32_t GetVertexStride() const;

        uint32_t GetNumVertices() const;
        const uint8_t* GetVertices() const;

        uint32_t GetNumIndices() const;
        const uint32_t* GetIndices() const;

        uint32_t GetNumFrames() const;
        uint32_t GetNumIndicesPerFrame() const;

        uint32_t GetNumAnimations() const;
        const Animation* GetAnimations() const;

        void Save(_Inout_ std::ostream& stream);

    private:
        GeometryContent(_In_ uint32_t numAttributes, _Inout_ std::unique_ptr<AttributeDesc[]>& attributes, _In_ uint32_t vertexStride, _In_ uint32_t numVertices, _Inout_ std::unique_ptr<uint8_t[]>& vertices, _In_ uint32_t numIndices, _Inout_ std::unique_ptr<uint32_t[]>& indices, _In_ uint32_t numFrames, _In_ uint32_t numAnimations, _Inout_ std::unique_ptr<Animation[]>& animations);

        uint32_t _numAttributes;
        uint32_t _numVertices;
        uint32_t _numIndices;
        uint32_t _numFrames;
        uint32_t _numAnimations;
        uint32_t _vertexStride;
        std::unique_ptr<AttributeDesc[]> _attributes;
        std::unique_ptr<uint8_t[]> _vertices;
        std::unique_ptr<uint32_t[]> _indices;
        std::unique_ptr<Animation[]> _animations;
    };

#pragma pack(push, 1)   // these structures can be used in file I/O, and so must be packed to 1 byte packing

    struct Float2
    {
        Float2() {}
        Float2(float x, float y) : x(x), y(y) {}

        float x;
        float y;
    };

    struct Float3
    {
        Float3() {}
        Float3(float x, float y, float z) : x(x), y(y), z(z) {}

        float x;
        float y;
        float z;
    };

    template <typename T>
    struct Rectangle
    {
        Rectangle() : 
            left(static_cast<T>(0)), 
            top(static_cast<T>(0)), 
            right(static_cast<T>(0)), 
            bottom(static_cast<T>(0))
        {}

        Rectangle(_In_ T left, _In_ T top, _In_ T width, _In_ T height) 
            : left(left), top(top), right(left + width), bottom(top + height) {}

        T Width() const { return right - left; }
        T Height() const { return bottom - top; }

        T left, top, right, bottom;
    };

    typedef Rectangle<float> RectangleF;

#pragma pack(pop)

        enum class TextureFormat
    {
        R8G8B8A8,
    };

    class TextureContent
    {
    public:
        static std::shared_ptr<TextureContent> Create(_In_ uint32_t width, _In_ uint32_t height, _In_ TextureFormat format, _Inout_ std::unique_ptr<uint8_t[]>& pixels, _In_ uint32_t numFrames, _Inout_ std::unique_ptr<RectangleF[]>& frames);
        static std::shared_ptr<TextureContent> Create(_Inout_ std::istream &stream);

        static uint32_t BytesPerPixel(_In_ TextureFormat format);

        TextureFormat GetFormat() const;

        uint32_t GetWidth() const;
        uint32_t GetHeight() const;
        const uint8_t* GetPixels() const;

        uint32_t GetNumFrames() const;
        const RectangleF* GetFrames() const;

        void Save(_Inout_ std::ostream& stream);

    private:
        TextureContent(_In_ uint32_t width, _In_ uint32_t height, _In_ TextureFormat format, _Inout_ std::unique_ptr<uint8_t[]>& pixels, _In_ uint32_t numFrames, _Inout_ std::unique_ptr<RectangleF[]>& frames);

        TextureFormat _format;
        uint32_t _width;
        uint32_t _height;
        uint32_t _numFrames;
        std::unique_ptr<uint8_t[]> _pixels;
        std::unique_ptr<RectangleF[]> _frames;
    };

}
