#include "stdafx.h"

#pragma once

namespace GDK
{
    const uint32_t GeometryContentVersion = 0x20315247; // "GR1 "

    #pragma pack(push,1)

    struct GeometryContentHeader
    {
        uint32_t Version;
        uint32_t Id;
        uint32_t AttributeCount;
        uint32_t VertexStride;
        uint32_t VertexCount;
        uint32_t IndexCount;
        uint32_t FrameCount;
        uint32_t AnimationCount;
    };

    //
    // Geometry Content File Specification
    // 
    // [GeometryContentHeader] <-- File header
    // [Attribute Data Buffer] (AttributeDesc) for each attribute
    // [Vertices Data Buffer ] (uint8_t*) for each element
    // [Indices Data Buffer  ] (uint32_t) for each element
    // [Animation Data Buffer] (Animation) for each element

    #pragma pack(pop)

    std::shared_ptr<GeometryContent> GeometryContent::Create(_In_ uint32_t numAttributes, _Inout_ std::unique_ptr<AttributeDesc[]>& attributes, _In_ uint32_t vertexStride, _In_ uint32_t numVertices, _Inout_ std::unique_ptr<uint8_t[]>& vertices, _In_ uint32_t numIndices, _Inout_ std::unique_ptr<uint32_t[]>& indices, _In_ uint32_t numFrames, _In_ uint32_t numAnimations, _Inout_ std::unique_ptr<Animation[]>& animations)
    {
        return std::shared_ptr<GeometryContent>(new GeometryContent(numAttributes, attributes, vertexStride, numVertices, vertices, numIndices, indices, numFrames, numAnimations, animations));
    }

    std::shared_ptr<GeometryContent> GeometryContent::Create(_Inout_ std::istream &stream)
    {
        GeometryContentHeader header;

        stream.read(reinterpret_cast<char*>(&header), sizeof(header));

        std::unique_ptr<AttributeDesc[]> attributes(new AttributeDesc[header.AttributeCount]);
        stream.read(reinterpret_cast<char*>(attributes.get()), sizeof(AttributeDesc) * header.AttributeCount);

        std::unique_ptr<uint8_t[]> vertices(new uint8_t[header.VertexStride * header.VertexCount]);
        stream.read(reinterpret_cast<char*>(vertices.get()), header.VertexStride * header.VertexCount);

        std::unique_ptr<uint32_t[]> indices(new uint32_t[header.IndexCount]);
        stream.read(reinterpret_cast<char*>(indices.get()), sizeof(uint32_t) * header.IndexCount);

        std::unique_ptr<Animation[]> animations(new Animation[header.AnimationCount]);
        stream.read(reinterpret_cast<char*>(animations.get()), sizeof(Animation) * header.AnimationCount);

        return std::shared_ptr<GeometryContent>(new GeometryContent(header.AttributeCount, attributes, header.VertexStride, header.VertexCount, vertices, header.IndexCount, indices, header.FrameCount, header.AnimationCount, animations));
    }

    void GeometryContent::Save(_Inout_ std::ostream& stream)
    {
        GeometryContentHeader header;
        header.Version = GeometryContentVersion;
        header.Id = 0;
        header.AttributeCount = _numAttributes;
        header.VertexStride = _vertexStride;
        header.VertexCount = _numVertices;
        header.IndexCount = _numIndices;
        header.FrameCount = _numFrames;
        header.AnimationCount = _numAnimations;

        stream.write(reinterpret_cast<char*>(&header), sizeof(header));
        stream.write(reinterpret_cast<char*>(_attributes.get()), sizeof(AttributeDesc) * _numAttributes);
        stream.write(reinterpret_cast<char*>(_vertices.get()), _vertexStride * _numVertices);
        stream.write(reinterpret_cast<char*>(_indices.get()), sizeof(uint32_t) * _numIndices);
        stream.write(reinterpret_cast<char*>(_animations.get()), sizeof(Animation) * _numAnimations);
    }

    GeometryContent::GeometryContent(_In_ uint32_t numAttributes, _Inout_ std::unique_ptr<AttributeDesc[]>& attributes, _In_ uint32_t vertexStride, _In_ uint32_t numVertices, _Inout_ std::unique_ptr<uint8_t[]>& vertices, _In_ uint32_t numIndices, _Inout_ std::unique_ptr<uint32_t[]>& indices, _In_ uint32_t numFrames, _In_ uint32_t numAnimations, _Inout_ std::unique_ptr<Animation[]>& animations) :
        _numAttributes(numAttributes),
        _numVertices(numVertices),
        _numIndices(numIndices),
        _numFrames(numFrames),
        _numAnimations(numAnimations),
        _vertexStride(vertexStride)
    {
        _attributes.swap(attributes);
        _vertices.swap(vertices);
        _indices.swap(indices);
        _animations.swap(animations);
    }

    uint32_t GeometryContent::GetNumAttributes() const
    {
        return _numAttributes;
    }

    const GeometryContent::AttributeDesc* GeometryContent::GetAttributes() const
    {
        return _attributes.get();
    }

    uint32_t GeometryContent::GetVertexStride() const
    {
        return _vertexStride;
    }

    const uint8_t* GeometryContent::GetAttributeData(_In_ GeometryContent::AttributeType type, _In_ GeometryContent::AttributeName name, _In_ uint8_t semanticIndex) const
    {
        // validate params and find attribute
        for (uint32_t i = 0; i < _numAttributes; ++i)
        {
            if (_attributes.get()[i].semanticName == name && 
                _attributes.get()[i].semanticIndex == semanticIndex &&
                _attributes.get()[i].type == type)
            {
                // match found
                return _vertices.get() + _attributes.get()[i].offset;
            }
        }

        return nullptr;
    }

    uint32_t GeometryContent::GetNumVertices() const
    {
        return _numVertices;
    }

    uint32_t GeometryContent::GetNumIndices() const
    {
        return _numIndices;
    }

    const uint8_t* GeometryContent::GetVertices() const
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
