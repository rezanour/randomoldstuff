#include "stdafx.h"

struct BSP_TRIANGLE_VERTEX
{
    BSP_TRIANGLE_VERTEX(_In_ const BSP_POINT3F& position, _In_ const BSP_TEXTUREINFO* texture) :
        position(position)
    {
        u = position.x * texture->u_axis.x + position.y * texture->u_axis.y + position.z * texture->u_axis.z + texture->u_offset;
        v = position.x * texture->v_axis.x + position.y * texture->v_axis.y + position.z * texture->v_axis.z + texture->v_offset;
    }

    BSP_POINT3F position;
    float u, v; // NOTE NOTE NOTE: These are in PIXELS. They need to be divided by the size of the texture at save time
};

struct BSP_TRIANGLE
{
    BSP_TRIANGLE(_In_ const BSP_TRIANGLE_VERTEX& a, _In_ const BSP_TRIANGLE_VERTEX& b, _In_ const BSP_TRIANGLE_VERTEX& c, _In_ const BSP_POINT3F& normal) :
        a(a), b(b), c(c), normal(normal)
    {}

    BSP_TRIANGLE_VERTEX a, b, c;
    BSP_POINT3F normal;
};

struct ExtractedBsp
{
    std::unique_ptr<BSP_PLANE[]> planes;
    std::unique_ptr<BSP_POINT3F[]> vertices;
    std::unique_ptr<BSP_NODE[]> nodes;
    std::unique_ptr<BSP_TEXTUREINFO[]> textures;
    std::unique_ptr<BSP_FACE[]> faces;
    std::unique_ptr<BSP_LEAF[]> leaves;
    std::unique_ptr<uint16_t[]> leaf_faces;
    std::unique_ptr<BSP_EDGE[]> edges;
    std::unique_ptr<int32_t[]> face_edges;

    uint32_t numPlanes;
    uint32_t numVertices;
    uint32_t numNodes;
    uint32_t numTextures;
    uint32_t numFaces;
    uint32_t numLeaves;
    uint32_t numLeafFaces;
    uint32_t numEdges;
    uint32_t numFaceEdges;

    // parsed triangles by texture
    std::map<std::string, std::vector<BSP_TRIANGLE>> triangles;
};

static HRESULT GenerateTriangles(_Inout_ ExtractedBsp& bsp);
static HRESULT SaveToObjectsInDirectory(_In_z_ const wchar_t* pakFile, _In_ const ExtractedBsp& bsp, _In_z_ const wchar_t* rootDirectory);

