#include "stdafx.h"
#include "xmtypes.h"

CLucid3DRenderer::CLucid3DRenderer()
{

}

CLucid3DRenderer::~CLucid3DRenderer()
{
    Clear();
}

void CLucid3DRenderer::Clear()
{
    for (UINT i = 0; i < m_buffers.size(); i++)
    {
        m_buffers[i].m_vertexBuffer.reset();
        m_buffers[i].m_indexBuffer.reset();
        m_buffers[i].m_material.reset();
    }

    m_buffers.clear();
}

void CLucid3DRenderer::Draw(DeviceContextPtr pContext, IShaderEffect* effect)
{
    for (UINT i = 0; i < m_buffers.size(); i++)
    {
        if (m_buffers[i].m_vertexBuffer && m_buffers[i].m_indexBuffer)
        {
            effect->Apply(m_buffers[i].m_material);
            pContext->SetIndexBuffer(m_buffers[i].m_indexBuffer);
            pContext->SetVertexBuffer(m_buffers[i].m_vertexBuffer);
            pContext->DrawIndexed(m_buffers[i].m_indexBuffer->GetNumIndices(), 0, 0);
        }
    }
}

HRESULT CLucid3DRenderer::AddMesh(GraphicsDevicePtr graphicsDevice, std::wstring& contentPath, IModelContentPtr pModelContent, MeshInfo meshInfo)
{
    HRESULT hr = S_OK;
    CStringW diffuseTextureName;
    Lucid3DRenderBufferDesc renderDesc;
    ModelContentBufferDescription vertexBuffer;
    ModelContentBufferDescription indexBuffer;
    ModelContentMaterialDescription material;
    InputStreamPtr spStream;
    TexturePtr tex;
    std::string contentPathA = WideToAnsi(contentPath);

    std::string diffuseTexturePath;
    std::string specularTexturePath;
    std::string normalTexturePath;
    
    // Get vertex and index buffer descriptions for target mesh
    hr = pModelContent->GetVertexBuffer(meshInfo.vertexBufferId, vertexBuffer);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to get vertex buffer");

    hr = pModelContent->GetIndexBuffer(meshInfo.indexBufferId, indexBuffer);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to get index buffer");

    hr = pModelContent->GetMaterial(meshInfo.materialId, material);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to get material");

    VertexElement elements[] =
    {
        VertexElement(0, VertexChannelType::Float3, VertexChannelSemantic::Position, false),
        VertexElement(0, VertexChannelType::Float3, VertexChannelSemantic::Normal, false),
        VertexElement(0, VertexChannelType::Float3, VertexChannelSemantic::Tangent, false),
        VertexElement(0, VertexChannelType::Float3, VertexChannelSemantic::BiTangent, false),
        VertexElement(0, VertexChannelType::Float2, VertexChannelSemantic::TexCoord0, false),
    };

    hr = graphicsDevice->CreateVertexBuffer((uint)vertexBuffer.stride, vertexBuffer.numElements, elements, _countof(elements),  ResourceAccess::Read, ResourceAccess::Write, vertexBuffer.pBuffer.get()->ptr, &renderDesc.m_vertexBuffer);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to create vertex buffer");

    hr = graphicsDevice->CreateIndexBuffer((uint)indexBuffer.stride, (uint)indexBuffer.numElements, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, ResourceAccess::Read, ResourceAccess::Write, indexBuffer.pBuffer.get()->ptr, &renderDesc.m_indexBuffer);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to create index buffer");

    hr = Renderer::CreateMaterial(std::to_string((uint64)meshInfo.materialId).c_str(), &renderDesc.m_material);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to create the material");

    diffuseTexturePath = (contentPathA + material.info.diffuse + ".tc").c_str();

    hr = IO::ReadExistingFile(diffuseTexturePath.c_str(), &spStream);
    if (FAILED(hr))
    {
        DEBUG_PRINT("Failed to load diffuse texture file '%s', hr = 0x%lx",diffuseTexturePath.c_str(), hr);
        goto Exit;
    }

    hr = Graphics::CreateTextureFromStream(graphicsDevice, spStream, false, ResourceAccess::Read, ResourceAccess::None, &tex);
    if (FAILED(hr))
    {
        DEBUG_PRINT("Failed to create texture from diffuse texture file '%s', hr = 0x%lx",diffuseTexturePath.c_str(), hr);
        goto Exit;
    }
    
    renderDesc.m_material->SetTexture(TextureUsage::Diffuse, tex);

    if (material.info.normal.size() != 0)
    {
        normalTexturePath = (contentPathA + material.info.normal + ".tc").c_str();

        hr = IO::ReadExistingFile(normalTexturePath.c_str(), &spStream);
        if (FAILED(hr))
        {
            DEBUG_PRINT("Failed to load normal texture file '%s', hr = 0x%lx",normalTexturePath.c_str(), hr);
            goto Exit;
        }

        hr = Graphics::CreateTextureFromStream(graphicsDevice, spStream, false, ResourceAccess::Read, ResourceAccess::None, &tex);
        if (FAILED(hr))
        {
            DEBUG_PRINT("Failed to create texture from normal texture file '%s', hr = 0x%lx",normalTexturePath.c_str(), hr);
            goto Exit;
        }
    
        renderDesc.m_material->SetTexture(TextureUsage::Normal, tex);
    }

    renderDesc.m_material->SetSpecularPower(120.0f);

    // Add the buffers to the render collection
    m_buffers.push_back(renderDesc);

Exit:

    return hr;
}

