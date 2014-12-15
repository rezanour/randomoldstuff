#include "stdafx.h"

HRESULT LoadCubePrimitiveContent(LPCWSTR filePath, IContentPluginServices* pServices)
{
    DEBUG_PRINT("Loading Cube Primitive model content...");
    HRESULT hr = S_OK;
    IModelContentPtr pModel;
    IContentContainerPtr pContent;
    UINT vertexBufferId = 0;
    UINT indexBufferId = 0;
    UINT meshId = 0;
    UINT materialId = 0;

    // Get content container
    pContent = pServices->GetContentContainer();

    // Create model content
    hr = pServices->CreateModelContent(&pModel);
    GOTO_EXIT_IF_FAILED(hr);


    XMPosNormTanBiTexVertex vertices[] = {
        // back face
        { XMFLOAT3(-0.5, -0.5, -0.5),   XMFLOAT3(0, 0, 0),  XMFLOAT3(-1, 0, 0),  XMFLOAT3(0, -1, 0),  XMFLOAT2(1, 1) },
        { XMFLOAT3(0.5, -0.5, -0.5),    XMFLOAT3(0, 0, 0),  XMFLOAT3(-1, 0, 0),  XMFLOAT3(0, -1, 0),  XMFLOAT2(0, 1) },
        { XMFLOAT3(0.5, 0.5, -0.5),     XMFLOAT3(0, 0, 0),  XMFLOAT3(-1, 0, 0),  XMFLOAT3(0, -1, 0),  XMFLOAT2(0, 0) },
        { XMFLOAT3(-0.5, 0.5, -0.5),    XMFLOAT3(0, 0, 0),  XMFLOAT3(-1, 0, 0),  XMFLOAT3(0, -1, 0),  XMFLOAT2(1, 0) },

        // front face
        { XMFLOAT3(-0.5, -0.5, 0.5),    XMFLOAT3(0, 0, 0),  XMFLOAT3(1, 0, 0),  XMFLOAT3(0, -1, 0),  XMFLOAT2(0, 1) },
        { XMFLOAT3(-0.5, 0.5, 0.5),     XMFLOAT3(0, 0, 0),  XMFLOAT3(1, 0, 0),  XMFLOAT3(0, -1, 0),  XMFLOAT2(0, 0) },
        { XMFLOAT3(0.5, 0.5, 0.5),      XMFLOAT3(0, 0, 0),  XMFLOAT3(1, 0, 0),  XMFLOAT3(0, -1, 0),  XMFLOAT2(1, 0) },
        { XMFLOAT3(0.5, -0.5, 0.5),     XMFLOAT3(0, 0, 0),  XMFLOAT3(1, 0, 0),  XMFLOAT3(0, -1, 0),  XMFLOAT2(1, 1) },

        // top face
        { XMFLOAT3(-0.5, 0.5, 0.5),     XMFLOAT3(0, 0, 0),  XMFLOAT3(1, 0, 0),  XMFLOAT3(0, 0, 1),  XMFLOAT2(0, 1) },
        { XMFLOAT3(-0.5, 0.5, -0.5),    XMFLOAT3(0, 0, 0),  XMFLOAT3(1, 0, 0),  XMFLOAT3(0, 0, 1),  XMFLOAT2(0, 0) },
        { XMFLOAT3(0.5, 0.5, -0.5),     XMFLOAT3(0, 0, 0),  XMFLOAT3(1, 0, 0),  XMFLOAT3(0, 0, 1),  XMFLOAT2(1, 0) },
        { XMFLOAT3(0.5, 0.5, 0.5),      XMFLOAT3(0, 0, 0),  XMFLOAT3(1, 0, 0),  XMFLOAT3(0, 0, 1),  XMFLOAT2(1, 1) },

        // bottom face
        { XMFLOAT3(-0.5, -0.5, 0.5),    XMFLOAT3(0, 0, 0),  XMFLOAT3(1, 0, 0),  XMFLOAT3(0, 0, -1),  XMFLOAT2(0, 0) },
        { XMFLOAT3(0.5, -0.5, 0.5),     XMFLOAT3(0, 0, 0),  XMFLOAT3(1, 0, 0),  XMFLOAT3(0, 0, -1),  XMFLOAT2(1, 0) },
        { XMFLOAT3(0.5, -0.5, -0.5),    XMFLOAT3(0, 0, 0),  XMFLOAT3(1, 0, 0),  XMFLOAT3(0, 0, -1),  XMFLOAT2(1, 1) },
        { XMFLOAT3(-0.5, -0.5, -0.5),   XMFLOAT3(0, 0, 0),  XMFLOAT3(1, 0, 0),  XMFLOAT3(0, 0, -1),  XMFLOAT2(0, 1) },

        // left face
        { XMFLOAT3(-0.5, 0.5, -0.5),    XMFLOAT3(0, 0, 0),  XMFLOAT3(0, 0, 1),  XMFLOAT3(0, -1, 0),  XMFLOAT2(0, 0) },
        { XMFLOAT3(-0.5, 0.5, 0.5),     XMFLOAT3(0, 0, 0),  XMFLOAT3(0, 0, 1),  XMFLOAT3(0, -1, 0),  XMFLOAT2(1, 0) },
        { XMFLOAT3(-0.5, -0.5, 0.5),    XMFLOAT3(0, 0, 0),  XMFLOAT3(0, 0, 1),  XMFLOAT3(0, -1, 0),  XMFLOAT2(1, 1) },
        { XMFLOAT3(-0.5, -0.5, -0.5),   XMFLOAT3(0, 0, 0),  XMFLOAT3(0, 0, 1),  XMFLOAT3(0, -1, 0),  XMFLOAT2(0, 1) },

        // right face
        { XMFLOAT3(0.5, 0.5, 0.5),      XMFLOAT3(0, 0, 0),      XMFLOAT3(0, 0, -1),  XMFLOAT3(0, -1, 0),  XMFLOAT2(0, 0) },
        { XMFLOAT3(0.5, 0.5, -0.5),     XMFLOAT3(0, 0, 0),      XMFLOAT3(0, 0, -1),  XMFLOAT3(0, -1, 0),  XMFLOAT2(1, 0) },
        { XMFLOAT3(0.5, -0.5, -0.5),    XMFLOAT3(0, 0, 0),      XMFLOAT3(0, 0, -1),  XMFLOAT3(0, -1, 0),  XMFLOAT2(1, 1) },
        { XMFLOAT3(0.5, -0.5, 0.5),     XMFLOAT3(0, 0, 0),      XMFLOAT3(0, 0, -1),  XMFLOAT3(0, -1, 0),  XMFLOAT2(0, 1) },
    };


    unsigned __int32 indices[] =
    {
        2,   1,  0,   3,  2,   0,
        6,   5,  4,   7,  6,   4,
        10,   9,  8,  11, 10,   8,
        14,  13,  12, 15, 14,  12,
        18,  17,  16, 19, 18,  16,
        22,  21,  20, 23, 22,  20,
    };

    //ScaleVertices(vertices, ARRAYSIZE(vertices), 3.0f);

    hr = Graphics::GenerateTangentSpaceLH(
                    (void*)(&vertices[0].position),
                    (void*)(&vertices[0].texCoord),
                    (void*)(&vertices[0].normal),
                    (void*)(&vertices[0].tangent),
                    (void*)(&vertices[0].biTangent),
                    sizeof(vertices[0]), 
                    ARRAYSIZE(vertices), 
                    indices, sizeof(UINT), ARRAYSIZE(indices), true);

    // Create vertices buffer on model content
    vertexBufferId = pModel->AddVertexBuffer((void*)vertices, ARRAYSIZE(vertices), sizeof(vertices[0]));

    // Create indices buffer on model content
    indexBufferId = pModel->AddIndexBuffer((void*)indices, ARRAYSIZE(indices), sizeof(indices[0]));

    // Create material on model content
    materialId = pModel->AddMaterial("blank", "blank", "blank");

    // Create mesh on model content (this is a virtual linkage of vertices, indices and material)
    meshId =  pModel->AddMesh("cube", vertexBufferId, indexBufferId, 0);

    // Add model content to content container
    hr = pContent->AddModelContent(pModel);

Exit:

    return hr;
}