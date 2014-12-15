#include "stdafx.h"

CModelContent::CModelContent() :
    m_meshPartId(0)
{

}

CModelContent::~CModelContent()
{

}

UINT CModelContent::AddVertexBuffer(void* pbuffer, UINT numVertices, UINT stride)
{
    UINT bufferId = (UINT)m_vertexBufferArray.size();
    
    ModelContentBufferDescription desc;
    desc.numElements = numVertices;
    desc.stride = stride;
    desc.pBuffer = std::make_shared<CCopiedBuffer>(pbuffer, numVertices * stride);
    
    m_vertexBufferArray.push_back(desc);
    
    return bufferId;
}

UINT CModelContent::AddIndexBuffer(void* pbuffer, UINT numIndices, UINT stride)
{
    UINT bufferId = (UINT)m_indexBufferArray.size();
    
    ModelContentBufferDescription desc;
    desc.numElements = numIndices;
    desc.stride = stride;
    desc.pBuffer = std::make_shared<CCopiedBuffer>(pbuffer, numIndices * stride);
    
    m_indexBufferArray.push_back(desc);
    
    return bufferId;
}

UINT CModelContent::AddMaterial(LPCSTR diffuse, LPCSTR specular, LPCSTR normal)
{
    // First attempt to find matching material to prevent duplicates from being added to the
    // materials list.
    UINT materialId = (UINT)m_materialArray.size();
    MaterialInfo materialInfo;
    materialInfo.diffuse = diffuse != NULL ? diffuse : "concrete.tc";
    materialInfo.normal = normal != NULL ? normal : materialInfo.diffuse;
    materialInfo.specular = specular != NULL ? specular : materialInfo.diffuse;

    bool addNewMaterial = true;

    for (UINT i = 0; i < m_materialArray.size(); i++)
    {
        if ((m_materialArray[i].diffuse == materialInfo.diffuse) &&
            (m_materialArray[i].specular == materialInfo.specular) &&
            (m_materialArray[i].normal == materialInfo.normal))
        {
            materialId = i;
            addNewMaterial = false;
            break;
        }
    }

    if (addNewMaterial)
    {
        m_materialArray.push_back(materialInfo);
    }

    return materialId;
}

HRESULT CModelContent::AddMesh(LPCSTR name, UINT vertexBufferId, UINT indexBufferId, UINT materialId)
{
    HRESULT hr = S_OK;
    MeshInfo meshInfo;
    memset(&meshInfo.subMeshIds, 0, sizeof(meshInfo.subMeshIds));
    meshInfo.numSubMeshIds = 0;
    meshInfo.vertexBufferId = vertexBufferId;
    meshInfo.materialId = materialId;
    meshInfo.indexBufferId = indexBufferId;

    if (IsValidMesh(meshInfo) && name != NULL)
    {
        m_meshMap.insert(std::pair<std::string,MeshInfo>(std::string(name), meshInfo));
    }
    else
    {
        hr = E_INVALIDARG;
    }

    return hr;
}

HRESULT CModelContent::AddMeshPart(LPCSTR name, UINT vertexBufferId, UINT indexBufferId, UINT materialId)
{
    HRESULT hr = S_OK;

    MeshInfo meshInfo;
    hr = GetMesh(name, meshInfo);
    if (SUCCEEDED(hr))
    {
        // Find the next available mesh part id to use
        UINT meshPartId = GetNextMeshPartId();

        // Add a new mesh using the MESHPART_PREFIX + mesh part id
        std::string meshPartName = MESHPART_PREFIX + ConvertToA<std::string>(meshPartId);

        // Update parent meshInfo 
        m_meshMap[name].numSubMeshIds++;
        m_meshMap[name].subMeshIds[meshInfo.numSubMeshIds] = meshPartId;

        // Add the mesh part
        hr = AddMesh(meshPartName.c_str(), vertexBufferId, indexBufferId, materialId);
    }

    return hr;
}

HRESULT CModelContent::GetMesh(LPCSTR name, MeshInfo& meshInfo)
{
    HRESULT hr = S_OK;

    meshInfo = m_meshMap[name];

    return hr;
}

HRESULT CModelContent::GetMeshPart(UINT meshInfoId, MeshInfo& meshInfo)
{
    std::string meshName = MESHPART_PREFIX + ConvertToA<std::string>(meshInfoId);
    return GetMesh(meshName.c_str(), meshInfo);
}

void CModelContent::GetMeshNames(std::vector<std::string>& meshNames)
{
    for ( std::map< std::string, MeshInfo >::const_iterator iter = m_meshMap.begin(); iter != m_meshMap.end(); ++iter )
    {
        // only return mesh names that do not contain an id prefix
        if (iter->first.rfind(MESHPART_PREFIX) == std::string::npos)
        {
            meshNames.push_back(iter->first);
            // iter->first  (key)
            // iter->second (value)
        }
    }
}

HRESULT CModelContent::GetVertexBuffer(UINT id, ModelContentBufferDescription& desc)
{
    HRESULT hr = S_OK;
    desc = m_vertexBufferArray[id];
    return hr;
}