HRESULT CLucid3DRenderer::AddContent(GraphicsDevicePtr graphicsDevice, std::wstring& contentPath, ITextureContentPtr pTextureContent)
{
    Clear();

    HRESULT hr = S_OK;
    std::string textureName;
    std::wstring textureNameW;
    Lucid3DRenderBufferDesc renderDesc;
    InputStreamPtr spStream;
    TexturePtr tex;
    WCHAR szTempPath[MAX_PATH] = {0};
    WCHAR szTempFile[MAX_PATH] = {0};

    pTextureContent->GetTextureName(textureNameW);
    textureName = WideToAnsi(textureNameW);

    XMProjSpaceVertex vertices[] = 
    {
        { XMFLOAT2(-1, 1), XMFLOAT2(0, 0) },
        { XMFLOAT2(1, 1), XMFLOAT2(1, 0) },
        { XMFLOAT2(1, -1), XMFLOAT2(1, 1) },
        { XMFLOAT2(-1, -1), XMFLOAT2(0, 1) },
    };

    uint32 indices[] = { 0, 1, 2, 0, 2, 3 };

    VertexElement layout[] =
    {
        VertexElement(0, VertexChannelType::Float2, VertexChannelSemantic::Position, false),
        VertexElement(0, VertexChannelType::Float2, VertexChannelSemantic::TexCoord0, false),
    };

    hr = graphicsDevice->CreateVertexBuffer(sizeof(XMProjSpaceVertex), _countof(vertices), layout, _countof(layout), ResourceAccess::Read, ResourceAccess::None, vertices, &renderDesc.m_vertexBuffer);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to create vertex buffer");

    hr = graphicsDevice->CreateIndexBuffer(sizeof(indices[0]), _countof(indices), D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, ResourceAccess::Read, ResourceAccess::None, indices, &renderDesc.m_indexBuffer);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to create index buffer");

    hr = Renderer::CreateMaterial(textureName.c_str(), &renderDesc.m_material);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to create the material");

    // HACK: HACK: Coop, I'm not familiar enough with this to do the 'right' way of loading the .tc
    // so I'm hacking it up good :) you can fix this later if you want
    GetTempPath(ARRAYSIZE(szTempPath), szTempPath);
    GetTempFileName(szTempPath, L"", 0, szTempFile);
    pTextureContent->SaveToFile(szTempFile);

    hr = IO::ReadExistingFile(WideToAnsi(std::wstring(szTempFile)).c_str(), &spStream);
    if (FAILED(hr))
    {
        DEBUG_PRINT("Failed to load diffuse texture file '%s', hr = 0x%lx",szTempFile, hr);
        goto Exit;
    }

    hr = Graphics::CreateTextureFromStream(graphicsDevice, spStream, false, ResourceAccess::Read, ResourceAccess::None, &tex);
    if (FAILED(hr))
    {
        DEBUG_PRINT("Failed to create texture from diffuse texture file '%s', hr = 0x%lx", szTempFile, hr);
        goto Exit;
    }

    renderDesc.m_material->SetTexture(TextureUsage::Diffuse, tex);

    // Add the buffers to the render collection
    m_buffers.push_back(renderDesc);

Exit:

    return hr;
}

