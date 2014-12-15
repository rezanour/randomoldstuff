#pragma once

enum class VertexFormat
{
    Position2DProj,
    Position2DTexture,
    Position3D,
    Position3DColor,
    PositionNormalTangentTexture,
    Max
};

struct Position2DProjVertex
{
    XMFLOAT2 Position;  // Post-projection space (-1, 1)
};

struct Position2DTextureVertex
{
    XMFLOAT2 Position;  // Normalized screen space (0, 1)
    XMFLOAT2 TexCoord;
};

struct Position3DVertex
{
    XMFLOAT3 Position;
};

struct Position3DColorVertex
{
    XMFLOAT3 Position;
    XMFLOAT4 Color;
};

struct PositionNormalTangentTextureVertex
{
    XMFLOAT3 Position;
    XMFLOAT3 Normal;
    XMFLOAT3 Tangent;
    XMFLOAT2 TexCoord;
};

uint32_t GetVertexSize(_In_ VertexFormat format);

inline PositionNormalTangentTextureVertex LerpVertex(_In_ const PositionNormalTangentTextureVertex& v0, _In_ const PositionNormalTangentTextureVertex& v1, float t)
{
    PositionNormalTangentTextureVertex v;
    XMStoreFloat3(&v.Position, XMVectorLerp(XMLoadFloat3(&v0.Position), XMLoadFloat3(&v1.Position), t));
    XMStoreFloat3(&v.Normal, XMVectorLerp(XMLoadFloat3(&v0.Normal), XMLoadFloat3(&v1.Normal), t));
    XMStoreFloat3(&v.Tangent, XMVectorLerp(XMLoadFloat3(&v0.Tangent), XMLoadFloat3(&v1.Tangent), t));
    XMStoreFloat2(&v.TexCoord, XMVectorLerp(XMLoadFloat2(&v0.TexCoord), XMLoadFloat2(&v1.TexCoord), t));
    return v;
}

void ScaleGeometryToHeight(_In_ uint32_t numVertices, _In_ VertexFormat format, _Inout_ void* vertices, _In_ float targetHeight);
void GenerateTangents(_In_ uint32_t numVertices, _In_ VertexFormat format, _Inout_ void* vertices, _In_ uint32_t numIndices, _In_ const uint32_t* indices);

struct __declspec(novtable) IGeometry
{
    virtual const AABB& GetAABB() const = 0;
};
