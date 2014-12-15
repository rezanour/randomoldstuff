#pragma once

enum class VertexFormat
{
    PositionNormal,
    PositionNormalTexture,
};

void GetInputElementsForFormat(VertexFormat format, D3D11_INPUT_ELEMENT_DESC* elems, uint32_t* size);

struct VertexPositionNormal
{
    XMFLOAT3 Position;
    XMFLOAT3 Normal;
};

struct VertexPositionNormalTexture
{
    XMFLOAT3 Position;
    XMFLOAT3 Normal;
    XMFLOAT2 TexCoord;
};
