#pragma once

#include <Platform.h>
#include <RuntimeObject.h>
#include <MathTypes.h>

namespace GDK
{
    class GeometryContent : public RuntimeObject<GeometryContent>
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
            AttributeDesc(_In_ byte_t offset, _In_ AttributeType type, _In_ AttributeName name, _In_ byte_t semanticIndex) : offset(offset), type(type), semanticName(name), semanticIndex(semanticIndex) {}

            byte_t offset;
            AttributeType type;
            AttributeName semanticName;
            byte_t semanticIndex;
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
        static std::shared_ptr<GeometryContent> Create(_In_ uint32_t numAttributes, _Inout_ std::unique_ptr<AttributeDesc[]>& attributes, _In_ uint32_t vertexStride, _In_ uint32_t numVertices, _Inout_ std::unique_ptr<byte_t[]>& vertices, _In_ uint32_t numIndices, _Inout_ std::unique_ptr<uint32_t[]>& indices, _In_ uint32_t numFrames, _In_ uint32_t numAnimations, _Inout_ std::unique_ptr<Animation[]>& animations);

        uint32_t GetNumAttributes() const;
        const AttributeDesc* GetAttributes() const;

        // Raw attribute data is somewhat interesting, but more interesting is to pull pointers to attributes you care about
        const byte_t* GetAttributeData(_In_ AttributeType type, _In_ AttributeName name, _In_ byte_t semanticIndex) const;

        uint32_t GetVertexStride() const;

        uint32_t GetNumVertices() const;
        const byte_t* GetVertices() const;

        uint32_t GetNumIndices() const;
        const uint32_t* GetIndices() const;

        uint32_t GetNumFrames() const;
        uint32_t GetNumIndicesPerFrame() const;

        uint32_t GetNumAnimations() const;
        const Animation* GetAnimations() const;

        void Save(_Inout_ std::ostream& stream);

    private:
        GeometryContent(_In_ uint32_t numAttributes, _Inout_ std::unique_ptr<AttributeDesc[]>& attributes, _In_ uint32_t vertexStride, _In_ uint32_t numVertices, _Inout_ std::unique_ptr<byte_t[]>& vertices, _In_ uint32_t numIndices, _Inout_ std::unique_ptr<uint32_t[]>& indices, _In_ uint32_t numFrames, _In_ uint32_t numAnimations, _Inout_ std::unique_ptr<Animation[]>& animations);

        uint32_t _numAttributes;
        uint32_t _numVertices;
        uint32_t _numIndices;
        uint32_t _numFrames;
        uint32_t _numAnimations;
        uint32_t _vertexStride;
        std::unique_ptr<AttributeDesc[]> _attributes;
        std::unique_ptr<byte_t[]> _vertices;
        std::unique_ptr<uint32_t[]> _indices;
        std::unique_ptr<Animation[]> _animations;
    };
}
