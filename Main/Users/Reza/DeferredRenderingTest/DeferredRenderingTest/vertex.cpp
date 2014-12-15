#include "precomp.h"
#include "vertex.h"

void GetInputElementsForFormat(VertexFormat format, D3D11_INPUT_ELEMENT_DESC* elems, uint32_t* size)
{
    switch (format)
    {
    case VertexFormat::PositionNormal:
        if (*size < 2)
        {
            throw std::exception();
        }

        *size = 2;

        elems[0].AlignedByteOffset = 0;
        elems[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        elems[0].InputSlot = 0;
        elems[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        elems[0].SemanticIndex = 0;
        elems[0].SemanticName = "POSITION";
        elems[1].AlignedByteOffset = 12;
        elems[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        elems[1].InputSlot = 0;
        elems[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        elems[1].SemanticIndex = 0;
        elems[1].SemanticName = "NORMAL";
        break;

    case VertexFormat::PositionNormalTexture:
        if (*size < 3)
        {
            throw std::exception();
        }

        *size = 3;

        elems[0].AlignedByteOffset = 0;
        elems[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        elems[0].InputSlot = 0;
        elems[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        elems[0].SemanticIndex = 0;
        elems[0].SemanticName = "POSITION";
        elems[1].AlignedByteOffset = 12;
        elems[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        elems[1].InputSlot = 0;
        elems[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        elems[1].SemanticIndex = 0;
        elems[1].SemanticName = "NORMAL";
        elems[2].AlignedByteOffset = 24;
        elems[2].Format = DXGI_FORMAT_R32G32_FLOAT;
        elems[2].InputSlot = 0;
        elems[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        elems[2].SemanticIndex = 0;
        elems[2].SemanticName = "TEXCOORD";
        break;

    default:
        assert(false);
    }
}
