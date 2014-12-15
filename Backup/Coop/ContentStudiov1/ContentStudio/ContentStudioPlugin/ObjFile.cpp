#include "stdafx.h"

HRESULT LoadObjContent(LPCWSTR filePath, IContentPluginServices* pServices)
{
    DEBUG_PRINT("Loading Wavefront .obj model content...");
    HRESULT hr = S_OK;
    CLoaderOBJFile file;
    IModelContentPtr pModelContent;

    hr = pServices->CreateModelContent(&pModelContent);
    if (SUCCEEDED(hr))
    {
        hr = file.Load(filePath, pModelContent);
        if (SUCCEEDED(hr))
        {
            IContentContainerPtr pContent = pServices->GetContentContainer();
            if (pContent)
            {
                hr = pContent->AddModelContent(pModelContent);
            }
        }
    }
    return hr;
}

CLoaderOBJFile::CLoaderOBJFile()
{

}

CLoaderOBJFile::~CLoaderOBJFile()
{
    Destroy();
}

HRESULT CLoaderOBJFile::Load(LPCWSTR szFileName, IModelContentPtr& pModelContent )
{
    HRESULT hr = E_FAIL;
    
    hr = LoadGeometryFromOBJ(szFileName);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Loading OBJ raw data");

    hr = InitializeModelContentFromGeometry(pModelContent);

Exit:

    return hr;
}

HRESULT CLoaderOBJFile::InitializeModelContentFromGeometry(IModelContentPtr& pModelContent)
{
    HRESULT hr = S_OK;

    DWORD dwNumVertices = 0;
    DWORD dwNumFaces = 0;
    DWORD dwNumIndices = 0;

    std::vector<XMPosNormTanBiTexVertex> vertices;
    std::vector<DWORD> indices;

    int numRawVertices = m_Vertices.size();
    for (int i = 0; i < numRawVertices; i++)
    {
        XMPosNormTanBiTexVertex v;
        v.position.x = m_Vertices[i].position.x;
        v.position.y = m_Vertices[i].position.y;
        v.position.z = m_Vertices[i].position.z;
        v.texCoord.x = m_Vertices[i].texCoord.x;
        v.texCoord.y = m_Vertices[i].texCoord.y;
        v.normal.x = m_Vertices[i].normal.x;
        v.normal.y = m_Vertices[i].normal.y;
        v.normal.z = m_Vertices[i].normal.z;
        vertices.push_back(v);
    }

    int numRawIndices = m_Indices.size();
    for (int i = 0; i < numRawIndices; i++)
    {
        indices.push_back(m_Indices[i]);
    }
    
    dwNumVertices = vertices.size();
    dwNumIndices = indices.size();

    FLOAT largestTextureCoordinate = 1.0f;
    FLOAT smallestTextureCoordinate = 0.0f;

    // Create vertex buffer
    DWORD dwVertexStride = sizeof(XMPosNormTanBiTexVertex);
    XMPosNormTanBiTexVertex* pVertices = new XMPosNormTanBiTexVertex[dwNumVertices];
    for (DWORD i = 0; i < dwNumVertices; i++)
    {
        pVertices[i].position.x = vertices[i].position.x;
        pVertices[i].position.y = vertices[i].position.y;
        pVertices[i].position.z = vertices[i].position.z;
        pVertices[i].normal.x = vertices[i].normal.x;
        pVertices[i].normal.y = vertices[i].normal.y;
        pVertices[i].normal.z = vertices[i].normal.z;
        pVertices[i].texCoord.x = vertices[i].texCoord.x;
        pVertices[i].texCoord.y = vertices[i].texCoord.y;

        if (pVertices[i].texCoord.x > largestTextureCoordinate)
        {
            largestTextureCoordinate = pVertices[i].texCoord.x;
        }

        if (pVertices[i].texCoord.y > largestTextureCoordinate)
        {
            largestTextureCoordinate = pVertices[i].texCoord.y;
        }

        if (pVertices[i].texCoord.x < smallestTextureCoordinate)
        {
            smallestTextureCoordinate = pVertices[i].texCoord.x;
        }

        if (pVertices[i].texCoord.y < smallestTextureCoordinate)
        {
            smallestTextureCoordinate = pVertices[i].texCoord.y;
        }
    }

    // Create indices buffer
    UINT* pIndices = new UINT[dwNumIndices];
    for (DWORD i = 0; i < dwNumIndices; i++)
    {
        pIndices[i] = (UINT)indices[i];
    }

    // Calculate normals
    /*hr = Graphics::GenerateVertexNormals(
        (void*)(&pVertices[0].position),  
        (void*)(&pVertices[0].normal), dwVertexStride, 
        dwNumVertices, 
        pIndices, sizeof(UINT), dwNumIndices, false);*/

    hr = Graphics::GenerateTangentSpaceLH(
                    (void*)(&pVertices[0].position),
                    (void*)(&pVertices[0].texCoord),
                    (void*)(&pVertices[0].normal),
                    (void*)(&pVertices[0].tangent),
                    (void*)(&pVertices[0].biTangent),
                    dwVertexStride, 
                    dwNumVertices, 
                    pIndices, sizeof(UINT), dwNumIndices, true);

    pModelContent->AddVertexBuffer(pVertices, dwNumVertices, dwVertexStride);

    pModelContent->AddIndexBuffer(pIndices, dwNumIndices, sizeof(UINT));

    //if (m_Materials.size() > 0)
    //{
    //    int xx = 0;
    //    xx++;
    //}
    //else
    //{
        pModelContent->AddMaterial("concrete", NULL, "proto_bricks_n");
    //}

    pModelContent->AddMesh("objModel", 0, 0, 0);

//Exit:

    SAFE_ARRAY_DELETE(pIndices);
    SAFE_ARRAY_DELETE(pVertices);

    return hr;
}