HRESULT CModelContent::GetIndexBuffer(UINT id, ModelContentBufferDescription& desc)
{
    HRESULT hr = S_OK;
    desc = m_indexBufferArray[id];
    return hr;
}

HRESULT CModelContent::GetMaterial(UINT id, ModelContentMaterialDescription& desc)
{
    HRESULT hr = S_OK;

    desc.info = m_materialArray[id];

    return hr;
}

HRESULT CModelContent::SetMaterial(UINT id, ModelContentMaterialDescription& desc)
{
    HRESULT hr = S_OK;

    m_materialArray[id] = desc.info;

    return hr;
}

bool CModelContent::IsValidMesh(MeshInfo meshInfo)
{
    bool isValid = ((meshInfo.vertexBufferId < m_vertexBufferArray.size()) &&
                    (meshInfo.indexBufferId < m_indexBufferArray.size()) &&
                    (meshInfo.materialId < m_materialArray.size()));

    return isValid;
}

// Basic layout
// [file header][buffer header][vertices][buffer header][indices][materials][meshes]

HRESULT CModelContent::SaveToFile(LPCWSTR filePath)
{
    HRESULT hr = S_OK;
    DWORD cbWritten = 0;
    IStream* pStream = NULL;
    MODELCONTENTFILEHEADER header = {0};
    header.dwSize              = sizeof(header);
    header.numIndexBuffers     = (DWORD)m_indexBufferArray.size();
    header.numMaterials        = (DWORD)m_materialArray.size();
    header.numMeshes           = (DWORD)m_meshMap.size();
    header.numVertexBuffers    = (DWORD)m_vertexBufferArray.size();
    header.offsetVertexBuffers = header.dwSize; // vertex buffers start directly after the header
    header.offsetIndexBuffers  = header.offsetVertexBuffers + GetTotalVerticesBufferByteSize();
    header.offsetMaterials     = header.offsetIndexBuffers + GetTotalIndicesBufferByteSize();
    header.offsetMeshes        = header.offsetMaterials + GetTotalMaterialsBytesSize();
    
    hr = FileStream::OpenFile(filePath, &pStream, true);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to create model content file");

    // Write file header
    hr = pStream->Write(&header, sizeof(header), &cbWritten);

    // Write vertices buffers
    for (UINT i = 0; i < m_vertexBufferArray.size(); i++)
    {
        MODELCONTENTBUFFERHEADER bufHeader = {0};
        bufHeader.numElements = m_vertexBufferArray[i].numElements;
        bufHeader.stride = m_vertexBufferArray[i].stride;
        hr = pStream->Write(&bufHeader, sizeof(bufHeader), &cbWritten);
        hr = pStream->Write(m_vertexBufferArray[i].pBuffer.get()->ptr, (ULONG)m_vertexBufferArray[i].pBuffer->numBytes, &cbWritten);
    }

    // Write indices buffers
    for (UINT i = 0; i < m_indexBufferArray.size(); i++)
    {
        MODELCONTENTBUFFERHEADER bufHeader = {0};
        bufHeader.numElements = m_indexBufferArray[i].numElements;
        bufHeader.stride = m_indexBufferArray[i].stride;
        hr = pStream->Write(&bufHeader, sizeof(bufHeader), &cbWritten);
        hr = pStream->Write(m_indexBufferArray[i].pBuffer.get()->ptr, (ULONG)m_indexBufferArray[i].pBuffer->numBytes, &cbWritten);
    }

    // Write materials
    for (UINT i = 0; i < m_materialArray.size(); i++)
    {
        MODELCONTENTMATERIALHEADER materialheader = {0};
        StringCchCopyA( materialheader.diffuse, ARRAYSIZE(materialheader.diffuse), m_materialArray[i].diffuse.c_str() );
        StringCchCopyA( materialheader.specular, ARRAYSIZE(materialheader.specular), m_materialArray[i].specular.c_str() );
        StringCchCopyA( materialheader.normal, ARRAYSIZE(materialheader.normal), m_materialArray[i].normal.c_str() );
        hr = pStream->Write(&materialheader, sizeof(materialheader), &cbWritten);
    }

    // Write meshes
    for ( std::map< std::string, MeshInfo >::const_iterator iter = m_meshMap.begin(); iter != m_meshMap.end(); ++iter )
    {
        MODELCONTENTMESHHEADER meshHeader = {0};
        StringCchCopyA( meshHeader.name, ARRAYSIZE(meshHeader.name), iter->first.c_str() );
        for (UINT i = 0; i < iter->second.numSubMeshIds; i++)
        {
            meshHeader.info.subMeshIds[i] = iter->second.subMeshIds[i];
        }
        meshHeader.info.numSubMeshIds = iter->second.numSubMeshIds;
        meshHeader.info.indexBufferId = iter->second.indexBufferId;
        meshHeader.info.materialId = iter->second.materialId;
        meshHeader.info.vertexBufferId = iter->second.vertexBufferId;
        hr = pStream->Write(&meshHeader, sizeof(meshHeader), &cbWritten);
    }

Exit:

    SAFE_RELEASE(pStream);

    return hr;
}

