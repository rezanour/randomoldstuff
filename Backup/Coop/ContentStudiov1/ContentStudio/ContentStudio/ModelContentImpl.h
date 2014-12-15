#pragma once

#define MESHPART_PREFIX "__mp__"

#pragma pack(push,1)
struct MODELCONTENTFILEHEADER
{
    DWORD dwSize;
    DWORD numVertexBuffers;
    DWORD numIndexBuffers;
    DWORD numMaterials;
    DWORD numMeshes;
    DWORD offsetVertexBuffers;
    DWORD offsetIndexBuffers;
    DWORD offsetMaterials;
    DWORD offsetMeshes;
};

struct MODELCONTENTBUFFERHEADER
{
    DWORD numElements;
    DWORD stride;
    // [ numElements * stride = data size ]
};

struct MODELCONTENTMATERIALHEADER
{
    CHAR diffuse[MAX_PATH];
    CHAR specular[MAX_PATH];
    CHAR normal[MAX_PATH];
};

struct MODELCONTENTMESHHEADER
{
    CHAR     name[MAX_PATH];
    MeshInfo info;
};

#pragma pack(pop)

class CModelContent : public IModelContent
{
public:
    CModelContent();
    virtual ~CModelContent();

public:
    UINT AddVertexBuffer(void* pbuffer, UINT numVertices, UINT stride);
    UINT AddIndexBuffer(void* pbuffer, UINT numIndices, UINT stride);
    UINT AddMaterial(LPCSTR diffuse, LPCSTR specular, LPCSTR normal);
    HRESULT AddMesh(LPCSTR name, UINT vertexBufferId, UINT indexBufferId, UINT materialId);
    HRESULT AddMeshPart(LPCSTR name, UINT vertexBufferId, UINT indexBufferId, UINT materialId);
    HRESULT GetMesh(LPCSTR name, MeshInfo& meshInfo);
    HRESULT GetMeshPart(UINT meshInfoId, MeshInfo& meshInfo);
    void GetMeshNames(std::vector<std::string>& meshNames);
    HRESULT GetVertexBuffer(UINT id, ModelContentBufferDescription& desc);
    HRESULT GetIndexBuffer(UINT id, ModelContentBufferDescription& desc);
    HRESULT GetMaterial(UINT id, ModelContentMaterialDescription& desc);
    HRESULT SetMaterial(UINT id, ModelContentMaterialDescription& desc);

    HRESULT SaveToFile(LPCWSTR filePath);
    HRESULT LoadFromFile(LPCWSTR filePath);

    static IModelContentPtr FromFile(LPCWSTR filePath);

private:
    bool IsValidMesh(MeshInfo meshInfo);
    DWORD GetTotalVerticesBufferByteSize();
    DWORD GetTotalIndicesBufferByteSize();
    DWORD GetTotalMaterialsBytesSize();
    DWORD GetTotalMeshesByteSize();
    UINT GetNextMeshPartId() { return m_meshPartId++; }

private:
    std::map<std::string, MeshInfo> m_meshMap;
    std::vector<ModelContentBufferDescription> m_vertexBufferArray;
    std::vector<ModelContentBufferDescription> m_indexBufferArray;
    std::vector<MaterialInfo> m_materialArray;
    UINT m_meshPartId;
};

typedef std::shared_ptr<CModelContent> ModelContentPtr;
