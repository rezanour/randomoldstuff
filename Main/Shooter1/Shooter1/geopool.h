#pragma once

class Graphics;

enum class VertexFormat
{
    Invalid = 0,
    StaticGeometry,
};

struct StaticGeometryVertex
{
    XMFLOAT3 Position;
    XMFLOAT3 Normal;
    XMFLOAT2 TexCoord;
    uint32_t MaterialId;
};

uint32_t GetVertexStride(_In_ VertexFormat format);
std::vector<D3D11_INPUT_ELEMENT_DESC> GetVertexElements(_In_ VertexFormat format);

class GeoPool : public TrackedObject<MemoryTag::GeoPool>
{
public:
    uint32_t GetId() const { return _id; }
    VertexFormat GetFormat() const { return _format; }

    ID3D11Buffer* GetVertices() const { return _vertexBuffer.Get(); }
    ID3D11Buffer** GetVerticesAddress() { return _vertexBuffer.GetAddressOf(); }

    ID3D11Buffer* GetIndices() const { return _indexBuffer.Get(); }
    ID3D11Buffer** GetIndicesAddress() { return _indexBuffer.GetAddressOf(); }

    uint32_t GetStride() const { return GetVertexStride(_format); }

    bool HasAvailableSpace(_In_ VertexFormat format, _In_ uint32_t numVertices, _In_ uint32_t numIndices) const;
    void ReserveRange(_In_ uint32_t numVertices, _In_ uint32_t numIndices, _Out_ uint32_t* verticesIndex, _Out_ uint32_t* indicesIndex);

private:
    friend class Graphics;
    GeoPool(_In_ ID3D11Device* device, _In_ VertexFormat format, _In_ uint32_t maxVertices, _In_ uint32_t maxIndices);

private:
    ComPtr<ID3D11Buffer> _vertexBuffer;
    ComPtr<ID3D11Buffer> _indexBuffer;

    static uint32_t s_nextId;
    uint32_t _id;
    VertexFormat _format;
    uint32_t _maxVertices;
    uint32_t _numVertices;
    uint32_t _maxIndices;
    uint32_t _numIndices;
};