HRESULT CModelContent::LoadFromFile(LPCWSTR filePath)
{
    HRESULT hr = S_OK;
    DWORD cbRead = 0;
    IStream* pStream = NULL;
    MODELCONTENTFILEHEADER header = {0};
        
    hr = FileStream::OpenFile(filePath, &pStream, false);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to load model content file");

    // Read file header
    hr = pStream->Read(&header, sizeof(header), &cbRead);

    // Read vertices buffers
    for (UINT i = 0; i < header.numVertexBuffers; i++)
    {
        // Read header
        MODELCONTENTBUFFERHEADER bufHeader = {0};
        hr = pStream->Read(&bufHeader, sizeof(bufHeader), &cbRead);

        // Read data
        BYTE* pBuffer = new BYTE[bufHeader.numElements * bufHeader.stride];
        hr = pStream->Read(pBuffer, bufHeader.numElements * bufHeader.stride, &cbRead);

        // Add vertex buffer
        AddVertexBuffer(pBuffer, bufHeader.numElements, bufHeader.stride);

        SAFE_DELETE_ARRAY(pBuffer);
    }

    // Read indices buffers
    for (UINT i = 0; i < header.numIndexBuffers; i++)
    {
        MODELCONTENTBUFFERHEADER bufHeader = {0};
        hr = pStream->Read(&bufHeader, sizeof(bufHeader), &cbRead);
        
        // Read data
        BYTE* pBuffer = new BYTE[bufHeader.numElements * bufHeader.stride];
        hr = pStream->Read(pBuffer, bufHeader.numElements * bufHeader.stride, &cbRead);

        // Add index buffer
        AddIndexBuffer(pBuffer, bufHeader.numElements, bufHeader.stride);

        SAFE_DELETE_ARRAY(pBuffer);
    }

    // Read materials
    for (UINT i = 0; i < header.numMaterials; i++)
    {
        MODELCONTENTMATERIALHEADER materialheader = {0};
        hr = pStream->Read(&materialheader, sizeof(materialheader), &cbRead);

        // Add material
        AddMaterial(materialheader.diffuse, materialheader.specular, materialheader.normal);
    }

    // Read meshes
    for (UINT i = 0; i < header.numMeshes; i++)
    {
        MODELCONTENTMESHHEADER meshHeader = {0};
        hr = pStream->Read(&meshHeader, sizeof(meshHeader), &cbRead);

        // Add mesh
        //AddMesh(meshHeader.name, meshHeader.info.vertexBufferId, meshHeader.info.indexBufferId, meshHeader.info.materialId);
        m_meshMap.insert(std::pair<std::string,MeshInfo>(std::string(meshHeader.name), meshHeader.info));

    }

Exit:

    SAFE_RELEASE(pStream);

    return hr;
}

DWORD CModelContent::GetTotalVerticesBufferByteSize()
{
    DWORD dwSize = 0;
    for (UINT i = 0; i < m_vertexBufferArray.size(); i++)
    {
        dwSize += sizeof(MODELCONTENTBUFFERHEADER);
        dwSize += (DWORD)m_vertexBufferArray[i].pBuffer->numBytes;
    }
    return dwSize;
}

DWORD CModelContent::GetTotalIndicesBufferByteSize()
{
    DWORD dwSize = 0;
    for (UINT i = 0; i < m_indexBufferArray.size(); i++)
    {
        dwSize += sizeof(MODELCONTENTBUFFERHEADER);
        dwSize += (DWORD)m_indexBufferArray[i].pBuffer->numBytes;
    }
    return dwSize;
}

DWORD CModelContent::GetTotalMaterialsBytesSize()
{
    DWORD dwSize = 0;
    for (UINT i = 0; i < m_materialArray.size(); i++)
    {
        dwSize += sizeof(MODELCONTENTMATERIALHEADER);
    }
    return dwSize;
}

DWORD CModelContent::GetTotalMeshesByteSize()
{
    return (DWORD)m_meshMap.size() * sizeof(MODELCONTENTMESHHEADER);
}

IModelContentPtr CModelContent::FromFile(LPCWSTR filePath)
{
    HRESULT hr = S_OK;
    IModelContentPtr pModelContent;
    CContentPluginServices* pServices = new CContentPluginServices();
    
    hr = pServices->CreateModelContent(&pModelContent);
    if (FAILED(hr))
    {
        DEBUG_PRINT( "Failed to load model content file '%ws', hr = 0x%lx ", filePath, hr);
        goto Exit;
    }

    hr = pModelContent->LoadFromFile(filePath);
    if (FAILED(hr))
    {
        DEBUG_PRINT( "Failed to load model content file '%ws', hr = 0x%lx ", filePath, hr);
        goto Exit;
    }

Exit:

    if FAILED(hr)
    {
        pModelContent.reset();
    }

    SAFE_DELETE(pServices);

    return pModelContent;
}