HRESULT CLucid3DRenderer::AddContent(GraphicsDevicePtr graphicsDevice, std::wstring& contentPath, IModelContentPtr pModelContent, LPCSTR meshName)
{
    Clear();

    HRESULT hr = S_OK;
    
    MeshInfo meshInfo;
    if (meshName == NULL)
    {
        // Add all meshes
        std::vector<std::string> meshNames;
        pModelContent->GetMeshNames(meshNames);
        
        for (UINT i = 0; i < meshNames.size(); i++)
        {
            hr = pModelContent->GetMesh(meshNames[i].c_str(), meshInfo);
            GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to find mesh");

            hr = AddMesh(graphicsDevice, contentPath, pModelContent, meshInfo);
            GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to add mesh to renderer");

            for (UINT m = 0; m < meshInfo.numSubMeshIds; m++)
            {
                MeshInfo subMeshInfo;

                hr = pModelContent->GetMeshPart(meshInfo.subMeshIds[m], subMeshInfo);
                GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to find sub mesh");

                hr = AddMesh(graphicsDevice, contentPath, pModelContent, subMeshInfo);
                GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to add mesh to renderer");
            }
        }
    }
    else
    {
        // Add only the target mesh
        hr = pModelContent->GetMesh(meshName, meshInfo);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to find mesh");

        hr = AddMesh(graphicsDevice, contentPath, pModelContent, meshInfo);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to add mesh to renderer");

        for (UINT i = 0; i < meshInfo.numSubMeshIds; i++)
        {
            MeshInfo subMeshInfo;

            hr = pModelContent->GetMeshPart(meshInfo.subMeshIds[i], subMeshInfo);
            GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to find sub mesh");

            hr = AddMesh(graphicsDevice, contentPath, pModelContent, subMeshInfo);
            GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to add mesh to renderer");
        }
    }

Exit:

    if FAILED(hr)
    {
        Clear();
    }

    return hr;
}

HRESULT CLucid3DRenderer::AddContent(GraphicsDevicePtr graphicsDevice, std::wstring& contentPath, ISpriteFontContentPtr pSpriteFontContent)
{
    Clear();

    HRESULT hr = S_OK;
    std::string spriteFontName;
    std::wstring spriteFontNameW;
    Lucid3DRenderBufferDesc renderDesc;
    InputStreamPtr spStream;
    TexturePtr tex;

    BYTE* pData = nullptr;
    DWORD cbData = 0;

    pSpriteFontContent->GetSpriteFontName(spriteFontNameW);
    spriteFontName = WideToAnsi(spriteFontNameW);

    XMProjSpaceVertex vertices[] = 
    {
        { XMFLOAT2(-1, 1), XMFLOAT2(0, 0) },
        { XMFLOAT2(1, 1), XMFLOAT2(1, 0) },
        { XMFLOAT2(1, -1), XMFLOAT2(1, 1) },
        { XMFLOAT2(-1, -1), XMFLOAT2(0, 1) },
    };

    uint32 indices[] = { 0, 1, 2, 0, 2, 3 };

    VertexElement layout[] =
    {
        VertexElement(0, VertexChannelType::Float2, VertexChannelSemantic::Position, false),
        VertexElement(0, VertexChannelType::Float2, VertexChannelSemantic::TexCoord0, false),
    };

    hr = graphicsDevice->CreateVertexBuffer(sizeof(XMProjSpaceVertex), _countof(vertices), layout, _countof(layout), ResourceAccess::Read, ResourceAccess::None, vertices, &renderDesc.m_vertexBuffer);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to create vertex buffer");

    hr = graphicsDevice->CreateIndexBuffer(sizeof(indices[0]), _countof(indices), D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, ResourceAccess::Read, ResourceAccess::None, indices, &renderDesc.m_indexBuffer);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to create index buffer");

    hr = Renderer::CreateMaterial(spriteFontName.c_str(), &renderDesc.m_material);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to create the material");

    // Get a stream to the DDS data stored in the spritefont content
    hr = pSpriteFontContent->GetTextureData(&pData, &cbData);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to get texture data from spritefont content");

    hr = IO::CreateInputStreamOnBuffer(pData, cbData, &spStream);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to get create lucid memory stream for texture data from spritefont content");

    hr = Graphics::CreateTextureFromStream(graphicsDevice, spStream, false, ResourceAccess::Read, ResourceAccess::None, &tex);
    if (FAILED(hr))
    {
        DEBUG_PRINT("Failed to create texture from spritefont texture data, hr = 0x%lx", hr);
        goto Exit;
    }

    renderDesc.m_material->SetTexture(TextureUsage::Diffuse, tex);

    // Add the buffers to the render collection
    m_buffers.push_back(renderDesc);

Exit:

    SAFE_DELETE_ARRAY(pData);

    return hr;
}
