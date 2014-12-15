#include "Precomp.h"
#include "VRES.h"

using namespace VRES;

static D3D11_INPUT_ELEMENT_DESC g_staticMesh[4];

static bool g_elementsInitialized;

static void InitializeVertexElements();

_Use_decl_annotations_
void VRES::GetVertexElements(VertexFormat format, const D3D11_INPUT_ELEMENT_DESC** elements, uint32_t* numElements)
{
    if (!g_elementsInitialized)
    {
        InitializeVertexElements();
        g_elementsInitialized = true;
    }

    switch (format)
    {
    case VertexFormat::StaticMesh:
        *elements = g_staticMesh;
        *numElements = _countof(g_staticMesh);
        break;

    default:
        assert(false);
        *elements = nullptr;
        *numElements = 0;
        break;
    }
}

void InitializeVertexElements()
{
    g_staticMesh[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    g_staticMesh[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    g_staticMesh[0].SemanticName = "POSITION";
    g_staticMesh[1].AlignedByteOffset = sizeof(XMFLOAT3);
    g_staticMesh[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    g_staticMesh[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    g_staticMesh[1].SemanticName = "NORMAL";
    g_staticMesh[2].AlignedByteOffset = 2 * sizeof(XMFLOAT3);
    g_staticMesh[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    g_staticMesh[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    g_staticMesh[2].SemanticName = "TANGENT";
    g_staticMesh[3].AlignedByteOffset = 3 * sizeof(XMFLOAT3);
    g_staticMesh[3].Format = DXGI_FORMAT_R32G32_FLOAT;
    g_staticMesh[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    g_staticMesh[3].SemanticName = "TEXCOORD";
}
