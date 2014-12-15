#pragma once

#include <Geometry.h>

class Geometry : public BaseObject<Geometry>, public IGeometry
{
public:
    static std::shared_ptr<Geometry> Create(_In_ const ComPtr<ID3D11Device>& device, _In_ uint32_t numVertices, _In_ VertexFormat format, _In_ const void* vertices, _In_ uint32_t numIndices, _In_ const uint32_t* indices);

    //
    // IGeometry
    //

    const AABB& GetAABB() const override;

    //
    // Internal
    //

    const ComPtr<ID3D11Buffer>& GetVertices() const;
    const ComPtr<ID3D11Buffer>& GetIndices() const;

    VertexFormat GetVertexFormat() const;
    uint32_t GetVertexStride() const;
    uint32_t GetNumVertices() const;
    uint32_t GetNumIndices() const;

private:
    Geometry(_In_ const ComPtr<ID3D11Device>& device, _In_ uint32_t numVertices, _In_ VertexFormat format, _In_ const void* vertices, _In_ uint32_t numIndices, _In_ const uint32_t* indices);

private:
    ComPtr<ID3D11Buffer> _vertices;
    ComPtr<ID3D11Buffer> _indices;

    VertexFormat _vertexFormat;
    uint32_t _vertexStride;
    uint32_t _numVertices;
    uint32_t _numIndices;

    AABB _aabb;
};