void CLoaderOBJFile::Destroy()
{
    for ( UINT iMaterial = 0; iMaterial < m_Materials.size(); ++iMaterial )
    {
        Material *pMaterial = m_Materials[ iMaterial ];
        SAFE_DELETE( pMaterial );
    }

    m_Materials.clear();
    m_Vertices.clear();
    m_Indices.clear();
    m_Attributes.clear();

    DeleteCache();
}

//--------------------------------------------------------------------------------------
HRESULT CLoaderOBJFile::LoadGeometryFromOBJ( const WCHAR* strFileName )
{
    WCHAR strMaterialFilename[MAX_PATH] = {0};
    HRESULT hr;

    // Create temporary storage for the input data. Once the data has been loaded into
    // a reasonable format we can create a D3DXMesh object and load it with the mesh data.
    std::vector<XMFLOAT3> Positions;
    std::vector<XMFLOAT2> TexCoords;
    std::vector<XMFLOAT3> Normals;

    // The first subset uses the default material
    Material* pMaterial = new Material();
    if( pMaterial == NULL )
        return E_OUTOFMEMORY;

    InitMaterial( pMaterial );
    wcscpy_s( pMaterial->strName, MAX_PATH - 1, L"default" );
    m_Materials.push_back( pMaterial );

    DWORD dwCurSubset = 0;

    // File input
    WCHAR strCommand[256] = {0};
    std::wifstream InFile( strFileName );
    if( !InFile )
        return E_FAIL;

    for(; ; )
    {
        InFile >> strCommand;
        if( !InFile )
            break;

        if( 0 == wcscmp( strCommand, L"#" ) )
        {
            // Comment
        }
        else if( 0 == wcscmp( strCommand, L"v" ) )
        {
            // Vertex Position
            float x, y, z;
            InFile >> x >> y >> z;
            Positions.push_back( XMFLOAT3( x, y, z ) );
        }
        else if( 0 == wcscmp( strCommand, L"vt" ) )
        {
            // Vertex TexCoord
            float u, v;
            InFile >> u >> v;
            TexCoords.push_back( XMFLOAT2( u, v ) );
        }
        else if( 0 == wcscmp( strCommand, L"vn" ) )
        {
            // Vertex Normal
            float x, y, z;
            InFile >> x >> y >> z;
            Normals.push_back( XMFLOAT3( x, y, z ) );
        }
        else if( 0 == wcscmp( strCommand, L"f" ) )
        {
            // Face
            UINT iPosition = 0;
            UINT iTexCoord = 0;
            UINT iNormal = 0;
            UINT iNumVertices = 0;
            char peekValue = ' ';
            bool isValidFaceVertex = false;
            std::vector<XMPosTexNormVertexId> faceVertices;

            // The original SDK sample code assumed that OBJ files would be properly
            // triangulated. (faces are defined as triangles (3 vertices) )  
            // This has been changed to read all face vertices until it reaches the end
            // of the face definition.
            do
            {
                XMPosTexNormVertexId vInfo;
                ZeroMemory( &vInfo.vertex, sizeof( XMPosTexNormVertex ) );

                // OBJ format uses 1-based arrays
                InFile >> iPosition;
                vInfo.vertexId = iPosition - 1;
                vInfo.vertex.position = Positions[ iPosition - 1 ];

                if( '/' == InFile.peek() )
                {
                    InFile.ignore();

                    if( '/' != InFile.peek() )
                    {
                        // Optional texture coordinate
                        InFile >> iTexCoord;
                        vInfo.vertex.texCoord = TexCoords[ iTexCoord - 1 ];
                    }

                    if( '/' == InFile.peek() )
                    {
                        InFile.ignore();

                        // Optional vertex normal
                        InFile >> iNormal;
                        vInfo.vertex.normal = Normals[ iNormal - 1 ];
                    }
                }

                // Collect newly read face information
                faceVertices.push_back(vInfo);
                iNumVertices++;

                // Move to next vertex definition on the face
                if( ' ' == InFile.peek() )
                {
                    InFile.ignore();
                }
                else
                {
                    break;
                }

            } while ('\n' != InFile.peek());

            // Triangulate face
            TriangulateFace(faceVertices, false);

            // Add newly triangulated faces to vertices and indices list
            for (UINT f = 0; f < faceVertices.size(); f++)
            {
                // If a duplicate vertex doesn't exist, add this vertex to the Vertices
                // list. Store the index in the Indices array. The Vertices and Indices
                // lists will eventually become the Vertex Buffer and Index Buffer for
                // the mesh.
                DWORD index = AddVertex( faceVertices[f].vertexId, &faceVertices[f].vertex );
                if ( index == (DWORD)-1 )
                    return E_OUTOFMEMORY;

                m_Indices.push_back( index );
            }
            m_Attributes.push_back( dwCurSubset );
        }
        else if( 0 == wcscmp( strCommand, L"mtllib" ) )
        {
            // Material library
            InFile >> strMaterialFilename;
        }
        else if( 0 == wcscmp( strCommand, L"usemtl" ) )
        {
            // Material
            WCHAR strName[MAX_PATH] = {0};
            InFile >> strName;

            bool bFound = false;
            for( UINT iMaterial = 0; iMaterial < m_Materials.size(); iMaterial++ )
            {
                Material* pCurMaterial = m_Materials[ iMaterial ];
                if( 0 == wcscmp( pCurMaterial->strName, strName ) )
                {
                    bFound = true;
                    dwCurSubset = iMaterial;
                    break;
                }
            }

            if( !bFound )
            {
                pMaterial = new Material();
                if( pMaterial == NULL )
                    return E_OUTOFMEMORY;

                dwCurSubset = m_Materials.size();

                InitMaterial( pMaterial );
                wcscpy_s( pMaterial->strName, MAX_PATH - 1, strName );

                m_Materials.push_back( pMaterial );
            }
        }
        else
        {
            // Unimplemented or unrecognized command
        }

        InFile.ignore( 1000, '\n' );
    }

    // Cleanup
    InFile.close();
    DeleteCache();

    // If an associated material file was found, read that in as well.
    if( strMaterialFilename[0] )
    {
        // create the full material path, based on the .obj path
        std::wstring filePath = strFileName;
        size_t pos = filePath.find_last_of(L"\\");
        filePath = filePath.substr(0, pos);
        filePath += L"\\" + std::wstring(strMaterialFilename);

        hr = LoadMaterialsFromMTL( filePath.c_str() );
    }

    return S_OK;
}


