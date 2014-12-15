#pragma once

namespace VRES
{
    enum class VertexFormat
    {
        StaticMesh = 0,
    };

    struct VertexStaticMesh
    {
        XMFLOAT3 Position;
        XMFLOAT3 Normal;
        XMFLOAT3 Tangent;
        XMFLOAT2 TexCoord;
    };

    void GetVertexElements(VertexFormat format, _Out_ const D3D11_INPUT_ELEMENT_DESC** elements, _Out_ uint32_t* numElements);
}
