#pragma once

#include <FileStream.h>
#include <System\Lucid3D.System.h>

HRESULT Load3dwContent(LPCWSTR filePath, IContentPluginServices* pServices);

#pragma once

#define OBJECTTYPE_GROUP         L"group"
#define OBJECTTYPE_VISGROUP      L"visgroup"
#define OBJECTTYPE_MATERIAL      L"material"
#define OBJECTTYPE_MESHREFERENCE L"meshreference"
#define OBJECTTYPE_LIGHTMAP      L"lightmap"
#define OBJECTTYPE_BRUSH         L"brush"
#define OBJECTTYPE_MESH          L"mesh"
#define OBJECTTYPE_ENTITY        L"entity"
#define OBJECTTYPE_TERRAIN       L"terrain"

// Ensure that lightmap data is 32-bit for texture processing assumptions
#ifndef MAKE_32BIT_LIGHTMAPS
#define MAKE_32BIT_LIGHTMAPS
#endif

// Binary compatible structures for .3DW files
#pragma pack(push,1)
struct _3DWFILEHEADER
{
    WORD version;
    BYTE flags;
    LONG cNames;
    LONG offsetNames;
    LONG cObjects;
    LONG offsetObjects;
};

struct _3DWOBJECTHEADER
{
    LONG id;
    LONG dataSize;
};

struct _3DWMATERIAL
{
    BYTE flags;
    LONG nameId;
    LONG objectNameId;
};

struct _3DWMESHREFERENCE
{
    BYTE flags;
    LONG nameId;
    LONG objectNameId;
    BYTE limbCount;
    LONG objectIndex; // Index into the full object list.
                      // This member should be subtracted from sizeof(_3DWMESHREFERENCE)
                      // when reading this data from the .3dw file since it is not part
                      // of the raw layout.
};

struct _3DWLIGHTMAPHEADER
{
    BYTE flags;
    BYTE resolution;
    LONG format;
};

struct _3DWGROUPHEADER
{
    BYTE flags;
    LONG groupIndex;
};

struct _3DWCOLOR_24BIT
{
    BYTE r;
    BYTE g;
    BYTE b;
};

struct _3DWVISGROUPHEADER
{
    BYTE            flags;
    LONG            nameId;
    _3DWCOLOR_24BIT color;
};

struct _3DWBRUSHHEADER
{
    BYTE flags;
    LONG keyCount;
};

struct _3DWBRUSHDATAHEADER
{
    LONG            groupIndex;
    LONG            visgroupIndex;
    _3DWCOLOR_24BIT color;
};

struct _3DWVECTOR2
{
    FLOAT x;
    FLOAT y;
};

struct _3DWVECTOR3
{
    FLOAT x;
    FLOAT y;
    FLOAT z;
};

struct _3DWVECTOR4
{
    FLOAT x;
    FLOAT y;
    FLOAT z;
    FLOAT w;
};

struct _3DWFACEHEADER
{
    BYTE        flags;
    _3DWVECTOR4 facePlanEquation;
    _3DWVECTOR2 texturePosition;
    _3DWVECTOR2 textureScale;
    _3DWVECTOR2 textureRotation;
    _3DWVECTOR4 UTextreMappingPlane;
    _3DWVECTOR4 VTextreMappingPlane;
    FLOAT       luxelSize;
    LONG        smoothGroupIndex;
    LONG        materialIndex;
};

struct _3DWTERRAINHEADER
{
    BYTE        flags;
    _3DWVECTOR3 position;
    FLOAT       width;
    FLOAT       height;
    LONG        nameId;
    LONG        resolution;
    LONG        sectors;
    LONG        detaillevels;
    FLOAT       lightmapresolution;
    LONG        layercount;
};

struct _3DWINDEX
{
    BYTE        vertexId;
    _3DWVECTOR2 texCoord1;
    _3DWVECTOR2 texCoord2; // optional if (flags & 16)
};

struct _3DWLIGHTMAP
{
    BYTE  flags;
    LONG  resolution;
    LONG  format;
    LONG  cbData;
    BYTE* pData;
};

struct _3DWTERRAINLAYER
{
    LONG  nameId;
    LONG  materialIndex;
    LONG  cbData;
    BYTE* pData;
};

#pragma pack(pop)

// structures that contain collections of other structures
struct _3DWBRUSHKEY
{
    std::wstring name;
    std::wstring value;
};

struct _3DWFACE
{
    _3DWFACEHEADER         header;
    LONG                   lightMapIndex; // optional if (flags & 16)
    std::vector<_3DWINDEX> indices;
};

struct _3DWTERRAIN_DATA
{
    _3DWTERRAINHEADER             header;
    LONG                          cblightmap;
    BYTE*                         plightmapData;
    LONG                          cbheightmap;
    FLOAT*                        pheightData;
    std::vector<_3DWTERRAINLAYER> layers;
};

struct _3DWBRUSH
{
    BYTE                      flags; // (flags & 16) = light map used
    std::vector<_3DWBRUSHKEY> keys;
    LONG                      groupIndex;
    LONG                      visgroupIndex;
    _3DWCOLOR_24BIT           color;
    std::vector<_3DWVECTOR3>  vertices;
    std::vector<_3DWFACE>     faces;
};

struct _3DWENTITY
{
    BYTE                      flags;
    _3DWVECTOR3               position;
    std::vector<_3DWBRUSHKEY> keys;
    LONG                      groupIndex;
    LONG                      visgroupIndex;
};

struct _3DWMESHDATA
{
    LONG                         materialIndex;
    std::vector<_3DWCOLOR_24BIT> vertexColors; // if (flags & 16)
};

struct _3DWMESH
{
    BYTE                      flags;
    std::vector<_3DWBRUSHKEY> keys;
    LONG                      groupIndex;
    LONG                      visgroupIndex;
    _3DWCOLOR_24BIT           color;
    LONG                      meshreferenceIndex;
    _3DWVECTOR3               position;
    _3DWVECTOR3               rotation;
    _3DWVECTOR3               scale; // !(if flags & 1)
    std::vector<_3DWMESHDATA> meshdata;
};

class Raw3DWFile
{
public:
    Raw3DWFile()
    {
        currentObjectIndex = 1; // 1-based index (.3dw file data uses 1-based)
    }

    virtual ~Raw3DWFile()
    {
        Clear();
    }

