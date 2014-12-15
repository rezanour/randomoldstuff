#include <Precomp.h>
#include "GeometryImpl.h"

uint32_t GetVertexSize(_In_ VertexFormat format)
{
    switch (format)
    {
    case VertexFormat::Position2DProj:
        return sizeof(Position2DProjVertex);

    case VertexFormat::Position2DTexture:
        return sizeof(Position2DTextureVertex);

    case VertexFormat::Position3D:
        return sizeof(Position3DVertex);

    case VertexFormat::Position3DColor:
        return sizeof(Position3DColorVertex);

    case VertexFormat::PositionNormalTangentTexture:
        return sizeof(PositionNormalTangentTextureVertex);

    default:
        throw std::exception();
    }
}

_Use_decl_annotations_
void ScaleGeometryToHeight(uint32_t numVertices, VertexFormat format, void* vertices, float targetHeight)
{
    float minY = FLT_MAX;
    float maxY = -FLT_MAX;

    if (format != VertexFormat::PositionNormalTangentTexture)
    {
        throw std::exception();
    }

    uint32_t vertexSize = GetVertexSize(format);

    // Compute initial height
    for (uint32_t i = 0; i < numVertices; ++i)
    {
        uint8_t* v = static_cast<uint8_t*>(vertices) + i * vertexSize;

        float y = 0.0f;
        switch (format)
        {
        case VertexFormat::PositionNormalTangentTexture:
            y = reinterpret_cast<PositionNormalTangentTextureVertex*>(v)->Position.y;
            break;

        default:
            throw std::exception();
        }

        if (y > maxY) maxY = y;
        if (y < minY) minY = y;
    }

    // compute scale
    float scale = targetHeight / (maxY - minY);

    // Update vertices
    for (uint32_t i = 0; i < numVertices; ++i)
    {
        uint8_t* v = static_cast<uint8_t*>(vertices)+i * vertexSize;

        switch (format)
        {
        case VertexFormat::PositionNormalTangentTexture:
            {
                auto p = reinterpret_cast<PositionNormalTangentTextureVertex*>(v);
                p->Position.x *= scale;
                p->Position.y *= scale;
                p->Position.z *= scale;
            }
            break;

        default:
            throw std::exception();
        }
    }
}

_Use_decl_annotations_
void GenerateTangents(uint32_t numVertices, VertexFormat format, void* vertices, uint32_t numIndices, const uint32_t* indices)
{
    // Pretty sure this is the same method I used in the original GDK, but this time I found
    // a nice write up of the process, so I'll cite it here: http://www.terathon.com/code/tangent.html

    if (format != VertexFormat::PositionNormalTangentTexture)
    {
        throw std::exception();
    }

    PositionNormalTangentTextureVertex* verts = static_cast<PositionNormalTangentTextureVertex*>(vertices);

    // zero out all tangents
    for (uint32_t i = 0; i < numVertices; ++i)
    {
        verts[i].Tangent = XMFLOAT3(0, 0, 0);
    }

    // accumulate tangents per triangle
    for (uint32_t i = 0; i < numIndices; i += 3)
    {
        PositionNormalTangentTextureVertex& A = verts[indices[i]];
        PositionNormalTangentTextureVertex& B = verts[indices[i + 1]];
        PositionNormalTangentTextureVertex& C = verts[indices[i + 2]];

        float x1 = B.Position.x - A.Position.x;
        float x2 = C.Position.x - A.Position.x;
        float y1 = B.Position.y - A.Position.y;
        float y2 = C.Position.y - A.Position.y;
        float z1 = B.Position.z - A.Position.z;
        float z2 = C.Position.z - A.Position.z;

        float s1 = B.TexCoord.x - A.TexCoord.x;
        float s2 = C.TexCoord.x - A.TexCoord.x;
        float t1 = B.TexCoord.y - A.TexCoord.y;
        float t2 = C.TexCoord.y - A.TexCoord.y;

        float r = 1.0f / (s1 * t2 - s2 * t1);
        XMFLOAT3 tangent((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);

        XMStoreFloat3(&A.Tangent, XMVectorAdd(XMLoadFloat3(&A.Tangent), XMLoadFloat3(&tangent)));
        XMStoreFloat3(&B.Tangent, XMVectorAdd(XMLoadFloat3(&B.Tangent), XMLoadFloat3(&tangent)));
        XMStoreFloat3(&C.Tangent, XMVectorAdd(XMLoadFloat3(&C.Tangent), XMLoadFloat3(&tangent)));
    }

    // normalize
    for (uint32_t i = 0; i < numVertices; ++i)
    {
        PositionNormalTangentTextureVertex& v = verts[i];
        XMVECTOR t = XMLoadFloat3(&v.Tangent);
        XMVECTOR n = XMLoadFloat3(&v.Normal);
        XMStoreFloat3(&v.Tangent, XMVector3Normalize(XMVectorSubtract(t, XMVectorScale(n, XMVectorGetX(XMVector3Dot(n, t))))));
    }
}

_Use_decl_annotations_
std::shared_ptr<Geometry> Geometry::Create(const ComPtr<ID3D11Device>& device, uint32_t numVertices, VertexFormat format, const void* vertices, uint32_t numIndices, const uint32_t* indices)
{
    return std::shared_ptr<Geometry>(new Geometry(device, numVertices, format, vertices, numIndices, indices));
}

_Use_decl_annotations_
Geometry::Geometry(const ComPtr<ID3D11Device>& device, uint32_t numVertices, VertexFormat format, const void* vertices, uint32_t numIndices, const uint32_t* indices) :
    _numVertices(numVertices), _numIndices(numIndices), _vertexFormat(format), _vertexStride(GetVertexSize(format))
{
    D3D11_BUFFER_DESC bd = {};
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.StructureByteStride = _vertexStride;
    bd.ByteWidth = numVertices * bd.StructureByteStride;
    bd.Usage = D3D11_USAGE_DEFAULT;

    D3D11_SUBRESOURCE_DATA init = {};
    init.pSysMem = vertices;
    init.SysMemPitch = bd.ByteWidth;
    init.SysMemSlicePitch = init.SysMemPitch;

    HRESULT hr = device->CreateBuffer(&bd, &init, &_vertices);
    if (FAILED(hr))
    {
        throw std::exception();
    }

    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.StructureByteStride = sizeof(uint32_t);
    bd.ByteWidth = numIndices * bd.StructureByteStride;

    init.pSysMem = indices;
    init.SysMemPitch = bd.ByteWidth;
    init.SysMemSlicePitch = init.SysMemPitch;

    hr = device->CreateBuffer(&bd, &init, &_indices);
    if (FAILED(hr))
    {
        throw std::exception();
    }
}

const AABB& Geometry::GetAABB() const
{
    return _aabb;
}

const ComPtr<ID3D11Buffer>& Geometry::GetVertices() const
{
    return _vertices;
}

const ComPtr<ID3D11Buffer>& Geometry::GetIndices() const
{
    return _indices;
}

VertexFormat Geometry::GetVertexFormat() const
{
    return _vertexFormat;
}

uint32_t Geometry::GetVertexStride() const
{
    return _vertexStride;
}

uint32_t Geometry::GetNumVertices() const
{
    return _numVertices;
}

uint32_t Geometry::GetNumIndices() const
{
    return _numIndices;
}
