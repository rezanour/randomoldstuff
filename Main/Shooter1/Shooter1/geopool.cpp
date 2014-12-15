#include "precomp.h"
#include "geopool.h"

uint32_t GeoPool::s_nextId = 0;

_Use_decl_annotations_
uint32_t GetVertexStride(VertexFormat format)
{
    switch (format)
    {
    case VertexFormat::Invalid:
        return 0;

    case VertexFormat::StaticGeometry:
        return sizeof(StaticGeometryVertex);

    default:
        assert(false);
        return 0;
    }
}

_Use_decl_annotations_
std::vector<D3D11_INPUT_ELEMENT_DESC> GetVertexElements(VertexFormat format)
{
    std::vector<D3D11_INPUT_ELEMENT_DESC> elems;
    D3D11_INPUT_ELEMENT_DESC elem = {};

    switch (format)
    {
    case VertexFormat::Invalid:
        assert(false);
        break;

    case VertexFormat::StaticGeometry:
        elem.Format = DXGI_FORMAT_R32G32B32_FLOAT;
        elem.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        elem.SemanticName = "POSITION";
        elems.push_back(elem);

        elem.AlignedByteOffset = sizeof(XMFLOAT3);
        elem.SemanticName = "NORMAL";
        elems.push_back(elem);

        elem.AlignedByteOffset = 2 * sizeof(XMFLOAT3);
        elem.Format = DXGI_FORMAT_R32G32_FLOAT;
        elem.SemanticName = "TEXCOORD";
        elems.push_back(elem);
        break;
    }

    return elems;
}

_Use_decl_annotations_
GeoPool::GeoPool(ID3D11Device* device, VertexFormat format, uint32_t maxVertices, uint32_t maxIndices) :
    _id(s_nextId++), _format(format), _maxVertices(maxVertices), _maxIndices(maxIndices), _numVertices(0), _numIndices(0)
{
    assert(device != nullptr);

    D3D11_BUFFER_DESC bd = {};
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.StructureByteStride = GetVertexStride(format);
    bd.ByteWidth = bd.StructureByteStride * _maxVertices;
    bd.Usage = D3D11_USAGE_DEFAULT;

    CHECKHR(device->CreateBuffer(&bd, nullptr, &_vertexBuffer));

    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.StructureByteStride = sizeof(uint32_t);
    bd.ByteWidth = bd.StructureByteStride * _maxIndices;

    CHECKHR(device->CreateBuffer(&bd, nullptr, &_indexBuffer));
}

_Use_decl_annotations_
bool GeoPool::HasAvailableSpace(VertexFormat format, uint32_t numVertices, uint32_t numIndices) const
{
    return (format == _format &&
            (_numVertices + numVertices <= _maxVertices) &&
            (_numIndices + numIndices <= _maxIndices));
}

_Use_decl_annotations_
void GeoPool::ReserveRange(uint32_t numVertices, uint32_t numIndices, uint32_t* verticesIndex, uint32_t* indicesIndex)
{
    *verticesIndex = static_cast<uint32_t>(-1);
    *indicesIndex = static_cast<uint32_t>(-1);

    if ((_numVertices + numVertices <= _maxVertices) && 
        (_numIndices + numIndices <= _maxIndices))
    {
        *verticesIndex = _numVertices;
        *indicesIndex = _numIndices;
        _numVertices += numVertices;
        _numIndices += numIndices;
    }
}
