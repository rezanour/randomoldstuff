#include <GeometryContent.h>
#include <GDKError.h>
#include <istream>

namespace GDK
{
    const uint32_t GeometryContentVersion = 0x20315247; // "GR1 "

    #pragma pack(push,1)

    struct GeometryContentHeader
    {
        uint32_t Version;
        uint32_t Id;
        uint32_t VertexCount;
        uint32_t IndexCount;
        uint32_t FrameCount;
        uint32_t AnimationCount;
    };

    //
    // Geometry Content File Specification
    // 
    // [GeometryContentHeader] <-- File header
    // [Vertices Data Buffer ] (GeometryContent::Vertex) for each element
    // [Indices Data Buffer  ] (uint32_t) for each element
    // [Animation Data Buffer] (Animation) for each element

    #pragma pack(pop)

    std::shared_ptr<GeometryContent> GeometryContent::Create(_In_ uint32_t numVertices, _Inout_ std::unique_ptr<Vertex[]>& vertices, _In_ uint32_t numIndices, _Inout_ std::unique_ptr<uint32_t[]>& indices, _In_ uint32_t numFrames, _In_ uint32_t numAnimations, _Inout_ std::unique_ptr<Animation[]>& animations)
    {
        return std::shared_ptr<GeometryContent>(GDKNEW GeometryContent(numVertices, vertices, numIndices, indices, numFrames, numAnimations, animations));
    }

    std::shared_ptr<GeometryContent> GeometryContent::Create(_Inout_ std::istream &stream)
    {
        GeometryContentHeader header;

        stream.read(reinterpret_cast<char*>(&header), sizeof(header));
        CHECK_TRUE(header.Version == GeometryContentVersion);

        std::unique_ptr<Vertex[]> vertices(new Vertex[header.VertexCount]);
        std::unique_ptr<uint32_t[]> indices(new uint32_t[header.IndexCount]);
        std::unique_ptr<Animation[]> animations(new Animation[header.AnimationCount]);

        stream.read(reinterpret_cast<char*>(vertices.get()), sizeof(Vertex) * header.VertexCount);
        stream.read(reinterpret_cast<char*>(indices.get()), sizeof(uint32_t) * header.IndexCount);
        stream.read(reinterpret_cast<char*>(animations.get()), sizeof(Animation) * header.AnimationCount);

        return std::shared_ptr<GeometryContent>(GDKNEW GeometryContent(header.VertexCount, vertices, header.IndexCount, indices, header.FrameCount, header.AnimationCount, animations));
    }

    void GeometryContent::Save(_Inout_ std::ostream& stream)
    {
        GeometryContentHeader header;
        header.Version = GeometryContentVersion;
        header.Id = 0;
        header.VertexCount = _numVertices;
        header.IndexCount = _numIndices;
        header.FrameCount = _numFrames;
        header.AnimationCount = _numAnimations;

        stream.write(reinterpret_cast<char*>(&header), sizeof(header));
        stream.write(reinterpret_cast<char*>(_vertices.get()), sizeof(Vertex) * _numVertices);
        stream.write(reinterpret_cast<char*>(_indices.get()), sizeof(uint32_t) * _numIndices);
        stream.write(reinterpret_cast<char*>(_animations.get()), sizeof(Animation) * _numAnimations);
    }

    GeometryContent::GeometryContent(_In_ uint32_t numVertices, _Inout_ std::unique_ptr<Vertex[]>& vertices, _In_ uint32_t numIndices, _Inout_ std::unique_ptr<uint32_t[]>& indices, _In_ uint32_t numFrames, _In_ uint32_t numAnimations, _Inout_ std::unique_ptr<Animation[]>& animations) :
        _numVertices(numVertices),
        _numIndices(numIndices),
        _numFrames(numFrames),
        _numAnimations(numAnimations)
    {
        _vertices.swap(vertices);
        _indices.swap(indices);
        _animations.swap(animations);
    }

    uint32_t GeometryContent::GetNumVertices() const
    {
        return _numVertices;
    }

    uint32_t GeometryContent::GetNumIndices() const
    {
        return _numIndices;
    }

    const GeometryContent::Vertex* GeometryContent::GetVertices() const
    {
        return _vertices.get();
    }

    const uint32_t* GeometryContent::GetIndices() const
    {
        return _indices.get();
    }

    uint32_t GeometryContent::GetNumFrames() const
    {
        return _numFrames;
    }

    uint32_t GeometryContent::GetNumVerticesPerFrame() const
    {
        return _numVertices / _numFrames;
    }

    uint32_t GeometryContent::GetNumIndicesPerFrame() const
    {
        return _numIndices / _numFrames;
    }

    uint32_t GeometryContent::GetNumAnimations() const
    {
        return _numAnimations;
    }

    const GeometryContent::Animation* GeometryContent::GetAnimations() const
    {
        return _animations.get();
    }
}