//--------------------------------------------------------------------------------------
DWORD CLoaderOBJFile::AddVertex( UINT hash, XMPosTexNormVertex* pVertex )
{
    // If this vertex doesn't already exist in the Vertices list, create a new entry.
    // Add the index of the vertex to the Indices list.
    bool bFoundInList = false;
    DWORD index = 0;

    // Since it's very slow to check every element in the vertex list, a hashtable stores
    // vertex indices according to the vertex position's index as reported by the OBJ file
    if( ( UINT )m_VertexCache.size() > hash )
    {
        CacheEntry* pEntry = m_VertexCache[ hash ];
        while( pEntry != NULL )
        {
            XMPosTexNormVertex* pCacheVertex = m_Vertices.data() + pEntry->index;

            // If this vertex is identical to the vertex already in the list, simply
            // point the index buffer to the existing vertex
            if( 0 == memcmp( pVertex, pCacheVertex, sizeof( XMPosTexNormVertex ) ) )
            {
                bFoundInList = true;
                index = pEntry->index;
                break;
            }

            pEntry = pEntry->pNext;
        }
    }

    // Vertex was not found in the list. Create a new entry, both within the Vertices list
    // and also within the hashtable cache
    if( !bFoundInList )
    {
        // Add to the Vertices list
        index = m_Vertices.size();
        m_Vertices.push_back( *pVertex );

        // Add this to the hashtable
        CacheEntry* pNewEntry = new CacheEntry;

        if( pNewEntry == NULL )
            return (DWORD)-1;

        pNewEntry->index = index;
        pNewEntry->pNext = NULL;

        // Grow the cache if needed
        while( ( UINT )m_VertexCache.size() <= hash )
        {
            m_VertexCache.push_back( NULL );
        }

        // Add to the end of the linked list
        CacheEntry* pCurEntry = m_VertexCache[ hash ];
        if( pCurEntry == NULL )
        {
            // This is the head element
            m_VertexCache[hash] = pNewEntry;
        }
        else
        {
            // Find the tail
            while( pCurEntry->pNext != NULL )
            {
                pCurEntry = pCurEntry->pNext;
            }

            pCurEntry->pNext = pNewEntry;
        }
    }

    return index;
}