    void Clear()
    {
        // reset object index
        currentObjectIndex = 1;

        // clear collections and free any allocated data
        stringTable.clear();
        brushes.clear();
        materials.clear();
        groups.clear();
        visgroups.clear();
        entities.clear();
        meshreferences.clear();
        
        // Free allocated lightmap data
        for (UINT i = 0; i < lightmaps.size(); i++)
        {
            SAFE_ARRAY_DELETE(lightmaps[i].pData);
            lightmaps[i].cbData = 0;
        }
        
        lightmaps.clear();

        for (UINT i = 0; i < terrains.size(); i++)
        {
            // Free lightmap data
            SAFE_ARRAY_DELETE(terrains[i].plightmapData);
            terrains[i].cblightmap = 0;

            // Free height data
            SAFE_ARRAY_DELETE(terrains[i].pheightData);
            terrains[i].cbheightmap = 0;

            // Free terrain layer data
            for (UINT j = 0; j < terrains[i].layers.size(); j++)
            {
                SAFE_ARRAY_DELETE(terrains[i].layers[j].pData);
            }
        }
        
        terrains.clear();

        for (UINT i = 0; i < meshes.size(); i++)
        {
            meshes[i].meshdata.clear();
        }
        
        meshes.clear();
    }

    HRESULT Initialize(LPCWSTR filePath)
    {
        Clear();

        HRESULT        hr          = S_OK;
        ULONG          cbRead      = 0;
        _3DWFILEHEADER header      = {0};
        IStream*       pFileStream = NULL;

        //hr = SHCreateStreamOnFileEx(filePath, STGM_READ, FILE_ATTRIBUTE_NORMAL, FALSE, NULL, &pFileStream);
        hr = FileStream::OpenFile(filePath, &pFileStream, false);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "SHCreateStreamOnFileEx");
    
        // Read 3DW file header
        hr = pFileStream->Read(&header, sizeof(header), &cbRead);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading 3DW file header");

        DebugOutputFileHeader(&header);

        // Read string table
        hr = ReadStringTable(pFileStream, header);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading 3DW string table");

        DebugOutStringTable();

        // Read object data
        hr = ReadObjects(pFileStream, header);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading 3DW objects");

    Exit:
        SAFE_RELEASE(pFileStream);

