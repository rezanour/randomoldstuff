#pragma once

#include "Platform.h"
#include "RuntimeObject.h"

namespace GDK
{
    #pragma pack(push,1)

    struct Float2
    {
        float x;
        float y;
    };

    struct Float3
    {
        float x;
        float y;
        float z;
    };

    struct Float4
    {
        float x;
        float y;
        float z;
        float w;
    };

    #pragma pack(pop)

    class GeometryContent : public RuntimeObject<GeometryContent>
    {
    public:
#pragma pack(push, 1)

        struct Vertex
        {
            Float3 Position;
            Float3 Normal;
            //Float3 Tangent;
            //Float3 Bitangent;
            //Float4 Color;
            Float2 TextureCoord0;
            //Float2 TextureCoord1;
            //Float2 TextureCoord2;
            //Float2 TextureCoord3;
            //Float4 BoneIndices;
            //Float4 BoneWeights;
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
        static std::shared_ptr<GeometryContent> Create(_In_ uint32_t numVertices, _Inout_ std::unique_ptr<Vertex[]>& vertices, _In_ uint32_t numIndices, _Inout_ std::unique_ptr<uint32_t[]>& indices, _In_ uint32_t numFrames, _In_ uint32_t numAnimations, _Inout_ std::unique_ptr<Animation[]>& animations);

        uint32_t GetNumVertices() const;
        uint32_t GetNumIndices() const;
        const Vertex* GetVertices() const;
        const uint32_t* GetIndices() const;

        uint32_t GetNumFrames() const;
        uint32_t GetNumVerticesPerFrame() const;
        uint32_t GetNumIndicesPerFrame() const;

        uint32_t GetNumAnimations() const;
        const Animation* GetAnimations() const;

        void Save(_Inout_ std::ostream& stream);

    private:
        GeometryContent(_In_ uint32_t numVertices, _Inout_ std::unique_ptr<Vertex[]>& vertices, _In_ uint32_t numIndices, _Inout_ std::unique_ptr<uint32_t[]>& indices, _In_ uint32_t numFrames, _In_ uint32_t numAnimations, _Inout_ std::unique_ptr<Animation[]>& animations);

        uint32_t _numVertices;
        uint32_t _numIndices;
        uint32_t _numFrames;
        uint32_t _numAnimations;
        std::unique_ptr<Vertex[]> _vertices;
        std::unique_ptr<uint32_t[]> _indices;
        std::unique_ptr<Animation[]> _animations;
    };
}