//--------------------------------------------------------------------------------------
void CLoaderOBJFile::DeleteCache()
{
    // Iterate through all the elements in the cache and subsequent linked lists
    for( UINT i = 0; i < m_VertexCache.size(); i++ )
    {
        CacheEntry* pEntry = m_VertexCache[ i ];
        while( pEntry != NULL )
        {
            CacheEntry* pNext = pEntry->pNext;
            SAFE_DELETE( pEntry );
            pEntry = pNext;
        }
    }

    m_VertexCache.clear();
}


//--------------------------------------------------------------------------------------
HRESULT CLoaderOBJFile::LoadMaterialsFromMTL( const WCHAR* strFileName )
{
    HRESULT hr = S_OK;

    std::wifstream InFile( strFileName );
    if (!InFile)
    {
        hr = E_FAIL;
    }

    if FAILED(hr)
    {
        DEBUG_PRINT("Materials description file '%ws' was not found...skipping materials processing", strFileName);
        return S_OK;
    }
    
    // File input
    WCHAR strCommand[256] = {0};
    
    if( !InFile )
        return E_FAIL;

    Material* pMaterial = NULL;

    for(; ; )
    {
        InFile >> strCommand;
        if( !InFile )
            break;

        if( 0 == wcscmp( strCommand, L"newmtl" ) )
        {
            // Switching active materials
            WCHAR strName[MAX_PATH] = {0};
            InFile >> strName;

            pMaterial = NULL;
            for( UINT i = 0; i < m_Materials.size(); i++ )
            {
                Material* pCurMaterial = m_Materials[ i ];
                if( 0 == wcscmp( pCurMaterial->strName, strName ) )
                {
                    pMaterial = pCurMaterial;
                    break;
                }
            }
        }

        // The rest of the commands rely on an active material
        if( pMaterial == NULL )
            continue;

        if( 0 == wcscmp( strCommand, L"#" ) )
        {
            // Comment
        }
        else if( 0 == wcscmp( strCommand, L"Ka" ) )
        {
            // Ambient color
            float r, g, b;
            InFile >> r >> g >> b;
            pMaterial->vAmbient = XMFLOAT3( r, g, b );
        }
        else if( 0 == wcscmp( strCommand, L"Kd" ) )
        {
            // Diffuse color
            float r, g, b;
            InFile >> r >> g >> b;
            pMaterial->vDiffuse = XMFLOAT3( r, g, b );
        }
        else if( 0 == wcscmp( strCommand, L"Ks" ) )
        {
            // Specular color
            float r, g, b;
            InFile >> r >> g >> b;
            pMaterial->vSpecular = XMFLOAT3( r, g, b );
        }
        else if( 0 == wcscmp( strCommand, L"d" ) ||
                 0 == wcscmp( strCommand, L"Tr" ) )
        {
            // Alpha
            InFile >> pMaterial->fAlpha;
        }
        else if( 0 == wcscmp( strCommand, L"Ns" ) )
        {
            // Shininess
            int nShininess;
            InFile >> nShininess;
            pMaterial->nShininess = nShininess;
        }
        else if( 0 == wcscmp( strCommand, L"illum" ) )
        {
            // Specular on/off
            int illumination;
            InFile >> illumination;
            pMaterial->bSpecular = ( illumination == 2 );
        }
        else if( 0 == wcscmp( strCommand, L"map_Kd" ) )
        {
            // Texture
            InFile >> pMaterial->strTexture;
        }

        else
        {
            // Unimplemented or unrecognized command
        }

        InFile.ignore( 1000, L'\n' );
    }

    InFile.close();

    return S_OK;
}

//--------------------------------------------------------------------------------------
void CLoaderOBJFile::InitMaterial( Material* pMaterial )
{
    ZeroMemory( pMaterial, sizeof( Material ) );

    pMaterial->vAmbient = XMFLOAT3( 0.2f, 0.2f, 0.2f );
    pMaterial->vDiffuse = XMFLOAT3( 0.8f, 0.8f, 0.8f );
    pMaterial->vSpecular = XMFLOAT3( 1.0f, 1.0f, 1.0f );
    pMaterial->nShininess = 0;
    pMaterial->fAlpha = 1.0f;
    pMaterial->bSpecular = false;
}