        return hr;
    }

    void CalculateVertexNormals(XMPosNormTexVertex* pVertices, DWORD dwNumVertices, DWORD* pIndices, DWORD dwNumIndices)
    {
        for (UINT i = 0; i < dwNumIndices; i+=3)
        {
            pVertices[pIndices[i]].normal = CalculateNormal(pVertices[pIndices[i]].position, pVertices[pIndices[i + 1]].position, pVertices[pIndices[i + 2]].position);
            pVertices[pIndices[i + 1]].normal = pVertices[pIndices[i]].normal;
            pVertices[pIndices[i + 2]].normal = pVertices[pIndices[i]].normal;
        }
    }

    std::wstring FileFromFilePath(LPCWSTR szFilePath, BOOL includeExtensionInName)
    {
        // Trim filename from full path
        std::wstring filePath = szFilePath;
        size_t pos = filePath.find_last_of(L"\\");
        std::wstring fileName = filePath.substr(pos + 1);
    
        // Trim filename without extension
        pos = fileName.find_last_of(L".");
        std::wstring name = fileName.substr(0, pos);

        if (includeExtensionInName)
        {
            return fileName;
        }
        else
        {
            return name;
        }
    }

    std::wstring FilePathwithNewExtension(LPCWSTR szFilePath, LPCWSTR szExtension)
    {
        // Trim filenpath without extension from full path
        std::wstring filePath = szFilePath;
        size_t pos = filePath.find_last_of(L".");
        filePath = filePath.substr(0, pos);
    
        std::wstring extension = szExtension;
        pos = extension.find_last_of(L".");
        if (pos != std::wstring::npos)
        {
            extension = extension.substr(pos + 1);
        }

        // Add extension
        filePath = filePath + L"." + extension;

        return filePath;
    }

    std::wstring AnsiToWide(std::string s)
    {
        std::wstring ws(s.size(), L' ');
        std::copy(s.begin(), s.end(), ws.begin());
        return ws;
    }

    std::string WideToAnsi(std::wstring ws)
    {
        std::string s(ws.size(), ' ');
        std::copy(ws.begin(), ws.end(), s.begin());
        return s;
    }

    std::wstring FilePathOnly(LPCWSTR szFilePath)
    {
        // Trim filenpath without extension from full path
        std::wstring filePath = szFilePath;
        size_t pos = filePath.find_last_of(L"\\");
        filePath = filePath.substr(0, pos);
        filePath += L"\\";
        return filePath;
    }

    HRESULT LoadLightmaps(IContentPluginServices* pServices)
    {
        HRESULT hr = S_OK;

        for (UINT i = 0; i < lightmaps.size(); i++)
        {
            _3DWLIGHTMAP lightMap = lightmaps[i];
            ITextureContentPtr pTextureContent;

            hr = pServices->CreateTextureContent(&pTextureContent);
            if (SUCCEEDED(hr))
            {
                hr = pTextureContent->LoadFromBuffer(lightMap.resolution, lightMap.resolution, DXGI_FORMAT_B8G8R8A8_UNORM, (lightMap.resolution * 4), (lightMap.resolution * lightMap.resolution * 4), (BYTE*)lightMap.pData);
                if (SUCCEEDED(hr))
                {
                    pTextureContent->SetTextureName(std::wstring(L"lightmap") + std::to_wstring((__int64)i));
                    hr = pServices->GetContentContainer()->AddTextureContent(pTextureContent);
                }
            }
        }

        return hr;
    }

    HRESULT Load(LPCWSTR filePath, IModelContentPtr pModelContent, IContentPluginServices* pServices)
    {
        std::wstring contentRootPath = FilePathOnly(filePath);
        HRESULT hr = S_OK;
        DWORD* pIndices = NULL;
        XMPosNormTanBiTexVertex* pVertices = NULL;
        //std::vector<XMPosNormTanBiTexVertex> vertices;
        //std::vector<size_t> indices;
        UINT materialId = 0;
        UINT vertexBufferId = 0;
        UINT indexBufferId = 0;

        std::map<std::wstring, std::wstring> texturesLoadedMap;

        hr = Initialize(filePath);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Initializing 3DW raw file");
        
        hr = LoadLightmaps(pServices);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Processing lightmaps");

        size_t dwNumVertices = 0;
        size_t dwNumFaces = 0;
        size_t dwNumIndices = 0;

        // enumerate all brushes
        for (UINT b = 0; b < brushes.size(); b++)
        {
            UINT faceMaterialId = 0;

            // create maps of vectors to track material-to-vertices groupings
            std::map<UINT, std::vector<XMPosNormTanBiTexVertex>> brushVerticesMap;
            std::map<UINT, std::vector<UINT>> brushIndicesMap;

            // indices data for brush (and texture coordinates)
            for (UINT f = 0; f < brushes[b].faces.size(); f++)
            {
                faceMaterialId = brushes[b].faces[f].header.materialIndex - 1; // 1-based raw id adjusted to 0-based id

                // Fix up the face to be composed of triangles
                TriangulateFace(brushes[b].faces[f], true);

                // collect indices values and update texture coordinates in vertices array
                for (UINT i = 0; i < brushes[b].faces[f].indices.size(); i++)
                {
                    DWORD vertexId = brushes[b].faces[f].indices[i].vertexId;

                    XMPosNormTanBiTexVertex vertex;
                    vertex.position.x = brushes[b].vertices[vertexId].x;
                    vertex.position.y = brushes[b].vertices[vertexId].y;
                    vertex.position.z = brushes[b].vertices[vertexId].z;
                    vertex.texCoord.x = brushes[b].faces[f].indices[i].texCoord1.x;
                    vertex.texCoord.y = brushes[b].faces[f].indices[i].texCoord1.y;

                    // save the vertex
                    //vertices.push_back(vertex);
                    brushVerticesMap[faceMaterialId].push_back(vertex);

                    // save the indice
                    //indices.push_back(vertices.size() - 1);
                    brushIndicesMap[faceMaterialId].push_back(brushVerticesMap[faceMaterialId].size() - 1);
                }
            }

            // Walk maps to create proper mesh and possibly sub meshes
            bool bfirstMesh = true;
            for ( std::map< UINT, std::vector<XMPosNormTanBiTexVertex> >::const_iterator iter = brushVerticesMap.begin(); iter != brushVerticesMap.end(); ++iter )
            {
                UINT matId = iter->first;

                // iter->first  (key)
                dwNumVertices = brushVerticesMap[matId].size();
                dwNumIndices = brushIndicesMap[matId].size();
                dwNumFaces = dwNumIndices / 3;
                
                // iter->second (value)

                // Create vertex buffer
                DWORD dwVertexStride = sizeof(XMPosNormTanBiTexVertex);
                pVertices = new XMPosNormTanBiTexVertex[dwNumVertices];
                for (DWORD i = 0; i < dwNumVertices; i++)
                {
                    pVertices[i].position.x = brushVerticesMap[matId][i].position.x * 0.03125f; // scaled
                    pVertices[i].position.y = brushVerticesMap[matId][i].position.y * 0.03125f;
                    pVertices[i].position.z = brushVerticesMap[matId][i].position.z * 0.03125f;
                    pVertices[i].texCoord.x = brushVerticesMap[matId][i].texCoord.x;
                    pVertices[i].texCoord.y = brushVerticesMap[matId][i].texCoord.y;
                }

                // Create indices buffer
                pIndices = new DWORD[dwNumIndices];
                for (DWORD i = 0; i < dwNumIndices; i++)
                {
                    pIndices[i] = (DWORD)brushIndicesMap[matId][i];
                }

                hr = Graphics::GenerateTangentSpaceLH(
                    (void*)(&pVertices[0].position),
                    (void*)(&pVertices[0].texCoord),
                    (void*)(&pVertices[0].normal),
                    (void*)(&pVertices[0].tangent),
                    (void*)(&pVertices[0].biTangent),
                    dwVertexStride, 
                    dwNumVertices, 
                    pIndices, sizeof(UINT), dwNumIndices, true);

                vertexBufferId = pModelContent->AddVertexBuffer(pVertices, dwNumVertices, dwVertexStride);
                indexBufferId = pModelContent->AddIndexBuffer(pIndices, dwNumIndices, sizeof(UINT));
                std::string meshName = std::string("mesh") + ConvertToA<std::string>(b);

                if (materials.size() > matId)
                {
                    std::wstring diffuseName;
                    std::string diffuseNameA;
                    std::wstring fullTexturePath;
                    std::wstring texDir = stringTable[materials[matId].nameId];
                    std::wstring texFile = stringTable[materials[matId].objectNameId];
            
                    fullTexturePath = contentRootPath + texDir + L"\\" + texFile + L".jpg";
                    diffuseName = texFile;
                    diffuseNameA = WideToAnsi(diffuseName);

                    materialId = pModelContent->AddMaterial(diffuseNameA.c_str(), NULL, (diffuseNameA + "_n").c_str());

                    //DEBUG_PRINT("Mesh Texture = %ws", fullTexturePath.c_str());

                    // If we have not already created a texture content of this asset, create one and add it to the
                    // content container
                    std::map<std::wstring,std::wstring>::iterator it;
                    it = texturesLoadedMap.find(fullTexturePath);
                    if (it != texturesLoadedMap.end())
                    {
                        DEBUG_PRINT("* texture '%ws' already loaded...", fullTexturePath.c_str());
                    }
                    else
                    {
                        DEBUG_PRINT("* texture '%ws' is being loaded...", fullTexturePath.c_str());
                        // Create texture content from model's source texture asset
                        ITextureContentPtr pDiffuseTexture = NULL;
                        pServices->CreateTextureContent(&pDiffuseTexture);
                        HRESULT texHr = pDiffuseTexture->LoadFromFile(fullTexturePath.c_str());
                        if (SUCCEEDED(texHr))
                        {
                            pDiffuseTexture->GenerateMipmaps(10);
                            pDiffuseTexture->SetTextureName(FileFromFilePath(fullTexturePath.c_str(), false));
                            pServices->GetContentContainer()->AddTextureContent(pDiffuseTexture);
                        
                            // Add loaded texture to map to track loading
                            texturesLoadedMap.insert(std::pair<std::wstring,std::wstring>(fullTexturePath, fullTexturePath));
                        }
                        else
                        {
                            DEBUG_PRINT("Failed to load texture '%ws', hr = 0x%lx", fullTexturePath.c_str(), texHr);
                        }

                        ITextureContentPtr pNormalTexture = NULL;
                        pServices->CreateTextureContent(&pNormalTexture);
                        texHr = pNormalTexture->LoadFromFile(fullTexturePath.c_str());
                        if (SUCCEEDED(texHr))
                        {
                            pNormalTexture->ConvertToNormalMap(10.0f);
                            pNormalTexture->GenerateMipmaps(10);
                            pNormalTexture->SetTextureName(FileFromFilePath(fullTexturePath.c_str(), false) + L"_n");
                            pServices->GetContentContainer()->AddTextureContent(pNormalTexture);
                        }
                        else
                        {
                            DEBUG_PRINT("Failed to load texture '%ws', hr = 0x%lx", fullTexturePath.c_str(), texHr);
                        }
                    }
                }

                if (bfirstMesh)
                {
                    bfirstMesh = false;
                    pModelContent->AddMesh(meshName.c_str(), vertexBufferId, indexBufferId, materialId);
                }
                else
                {
                    pModelContent->AddMeshPart(meshName.c_str(), vertexBufferId, indexBufferId, materialId);
                }

                // Free temporary buffers
                SAFE_ARRAY_DELETE(pIndices);
                SAFE_ARRAY_DELETE(pVertices);
            }
        }

    Exit:

        SAFE_ARRAY_DELETE(pIndices);
        SAFE_ARRAY_DELETE(pVertices);
        
        return hr;
    }

    HRESULT LoadAsOneMesh(LPCWSTR filePath, IModelContentPtr pModelContent, IContentPluginServices* pServices)
    {
        HRESULT hr = S_OK;
        DWORD* pIndices = NULL;
        XMPosNormTanBiTexVertex* pVertices = NULL;
        std::vector<XMPosNormTanBiTexVertex> vertices;
        std::vector<size_t> indices;
        UINT materialId = 0;
        UINT vertexBufferId = 0;
        UINT indexBufferId = 0;

        hr = Initialize(filePath);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Initializing 3DW raw file");
        
        size_t dwNumVertices = 0;
        size_t dwNumFaces = 0;
        size_t dwNumIndices = 0;

        for (UINT b = 0; b < brushes.size(); b++)
        {
            size_t dwVertexOffset = vertices.size();

            // indices data for brush (and texture coordinates)
            for (UINT f = 0; f < brushes[b].faces.size(); f++)
            {
                // Fix up the face to be composed of triangles
                TriangulateFace(brushes[b].faces[f], true);

                // collect indices values and update texture coordinates in vertices array
                for (UINT i = 0; i < brushes[b].faces[f].indices.size(); i++)
                {
                    DWORD vertexId = brushes[b].faces[f].indices[i].vertexId;

                    XMPosNormTanBiTexVertex vertex;
                    vertex.position.x = brushes[b].vertices[vertexId].x;
                    vertex.position.y = brushes[b].vertices[vertexId].y;
                    vertex.position.z = brushes[b].vertices[vertexId].z;
                    vertex.texCoord.x = brushes[b].faces[f].indices[i].texCoord1.x;
                    vertex.texCoord.y = brushes[b].faces[f].indices[i].texCoord1.y;

                    // save the vertex
                    vertices.push_back(vertex);

                    // save the indice
                    indices.push_back(vertices.size() - 1);
                }
            }
        }

        dwNumVertices = vertices.size();
        dwNumIndices = indices.size();
        dwNumFaces = dwNumIndices / 3;

        // Create vertex buffer
        DWORD dwVertexStride = sizeof(XMPosNormTanBiTexVertex);
        pVertices = new XMPosNormTanBiTexVertex[dwNumVertices];
        for (DWORD i = 0; i < dwNumVertices; i++)
        {
            pVertices[i].position.x = vertices[i].position.x * 0.03125f; // scaled
            pVertices[i].position.y = vertices[i].position.y * 0.03125f;
            pVertices[i].position.z = vertices[i].position.z * 0.03125f;
            pVertices[i].texCoord.x = vertices[i].texCoord.x;
            pVertices[i].texCoord.y = vertices[i].texCoord.y;
        }

        // Create indices buffer
        pIndices = new DWORD[dwNumIndices];
        for (DWORD i = 0; i < dwNumIndices; i++)
        {
            pIndices[i] = (DWORD)indices[i];
        }

        // Calculate normals
        //hr = Graphics::GenerateVertexNormals(
        //    (void*)(&pVertices[0].position),
        //    (void*)(&pVertices[0].normal),
        //    dwVertexStride, 
        //    dwNumVertices, 
        //    pIndices, sizeof(UINT), dwNumIndices, true);

        hr = Graphics::GenerateTangentSpaceLH(
            (void*)(&pVertices[0].position),
            (void*)(&pVertices[0].texCoord),
            (void*)(&pVertices[0].normal),
            (void*)(&pVertices[0].tangent),
            (void*)(&pVertices[0].biTangent),
            dwVertexStride, 
            dwNumVertices, 
            pIndices, sizeof(UINT), dwNumIndices, true);

        vertexBufferId = pModelContent->AddVertexBuffer(pVertices, dwNumVertices, dwVertexStride);
        indexBufferId = pModelContent->AddIndexBuffer(pIndices, dwNumIndices, sizeof(UINT));

        // add materials
        
        // Generate textures
        /*
        ITextureContentPtr pDiffuseTexture = NULL;
        ITextureContentPtr pSpecularTexture = NULL;
        ITextureContentPtr pNormalTexture = NULL;
        pServices->CreateTextureContentFromFile(L"diffuse.dds", &pDiffuseTexture);
        pServices->GetContentContainer()->AddTextureContent(pDiffuseTexture);

        pServices->CreateTextureContentFromFile(L"specular.dds", &pSpecularTexture);
        pServices->GetContentContainer()->AddTextureContent(pSpecularTexture);
        
        pServices->CreateTextureContentFromFile(L"normal.dds", &pNormalTexture);
        pServices->GetContentContainer()->AddTextureContent(pNormalTexture);

        materialId = pModelContent->AddMaterial((pDiffuseTexture != NULL) ? "diffuse.tc" : NULL, (pSpecularTexture != NULL) ? "specular.tc" : NULL, (pNormalTexture != NULL) ? "normal.tc" : NULL);
        */

        materialId = pModelContent->AddMaterial("concrete.tc", NULL, "proto_bricks_n.tc");
        pModelContent->AddMesh("3dwModel", vertexBufferId, indexBufferId, materialId);

    Exit:

        SAFE_ARRAY_DELETE(pIndices);
        SAFE_ARRAY_DELETE(pVertices);
        
        return hr;
    }

