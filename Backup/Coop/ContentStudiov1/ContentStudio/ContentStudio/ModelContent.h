#pragma once

#include "BufferObject.h"

#define INVALID_MESH_CONTENT_ID (UINT)-1

struct ModelContentBufferDescription
{
    UINT numElements;
    UINT stride;
    CopiedBufferPtr pBuffer;
};

#pragma pack(push,1)
struct MeshInfo
{
    UINT numSubMeshIds;
    UINT subMeshIds[32];
    UINT vertexBufferId;
    UINT indexBufferId;
    UINT materialId;
};
#pragma pack(pop)

struct MaterialInfo
{
    std::string diffuse;
    std::string specular;
    std::string normal;
};

struct ModelContentMaterialDescription
{
    MaterialInfo info;
};

class IModelContent
{
public:
    virtual UINT AddVertexBuffer(void* pbuffer, UINT numVertices, UINT stride) = 0;
    virtual UINT AddIndexBuffer(void* pbuffer, UINT numIndices, UINT stride) = 0;
    virtual UINT AddMaterial(LPCSTR diffuse, LPCSTR specular, LPCSTR normal) = 0;
    virtual HRESULT AddMesh(LPCSTR name, UINT vertexBufferId, UINT indexBufferId, UINT materialId) = 0;
    virtual HRESULT AddMeshPart(LPCSTR name, UINT vertexBufferId, UINT indexBufferId, UINT materialId) = 0;
    virtual HRESULT GetMesh(LPCSTR name, MeshInfo& meshInfo) = 0;
    virtual HRESULT GetMeshPart(UINT meshInfoId, MeshInfo& meshInfo) = 0;
    virtual void GetMeshNames(std::vector<std::string>& meshNames) = 0;
    virtual HRESULT GetVertexBuffer(UINT id, ModelContentBufferDescription& desc) = 0;
    virtual HRESULT GetIndexBuffer(UINT id, ModelContentBufferDescription& desc) = 0;
    virtual HRESULT GetMaterial(UINT id, ModelContentMaterialDescription& desc) = 0;
    virtual HRESULT SetMaterial(UINT id, ModelContentMaterialDescription& desc) = 0;
    virtual HRESULT SaveToFile(LPCWSTR filePath) = 0;
    virtual HRESULT LoadFromFile(LPCWSTR filePath) = 0;
};

typedef std::shared_ptr<IModelContent> IModelContentPtr;