HRESULT SaveBspAsObjectsUnderRoot(_In_z_ const wchar_t* pakFile, _In_count_(numBytes) const byte_t* data, _In_ uint32_t numBytes, _In_z_ const wchar_t* worldFolder)
{
    if (!data || !worldFolder)
    {
        return E_INVALIDARG;
    }

    HRESULT hr = S_OK;
    const BSP_HEADER* header = nullptr;
    const BSP_LUMP* lump = nullptr;

    ExtractedBsp bsp;

    // read the header
    header = reinterpret_cast<const BSP_HEADER*>(data);

    // check magic number & version
    CHECKTRUE(header->magic == header->c_Magic, L"magic number didn't match");
    CHECKTRUE(header->version == header->c_SupportedVersion, L"wrong bsp file version");

    // Load the lumps

#define LOAD_LUMP(uptr, count, index, type) \
    lump = &header->lumps[index]; \
    count = lump->length / sizeof(type); \
    uptr.reset(new (std::nothrow) type[count]); \
    CHECKPOINTER(uptr, L"Failed to allocate " L#uptr L" list"); \
    memcpy_s(reinterpret_cast<void*>(uptr.get()), lump->length, data + lump->offset, lump->length);

    LOAD_LUMP(bsp.planes, bsp.numPlanes, PLANES_LUMP, BSP_PLANE);
    LOAD_LUMP(bsp.vertices, bsp.numVertices, VERTICES_LUMP, BSP_POINT3F);
    LOAD_LUMP(bsp.nodes, bsp.numNodes, NODES_LUMP, BSP_NODE);
    LOAD_LUMP(bsp.textures, bsp.numTextures, TEXTURES_LUMP, BSP_TEXTUREINFO);
    LOAD_LUMP(bsp.faces, bsp.numFaces, FACES_LUMP, BSP_FACE);
    LOAD_LUMP(bsp.leaves, bsp.numLeaves, LEAVES_LUMP, BSP_LEAF);
    LOAD_LUMP(bsp.leaf_faces, bsp.numLeafFaces, LEAF_FACES_LUMP, uint16_t);
    LOAD_LUMP(bsp.edges, bsp.numEdges, EDGES_LUMP, BSP_EDGE);
    LOAD_LUMP(bsp.face_edges, bsp.numFaceEdges, FACE_EDGES_LUMP, int32_t);

#undef LOAD_LUMP

    CHECKHR(GenerateTriangles(bsp), L"Failed to generate triangles");
    CHECKHR(SaveToObjectsInDirectory(pakFile, bsp, worldFolder), L"Failed to save out triangle data");

Exit:

    return S_OK;
}

static void GetEdgeInCorrectOrder(_In_ const ExtractedBsp& bsp, _In_ uint32_t faceEdge, _Inout_ BSP_POINT3F* start, _Inout_ BSP_POINT3F* end)
{
    BSP_EDGE* edge = &bsp.edges[abs(bsp.face_edges[faceEdge])];
    bool reverse = bsp.face_edges[faceEdge] < 0;
    *start = bsp.vertices[(reverse) ? edge->v1 : edge->v0];
    *end = bsp.vertices[(reverse) ? edge->v0 : edge->v1];
}

static void AddTriangle(_Inout_ ExtractedBsp& bsp, _Inout_ std::vector<BSP_TRIANGLE>& list, _In_ const BSP_PLANE* plane, _In_ const BSP_TEXTUREINFO* texture, _In_ const BSP_POINT3F& v0, _In_ const BSP_POINT3F& v1, _In_ const BSP_POINT3F& v2)
{
    list.push_back(
        BSP_TRIANGLE(
            BSP_TRIANGLE_VERTEX(v0, texture),
            BSP_TRIANGLE_VERTEX(v1, texture),
            BSP_TRIANGLE_VERTEX(v2, texture),
            plane->normal)
        );
}

HRESULT GenerateTriangles(_Inout_ ExtractedBsp& bsp)
{
    HRESULT hr = S_OK;
    uint32_t i;

    for (i = 0; i < bsp.numFaces; ++i)
    {
        BSP_FACE* face = &bsp.faces.get()[i];
        BSP_PLANE* plane = &bsp.planes.get()[face->plane];
        BSP_TEXTUREINFO* texture = &bsp.textures.get()[face->texture_info];

        // validate that our face isn't empty (can't make a triangle with only 1 edge)
        if (face->num_edges < 2)
        {
            continue;
        }

        // if we don't already have a triangle list started for this texture,
        // start one now. Otherwise, we'll append the exiting one
        auto it = bsp.triangles.find(texture->texture_name);
        if (it == bsp.triangles.end())
        {
            bsp.triangles[texture->texture_name] = std::vector<BSP_TRIANGLE>();
            it = bsp.triangles.find(texture->texture_name);
        }

        // get a ref to the triangle list
        auto& list = it->second;

        // faces are defined as edge lists. The first 3 vertices are our initial triangle.
        BSP_POINT3F v0, v1, v2, v;
        GetEdgeInCorrectOrder(bsp, face->first_edge, &v0, &v1);
        GetEdgeInCorrectOrder(bsp, face->first_edge + 1, &v, &v2);
        if (memcmp(&v1, &v, sizeof(v)) != 0) // check that winding order makes sense
        {
            wprintf(L"Bad data!");
            return E_FAIL;
        }
        AddTriangle(bsp, list, plane, texture, v0, v1, v2);

        // Then, for every subsequent vertex v, we use (v0, v-1, v) as the triangle
        for (uint32_t fe = face->first_edge + 2; fe < face->first_edge + face->num_edges; ++fe)
        {
            GetEdgeInCorrectOrder(bsp, fe, &v1, &v2);
            AddTriangle(bsp, list, plane, texture, v0, v1, v2);
        }
    }

    return hr;
}

static HRESULT EnsureDirectoryExists(_Inout_ std::wstring& path)
{
    // normalize slashes
    for (auto i = 0; i < path.size(); ++i)
    {
        if (path[i] == L'/')
            path[i] = L'\\';
    }

    // recursively create directories up the end
    auto slash = path.find_first_of(L'\\');
    while (slash != path.npos)
    {
        std::wstring subPath = path.substr(0, slash);
        if (!CreateDirectory(subPath.c_str(), nullptr))
        {
            DWORD gle = GetLastError();
            if (gle != ERROR_ALREADY_EXISTS)
            {
                wprintf(L"\nFailed to create directory %s, GLE = %d\n", subPath.c_str(), gle);
                return E_FAIL;
            }
        }

        slash = path.find_first_of(L'\\', slash + 1);
    }

    // and the final directory
    if (!CreateDirectory(path.c_str(), nullptr))
    {
        DWORD gle = GetLastError();
        if (gle != ERROR_ALREADY_EXISTS)
        {
            wprintf(L"\nFailed to create directory %s, GLE = %d\n", path.c_str(), gle);
            return E_FAIL;
        }
    }


    return S_OK;
}

static HRESULT SaveTrianglesAsGeometryContent(_In_z_ const wchar_t* outputPath, _In_ const std::vector<BSP_TRIANGLE>& triangles, _In_ const GDK::Float2& textureDimensions)
{
    HRESULT hr = S_OK;

    std::ofstream output(outputPath, std::ios_base::binary);
    std::shared_ptr<GDK::GeometryContent> geometryContent;

    std::unique_ptr<byte_t[]> vertexData;
    std::unique_ptr<uint32_t[]> indexData;
    std::unique_ptr<GDK::GeometryContent::Animation[]> animationData;

    uint32_t attributeCount = 3;
    std::unique_ptr<GDK::GeometryContent::AttributeDesc[]> attributes(new GDK::GeometryContent::AttributeDesc[3]);
    attributes.get()[0] = GDK::GeometryContent::AttributeDesc(0, GDK::GeometryContent::AttributeType::Float3, GDK::GeometryContent::AttributeName::Position, 0);
    attributes.get()[1] = GDK::GeometryContent::AttributeDesc(12, GDK::GeometryContent::AttributeType::Float3, GDK::GeometryContent::AttributeName::Normal, 0);
    attributes.get()[2] = GDK::GeometryContent::AttributeDesc(24, GDK::GeometryContent::AttributeType::Float2, GDK::GeometryContent::AttributeName::TexCoord, 0);

    struct GDK_VERTEX
    {
        GDK_VERTEX(_In_ const BSP_TRIANGLE_VERTEX& v, _In_ const BSP_POINT3F& normal, _In_ const GDK::Float2& textureDimensions)
        {
            position.x = v.position.x;
            position.y = v.position.z; // flip y & z here
            position.z = v.position.y;

            this->normal.x = normal.x;
            this->normal.y = normal.z; // flip y & z here
            this->normal.z = normal.y;

            // translate the uvs here.
            texCoord.x = v.u / textureDimensions.x;
            texCoord.y = v.v / textureDimensions.y;
        }

        GDK::Float3 position;
        GDK::Float3 normal;
        GDK::Float2 texCoord;
    };

    std::vector<GDK_VERTEX> vertices;
    std::vector<uint32_t> indices;

    for (auto triangle : triangles)
    {
        indices.push_back((uint32_t)vertices.size());
        vertices.push_back(GDK_VERTEX(triangle.a, triangle.normal, textureDimensions));

        indices.push_back((uint32_t)vertices.size());
        vertices.push_back(GDK_VERTEX(triangle.b, triangle.normal, textureDimensions));

        indices.push_back((uint32_t)vertices.size());
        vertices.push_back(GDK_VERTEX(triangle.c, triangle.normal, textureDimensions));
    }

    // Convert the vertices and indices into buffers appropriate for GDK
    vertexData.reset(new byte_t[sizeof(GDK_VERTEX) * vertices.size()]);
    indexData.reset(new uint32_t[indices.size()]);

    memcpy(vertexData.get(), vertices.data(), sizeof(GDK_VERTEX) * vertices.size());
    memcpy(indexData.get(), indices.data(), sizeof(uint32_t) * indices.size());

    animationData.reset(new GDK::GeometryContent::Animation[1]);
    animationData.get()[0].StartFrame = 0;
    animationData.get()[0].EndFrame = 1;
    strcpy(animationData.get()[0].Name, "noname");

    geometryContent = GDK::GeometryContent::Create(attributeCount, attributes, (uint32_t)sizeof(GDK_VERTEX), (uint32_t)vertices.size(), vertexData, (uint32_t)indices.size(), indexData, 1, 1, animationData);
    geometryContent->Save(output);

    return hr;
}

HRESULT SaveToObjectsInDirectory(_In_z_ const wchar_t* pakFile, _In_ const ExtractedBsp& bsp, _In_z_ const wchar_t* rootDirectory)
{
    HRESULT hr = S_OK;
    std::vector<byte_t> data;
    std::vector<byte_t> bmpData;
    std::vector<RGBQUAD> bmpal;
    BITMAPINFOHEADER bmih;
    GDK::Float2 textureDimensions;

    CHECKHR(GetPakChunk(pakFile, "pics/colormap.pcx", data), L"Error finding colormap texture");
    ConvertPCXToBMP((PCXFILEHEADER*)data.data(), data.data(), (uint32_t)data.size(), bmpData, bmpal, bmih);

    for (auto chunk : bsp.triangles)
    {
        std::wstring worldPath(rootDirectory);
        worldPath += L"\\";
        worldPath += AnsiToWide(chunk.first);
        worldPath += L".object";

        CHECKHR(EnsureDirectoryExists(worldPath), L"failed to create path");

        CHECKHR(GetPakChunk(pakFile, ("textures/" + chunk.first + ".wal").c_str(), data), L"Error finding WAL data");
        CHECKHR(SaveWALAsTextureContent((worldPath + L"\\primary.texture").c_str(), "", data.data(), (uint32_t)data.size(), bmpal), L"Failed to save texture");

        WAL_HEADER* walHeader = (WAL_HEADER*)data.data();
        textureDimensions.x = (float)walHeader->width;
        textureDimensions.y = (float)walHeader->height;
        CHECKHR(SaveTrianglesAsGeometryContent((worldPath + L"\\mesh.geometry").c_str(), chunk.second, textureDimensions), L"Failed to save geometry");
    }

Exit:

    return hr;
}
