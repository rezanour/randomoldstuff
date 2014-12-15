#pragma once

HRESULT LoadObjContent(LPCWSTR filePath, IContentPluginServices* pServices);

typedef struct _ATTRIBUTE_RANGE
{
    UINT  AttribId;
    UINT  FaceStart;
    UINT  FaceCount;
    UINT  VertexStart;
    UINT  VertexCount;
} ATTRIBUTE_RANGE;

// Used for a hashtable vertex cache when creating the mesh from a .obj file
struct CacheEntry
{
    UINT index;
    CacheEntry* pNext;
};

// Material properties per mesh subset
struct Material
{
    WCHAR   strName[MAX_PATH];
    XMFLOAT3 vAmbient;
    XMFLOAT3 vDiffuse;
    XMFLOAT3 vSpecular;
    int nShininess;
    float fAlpha;
    bool bSpecular;
    WCHAR strTexture[MAX_PATH];
};

class CLoaderOBJFile
{
public:
    CLoaderOBJFile();
    virtual ~CLoaderOBJFile();

    HRESULT Load(LPCWSTR szFileName, IModelContentPtr& pModelContent );

private:
    HRESULT InitializeModelContentFromGeometry(IModelContentPtr& pModelContent);

    void Destroy();
    UINT GetNumMaterials() const { return (UINT)m_Materials.size(); }
    Material* GetMaterial( UINT iMaterial ) { return m_Materials[ iMaterial ]; }
    HRESULT LoadGeometryFromOBJ( const WCHAR* strFilename );
    HRESULT LoadMaterialsFromMTL( const WCHAR* strFileName );
    void InitMaterial( Material* pMaterial );
    DWORD AddVertex( UINT hash, XMPosTexNormVertex* pVertex );
    void DeleteCache();

    std::vector<CacheEntry*>        m_VertexCache;     // Hashtable cache for locating duplicate vertices
    std::vector<XMPosTexNormVertex> m_Vertices; // Filled and copied to the vertex buffer
    std::vector<DWORD>              m_Indices;           // Filled and copied to the index buffer
    std::vector<DWORD>              m_Attributes;        // Filled and copied to the attribute buffer
    std::vector<Material*>          m_Materials;     // Holds material properties per subset
};