private:

    void DebugOutputFileHeader(_3DWFILEHEADER* pheader)
    {
        DEBUG_PRINT("==== 3DW File Header ====");
        DEBUG_PRINT("Map Version   = %d", pheader->version);
        DEBUG_PRINT("flags         = %d (0x%lx)", pheader->flags, pheader->flags);
        DEBUG_PRINT("Name Count    = %d", pheader->cNames);
        DEBUG_PRINT("Name Offset   = %d", pheader->offsetNames);
        DEBUG_PRINT("Object Count  = %d", pheader->cObjects);
        DEBUG_PRINT("Object Offset = %d", pheader->offsetObjects);
        DEBUG_PRINT("=========================");
    }

    void DebugOutStringTable()
    {
        DEBUG_PRINT("==== 3DW String Table ====");
        int index = 0;
        for (UINT i = 0; i < stringTable.size(); i++)
        {
            DEBUG_PRINT("[%d] = %ws", index++,stringTable[i].c_str());
        }
        DEBUG_PRINT("==========================");
    }

    HRESULT SeekStream(IStream* pStream, DWORD dwOrigin, LONG offset)
    {
        HRESULT hr = S_OK;

        LARGE_INTEGER liMove = {0};
        ULARGE_INTEGER uliPos = {0};
        liMove.LowPart = offset;
        
        hr = pStream->Seek(liMove, dwOrigin, &uliPos);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Seeking stream");

    Exit:

        return hr;
    }

    HRESULT ReadStringTable(IStream* pStream, _3DWFILEHEADER header)
    {
        HRESULT hr = S_OK;
        ULONG   cbRead = 0;

        hr = SeekStream(pStream, STREAM_SEEK_SET, header.offsetNames);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Seeking to name table");

        // insert dummy node, to allow for 1-based index access to the table.
        stringTable.push_back(L"1-based node");
        for (LONG i = 0; i < header.cNames; i++)
        {
            std::wstring name;
            name = ReadString(pStream);
            stringTable.push_back(name);
        }

    Exit:

        return hr;
    }

    HRESULT ReadObjects(IStream* pStream, _3DWFILEHEADER header)
    {
        HRESULT hr = S_OK;
        ULONG   cbRead = 0;

        hr = SeekStream(pStream, STREAM_SEEK_SET, header.offsetObjects);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Seeking to object table");

        for (LONG i = 0; i < header.cObjects; i++)
        {
            _3DWOBJECTHEADER objectHeader = {0};
            hr = pStream->Read(&objectHeader, sizeof(objectHeader), &cbRead);
            GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading object header");

            hr = ReadObject(pStream, objectHeader);
            if (hr == E_NOTIMPL) { hr = S_OK; } // not implemented objects are skipped
            GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading object");
        }

    Exit:

        return hr;
    }

    HRESULT ReadEntity(IStream *pStream, _3DWOBJECTHEADER header)
    {
        HRESULT hr = E_NOTIMPL;
        _3DWENTITY entity;

        ULONG cbRead = 0;
        ULONG totalEntityData = 0;
        LONG numKeys = 0;

        hr = pStream->Read(&entity.flags, sizeof(entity.flags), &cbRead);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading entity flags");

        totalEntityData += cbRead;

        hr = pStream->Read(&entity.position, sizeof(entity.position), &cbRead);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading entity position");

        totalEntityData += cbRead;

        hr = pStream->Read(&numKeys, sizeof(numKeys), &cbRead);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading entity key count");
        totalEntityData += cbRead;

        // Read entity keys
        for (LONG i = 0; i < numKeys; i++)
        {
            _3DWBRUSHKEY newKey;
            LONG nameId = 0;
            LONG valueId = 0;
            
            hr = pStream->Read(&nameId, sizeof(nameId), &cbRead);
            GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading entity key name id");

            totalEntityData += cbRead;

            hr = pStream->Read(&valueId, sizeof(valueId), &cbRead);
            GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading entity key value id");

            totalEntityData += cbRead;

            newKey.name = stringTable[nameId];
            newKey.value = stringTable[valueId];

            entity.keys.push_back(newKey);
        }

        // Read group index
        hr = pStream->Read(&entity.groupIndex, sizeof(entity.groupIndex), &cbRead);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading entity group index");

        totalEntityData += cbRead;

        // Read visible group index
        hr = pStream->Read(&entity.visgroupIndex, sizeof(entity.visgroupIndex), &cbRead);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading entity visible group index");

        totalEntityData += cbRead;

        DEBUG_PRINT("Header specified data size (entity) = %d, Actual bytes read = %d", header.dataSize, totalEntityData);

        entities.push_back(entity);

    Exit:

        return hr;
    }

    HRESULT ReadObject(IStream* pStream, _3DWOBJECTHEADER header)
    {
        HRESULT hr = E_NOTIMPL;
        std::wstring objectName = stringTable[header.id];

        DEBUG_PRINT("Reading object '%ws'...", objectName.c_str());

        if (objectName == OBJECTTYPE_GROUP)
        {
            hr = ReadGroup(pStream, header);
        }
        else if (objectName == OBJECTTYPE_VISGROUP)
        {
            hr = ReadVisGroup(pStream, header);
        }
        else if (objectName == OBJECTTYPE_MATERIAL)
        {
            hr = ReadMaterial(pStream, header);
        }
        else if (objectName == OBJECTTYPE_MESHREFERENCE)
        {
            hr = ReadMeshReference(pStream, header);
        }
        else if (objectName == OBJECTTYPE_LIGHTMAP)
        {
            hr = ReadLightmap(pStream, header);
        }
        else if (objectName == OBJECTTYPE_BRUSH)
        {
            hr = ReadBrush(pStream, header);
        }
        else if (objectName == OBJECTTYPE_MESH)
        {
            hr = ReadMesh(pStream, header);
        }
        else if (objectName == OBJECTTYPE_ENTITY)
        {
            hr = ReadEntity(pStream, header);
        }
        else if (objectName == OBJECTTYPE_TERRAIN)
        {
            hr = ReadTerrain(pStream, header);
        }

        if (hr == E_NOTIMPL)
        {
            // Seek past unsupported objects;
            SeekStream(pStream, STREAM_SEEK_CUR, header.dataSize);
            DEBUG_PRINT("unsupported object type = %ws", objectName.c_str());
        }

        currentObjectIndex++;

        return hr;
    }

    HRESULT ReadMeshReference(IStream* pStream, _3DWOBJECTHEADER header)
    {
        HRESULT hr = S_OK;
        ULONG cbRead = 0;

        _3DWMESHREFERENCE meshreference;

        hr = pStream->Read(&meshreference, sizeof(meshreference) - sizeof(meshreference.objectIndex), &cbRead);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading meshreference header");

        DEBUG_PRINT("Header specified data size (meshreference) = %d, Actual bytes read = %d", header.dataSize, cbRead);

        meshreference.objectIndex = currentObjectIndex;

        meshreferences.push_back(meshreference);

    Exit:
        return hr;
    }

    HRESULT ReadMesh(IStream* pStream, _3DWOBJECTHEADER header)
    {
        HRESULT hr = S_OK;
        ULONG cbRead = 0;
        LONG totalMeshData = 0;
        _3DWMESH mesh;
        
        LONG numKeys = 0;

        hr = pStream->Read(&mesh.flags, sizeof(mesh.flags), &cbRead);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading mesh flags");

        totalMeshData += cbRead;

        hr = pStream->Read(&numKeys, sizeof(numKeys), &cbRead);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading mesh key count");
        totalMeshData += cbRead;

        // Read mesh keys
        for (LONG i = 0; i < numKeys; i++)
        {
            _3DWBRUSHKEY newKey;
            LONG nameId = 0;
            LONG valueId = 0;
            
            hr = pStream->Read(&nameId, sizeof(nameId), &cbRead);
            GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading mesh key name id");

            totalMeshData += cbRead;

            hr = pStream->Read(&valueId, sizeof(valueId), &cbRead);
            GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading mesh key value id");

            totalMeshData += cbRead;

            newKey.name = stringTable[nameId];
            newKey.value = stringTable[valueId];

            mesh.keys.push_back(newKey);
        }

        // Read group index
        hr = pStream->Read(&mesh.groupIndex, sizeof(mesh.groupIndex), &cbRead);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading mesh group index");

        totalMeshData += cbRead;

        // Read visible group index
        hr = pStream->Read(&mesh.visgroupIndex, sizeof(mesh.visgroupIndex), &cbRead);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading mesh visible group index");

        totalMeshData += cbRead;

        // Read color
        hr = pStream->Read(&mesh.color, sizeof(mesh.color), &cbRead);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading mesh color");
        totalMeshData += cbRead;

        // Read meshreference index
        hr = pStream->Read(&mesh.meshreferenceIndex, sizeof(mesh.meshreferenceIndex), &cbRead);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading mesh reference index");
        totalMeshData += cbRead;

        // Read position
        hr = pStream->Read(&mesh.position, sizeof(mesh.position), &cbRead);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading mesh position");
        totalMeshData += cbRead;

        // Read rotation
        hr = pStream->Read(&mesh.rotation, sizeof(mesh.rotation), &cbRead);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading mesh rotation");
        totalMeshData += cbRead;

        // Read scale
        if ((mesh.flags & 1) == 0)
        {
            hr = pStream->Read(&mesh.scale, sizeof(mesh.scale), &cbRead);
            GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading mesh scale");

            totalMeshData += cbRead;
        }

        // Adjust the meshreference index to be relative to the meshreferences collection
        // not the entire object list.  This will make consuming the mesh easier since
        // we are not maintaining a single object list.
        for (UINT m = 0; m < meshreferences.size(); m++)
        {
            if (meshreferences[m].objectIndex == mesh.meshreferenceIndex)
            {
                mesh.meshreferenceIndex = m;
                break;
            }
        }

        // Read mesh data
        for (BYTE i = 0; i < meshreferences[mesh.meshreferenceIndex].limbCount; i++)
        {
            _3DWMESHDATA meshData;

            hr = pStream->Read(&meshData.materialIndex, sizeof(meshData.materialIndex), &cbRead);
            GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading mesh data material index");

            totalMeshData += cbRead;

            // Read vertices color data
            if (mesh.flags & 16)
            {
                SHORT numVertices = 0;
                hr = pStream->Read(&numVertices, sizeof(numVertices), &cbRead);
                GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading mesh vertices count");

                totalMeshData += cbRead;

                for (SHORT v = 0; v < numVertices; v++)
                {
                    _3DWCOLOR_24BIT color;
                    hr = pStream->Read(&color, sizeof(color), &cbRead);
                    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading mesh vertex color");

                    totalMeshData += cbRead;

                    meshData.vertexColors.push_back(color);
                }
                mesh.meshdata.push_back(meshData);
            }
        }

        DEBUG_PRINT("Header specified data size (mesh) = %d, Actual bytes read = %d", header.dataSize, totalMeshData);

        meshes.push_back(mesh);

    Exit:
        return hr;
    }

    HRESULT ReadLightmap(IStream* pStream, _3DWOBJECTHEADER header)
    {
        HRESULT hr = S_OK;
        ULONG cbRead = 0;
        LONG totallightmapData = 0;
        BYTE resolution = 0;
        _3DWLIGHTMAP lightmap;

        hr = pStream->Read(&lightmap.flags, sizeof(lightmap.flags), &cbRead);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading lightmap flags");

        totallightmapData += cbRead;

        hr = pStream->Read(&resolution, sizeof(resolution), &cbRead);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading lightmap resolution");

        // Calculate proper lightmap resolution
        lightmap.resolution = 2;
        for (int i = 0; i < resolution - 1; i++) lightmap.resolution *= 2;

        totallightmapData += cbRead;

        hr = pStream->Read(&lightmap.format, sizeof(lightmap.format), &cbRead);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading lightmap format");

        totallightmapData += cbRead;
        
#ifdef MAKE_32BIT_LIGHTMAPS
        // Allocate lightmap data
        lightmap.cbData = lightmap.resolution * lightmap.resolution * 4; // ARGB
        lightmap.pData = new BYTE[lightmap.cbData];
        if (lightmap.pData != NULL)
        {
            DWORD cb24BitData = (lightmap.resolution * lightmap.resolution * 3);
            BYTE* p24BitData = lightmap.pData + (lightmap.cbData - cb24BitData);

            // Read 24-bit data into our 32-bit sized memory buffer
            // packed at the end...
            hr = pStream->Read(p24BitData, cb24BitData, &cbRead);
            GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading lightmap pixel data");

            totallightmapData += cbRead;
        }
#else
        // Allocate lightmap data
        lightmap.cbData = lightmap.resolution * lightmap.resolution * 3;
        lightmap.pData = new BYTE[lightmap.cbData];
        if (lightmap.pData != NULL)
        {
            hr = pStream->Read(lightmap.pData, lightmap.cbData, &cbRead);
            GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading lightmap pixel data");

            totallightmapData += cbRead;
        }
#endif

        DEBUG_PRINT("Header specified data size (lightmap) = %d, Actual bytes read = %d", header.dataSize, totallightmapData);

#ifdef MAKE_32BIT_LIGHTMAPS
        
        DEBUG_PRINT("Converting 24-bit lightmap data into 32-bit lightmap data...");
        // Shift the 24-bit data down to create ARGB

        DWORD cb24BitData = (lightmap.resolution * lightmap.resolution * 3);
        BYTE* p24BitData = lightmap.pData + (lightmap.cbData - cb24BitData);
        BYTE* p32BitData = lightmap.pData;
        
        for (DWORD i = 0; i < (DWORD)(lightmap.resolution * lightmap.resolution); i++)
        {
            BYTE* p32 = p32BitData;
            BYTE* p24 = p24BitData;

            p32[3] = 255;
            p32[0] = p24[0];
            p32[1] = p24[1];
            p32[2] = p24[2];

            p32BitData += 4;
            p24BitData += 3;
        }

        DEBUG_PRINT("Finished Converting 24-bit lightmap data into 32-bit lightmap data...");
#endif

        lightmaps.push_back(lightmap);

    Exit:
        return hr;
    }

    HRESULT ReadTerrain(IStream* pStream, _3DWOBJECTHEADER header)
    {
        HRESULT hr = S_OK;
        ULONG cbRead = 0;
        LONG totalTerrainData = 0;
        _3DWTERRAIN_DATA terrain;
        
        hr = pStream->Read(&terrain.header, sizeof(terrain.header), &cbRead);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading terrain header");

        totalTerrainData += cbRead;

        terrain.cblightmap = terrain.header.resolution * terrain.header.resolution * 3;
        terrain.plightmapData = new BYTE[terrain.cblightmap];

        hr = pStream->Read(terrain.plightmapData, terrain.cblightmap, &cbRead);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading terrain lightmap data");

        totalTerrainData += cbRead;

        // Allocate heightmap data
        terrain.cbheightmap = (terrain.header.resolution + 1) * (terrain.header.resolution + 1) * sizeof(FLOAT);
        terrain.pheightData = new FLOAT[(terrain.header.resolution + 1) * (terrain.header.resolution + 1)];
        
        // Read height data
        hr = pStream->Read(terrain.pheightData, terrain.cbheightmap, &cbRead);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading terrain height data");

        totalTerrainData += cbRead;

        // Read layers
        for (LONG i = 0; i < terrain.header.layercount; i++)
        {
            _3DWTERRAINLAYER layer;
            hr = pStream->Read(&layer.nameId, sizeof(layer.nameId), &cbRead);
            GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading terrain layer name id");

            totalTerrainData += cbRead;

            hr = pStream->Read(&layer.materialIndex, sizeof(layer.materialIndex), &cbRead);
            GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading terrain layer material index");

            totalTerrainData += cbRead;

            layer.cbData = 0;
            layer.pData = NULL;

            // Read alpha for all layers after initial layer
            if (i > 0)
            {
                // Allocate layer alpha data
                layer.cbData = (terrain.header.resolution) * (terrain.header.resolution);
                layer.pData = new BYTE[layer.cbData];

                // Read alpha
                hr = pStream->Read(layer.pData, layer.cbData, &cbRead);
                GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading terrain alpha data");

                totalTerrainData += cbRead;
            }

            terrain.layers.push_back(layer);
        }

        DEBUG_PRINT("Header specified data size (terrain) = %d, Actual bytes read = %d", header.dataSize, totalTerrainData);

        terrains.push_back(terrain);

    Exit:
        return hr;
    }

    HRESULT ReadGroup(IStream* pStream, _3DWOBJECTHEADER header)
    {
        HRESULT hr = S_OK;
        ULONG cbRead = 0;
        _3DWGROUPHEADER group = {0};
        hr = pStream->Read(&group, sizeof(group), &cbRead);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading group header");

        DEBUG_PRINT("Header specified data size (group) = %d, Actual bytes read = %d", header.dataSize, cbRead);

        groups.push_back(group);

    Exit:
        return hr;
    }

    HRESULT ReadVisGroup(IStream* pStream, _3DWOBJECTHEADER header)
    {
        HRESULT hr = S_OK;
        ULONG cbRead = 0;
        _3DWVISGROUPHEADER visgroup = {0};
        hr = pStream->Read(&visgroup, sizeof(visgroup), &cbRead);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading visgroup header");

        DEBUG_PRINT("Header specified data size (visgroup) = %d, Actual bytes read = %d", header.dataSize, cbRead);

        visgroups.push_back(visgroup);

    Exit:
        return hr;
    }

    HRESULT ReadMaterial(IStream* pStream, _3DWOBJECTHEADER header)
    {
        HRESULT hr = S_OK;
        ULONG cbRead = 0;
        _3DWMATERIAL material = {0};
        hr = pStream->Read(&material, sizeof(material), &cbRead);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading material header");

        DEBUG_PRINT("Header specified data size (material) = %d, Actual bytes read = %d", header.dataSize, cbRead);

        materials.push_back(material);

    Exit:
        return hr;

    }

    HRESULT ReadBrush(IStream* pStream, _3DWOBJECTHEADER header)
    {
        HRESULT hr = S_OK;
        _3DWBRUSH brush = {0};
        _3DWBRUSHHEADER brushHeader = {0};

        LONG totalBrushData = 0;
        ULONG cbRead = 0;
        BYTE numVertices = 0;
        BYTE numFaces = 0;

        // Read brush header
        hr = pStream->Read(&brushHeader, sizeof(brushHeader), &cbRead);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading brush header");

        totalBrushData += cbRead;

        // Read brush keys
        for (int i = 0; i < brushHeader.keyCount; i++)
        {
            _3DWBRUSHKEY newKey;
            LONG nameId = 0;
            LONG valueId = 0;
            
            hr = pStream->Read(&nameId, sizeof(nameId), &cbRead);
            GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading brush key name id");

            totalBrushData += cbRead;

            hr = pStream->Read(&valueId, sizeof(valueId), &cbRead);
            GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading brush key value id");

            totalBrushData += cbRead;

            // Convert name and value ids into proper string values
            newKey.name = stringTable[nameId];
            newKey.value = stringTable[valueId];

            brush.keys.push_back(newKey);
        }

        // Read group index
        hr = pStream->Read(&brush.groupIndex, sizeof(brush.groupIndex), &cbRead);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading brush group index");

        totalBrushData += cbRead;

        // Read visible group index
        hr = pStream->Read(&brush.visgroupIndex, sizeof(brush.visgroupIndex), &cbRead);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading brush visible group index");

        totalBrushData += cbRead;

        // Read brush color
        hr = pStream->Read(&brush.color, sizeof(brush.color), &cbRead);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading brush color");

        totalBrushData += cbRead;

        // Read vertex count
        hr = pStream->Read(&numVertices, sizeof(numVertices), &cbRead);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading vertices count");

        totalBrushData += cbRead;

        // Read vertices
        for (BYTE i = 0; i < numVertices; i++)
        {
            _3DWVECTOR3 vertex = {0};
            hr = pStream->Read(&vertex, sizeof(vertex), &cbRead);
            GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading vertex");

            totalBrushData += cbRead;

            brush.vertices.push_back(vertex);
        }

        // Read face count
        hr = pStream->Read(&numFaces, sizeof(numFaces), &cbRead);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading faces count");

        totalBrushData += cbRead;

        // Read faces
        for (BYTE i = 0; i < numFaces; i++)
        {
            _3DWFACE face = {0};
            hr = pStream->Read(&face.header, sizeof(face.header), &cbRead);
            GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading face header");

            totalBrushData += cbRead;

            // If lightmap data exists, read lightmap index value, otherwise the value
            // should be set to -1
            if (face.header.flags & 16)
            {
                hr = pStream->Read(&face.lightMapIndex, sizeof(face.lightMapIndex), &cbRead);
                GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading face lightmap index");

                totalBrushData += cbRead;
            }
            else
            {
                face.lightMapIndex = -1; // no lightmap
            }

            // Read indices count
            BYTE numIndices = 0;
            hr = pStream->Read(&numIndices, sizeof(numIndices), &cbRead);
            GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading face indices count");

            totalBrushData += cbRead;

            // Read indices
            for (BYTE i = 0; i < numIndices; i++)
            {
                _3DWINDEX indice = {0};
                hr = pStream->Read(&indice.vertexId, sizeof(indice.vertexId), &cbRead);
                GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading face indice id");

                totalBrushData += cbRead;

                hr = pStream->Read(&indice.texCoord1, sizeof(indice.texCoord1), &cbRead);
                GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading face indice texture coordinate 1");

                totalBrushData += cbRead;

                // If lightmap data exists, read secondary texture coordinate, otherwise
                // the secondary coordinate will remain (0.0f,0.0f).
                if (face.header.flags & 16)
                {
                    hr = pStream->Read(&indice.texCoord2, sizeof(indice.texCoord2), &cbRead);
                    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Reading face indice texture coordinate 2");

                    totalBrushData += cbRead;
                }

                face.indices.push_back(indice);
            }
            brush.faces.push_back(face);
        }

        brushes.push_back(brush);

    Exit:

        DEBUG_PRINT("Header specified data size (brush) = %d, Actual bytes read = %d", header.dataSize, totalBrushData);

        return hr;
    }

    std::wstring ReadString(IStream* pStream)
    {
        std::wstring s;

        HRESULT hr     = S_OK;
        BYTE    ch     = 0;
        ULONG   cbRead = 0;

        do
        {
            ch = 0;
            hr = pStream->Read(&ch, sizeof(ch), &cbRead);
            if FAILED(hr)
            {
                s += L"_invalid";
                break;
            }
            // avoid writing null terminator
            if (ch != 0)
            {
                s += ch;
            }
        } while(ch != 0);

        DEBUG_PRINT("Read string '%ws'...", s.c_str());

        return s;
    }

    void TriangulateFace(_3DWFACE& face, bool swapWindingOrder)
    {
        size_t numVertices = face.indices.size();
        DWORD missingVertices = numVertices % 3;
        if (missingVertices != 0)
        {
            _3DWFACE tface;
            tface.header = face.header;
            tface.lightMapIndex = face.lightMapIndex;
            for (UINT i = 0; i < face.indices.size() - 2; i++)
            {
                if (swapWindingOrder)
                {
                    tface.indices.push_back(face.indices[i + 2]);
                    tface.indices.push_back(face.indices[i + 1]);
                    tface.indices.push_back(face.indices[0]);
                }
                else
                {
                    tface.indices.push_back(face.indices[0]);
                    tface.indices.push_back(face.indices[i + 1]);
                    tface.indices.push_back(face.indices[i + 2]);
                }
            }

            face.indices.clear();
            for (UINT i = 0; i < tface.indices.size(); i++)
            {
                face.indices.push_back(tface.indices[i]);
            }
        }

        //DEBUG_PRINT("Face had %d vertices, now has %d vertices", totalPolyCount, face.indices.size());
    }

private:
    LONG currentObjectIndex;

public:
    std::vector<std::wstring>       stringTable;
    std::vector<_3DWBRUSH>          brushes;
    std::vector<_3DWMATERIAL>       materials;
    std::vector<_3DWGROUPHEADER>    groups;
    std::vector<_3DWVISGROUPHEADER> visgroups;
    std::vector<_3DWENTITY>         entities;
    std::vector<_3DWLIGHTMAP>       lightmaps;
    std::vector<_3DWMESHREFERENCE>  meshreferences;
    std::vector<_3DWTERRAIN_DATA>   terrains;
    std::vector<_3DWMESH>           meshes;
};
