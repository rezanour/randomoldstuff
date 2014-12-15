#include "bsp.h"
#include "pak.h"
#include "wal.h"
#include "helpers.h"

struct bsp_triangle_vertex_t
{
    bsp_triangle_vertex_t(_In_ const bsp_pointF_t& position, _In_ const bsp_textureinfo_t* texture) :
        position(position)
    {
        u = position.x * texture->u_axis.x + position.y * texture->u_axis.y + position.z * texture->u_axis.z + texture->u_offset;
        v = position.x * texture->v_axis.x + position.y * texture->v_axis.y + position.z * texture->v_axis.z + texture->v_offset;
    }

    bsp_pointF_t position;
    float u, v; // NOTE NOTE NOTE: These are in PIXELS. They need to be divided by the size of the texture at save time
};

struct bsp_triangle_t
{
    bsp_triangle_t(_In_ const bsp_triangle_vertex_t& a, _In_ const bsp_triangle_vertex_t& b, _In_ const bsp_triangle_vertex_t& c, _In_ const bsp_pointF_t& normal) :
        a(a), b(b), c(c), normal(normal)
    {}

    bsp_triangle_vertex_t a, b, c;
    bsp_pointF_t normal;
};

struct bsp_extracted_t
{
    std::unique_ptr<bsp_plane_t[]> planes;
    std::unique_ptr<bsp_pointF_t[]> vertices;
    std::unique_ptr<bsp_node_t[]> nodes;
    std::unique_ptr<bsp_textureinfo_t[]> textures;
    std::unique_ptr<bsp_face_t[]> faces;
    std::unique_ptr<bsp_leaf_t[]> leaves;
    std::unique_ptr<uint16_t[]> leaf_faces;
    std::unique_ptr<bsp_edge_t[]> edges;
    std::unique_ptr<int32_t[]> face_edges;
    std::unique_ptr<bsp_model_t[]> models; // first model is always the level itself
    std::string entitiesData;

    uint32_t numPlanes;
    uint32_t numVertices;
    uint32_t numNodes;
    uint32_t numTextures;
    uint32_t numFaces;
    uint32_t numLeaves;
    uint32_t numLeafFaces;
    uint32_t numEdges;
    uint32_t numFaceEdges;
    uint32_t numModels;

    // parsed triangles by texture, by model
    std::vector<std::map<std::string, std::vector<bsp_triangle_t>>> triangles;
};

struct bsp_gdkvertex_t
{
    bsp_gdkvertex_t(_In_ const bsp_triangle_vertex_t& v, _In_ const bsp_pointF_t& normal, _In_ const GDK::Float2& textureDimensions)
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

void GetEdgeInCorrectOrder(_In_ const bsp_extracted_t& bsp, _In_ uint32_t faceEdge, _Out_ bsp_pointF_t* start, _Out_ bsp_pointF_t* end)
{
    bsp_edge_t* edge = &bsp.edges[abs(bsp.face_edges[faceEdge])];
    bool reverse = bsp.face_edges[faceEdge] < 0;
    *start = bsp.vertices[(reverse) ? edge->v1 : edge->v0];
    *end = bsp.vertices[(reverse) ? edge->v0 : edge->v1];
}

void AddTriangle(_Inout_ bsp_extracted_t& bsp, _Inout_ std::vector<bsp_triangle_t>& list, _In_ const bsp_plane_t* plane, _In_ const bsp_textureinfo_t* texture, _In_ const bsp_pointF_t& v0, _In_ const bsp_pointF_t& v1, _In_ const bsp_pointF_t& v2)
{
    UNREFERENCED_PARAMETER(bsp);
    list.push_back( bsp_triangle_t( bsp_triangle_vertex_t(v0, texture), bsp_triangle_vertex_t(v1, texture),
            bsp_triangle_vertex_t(v2, texture), plane->normal) );
}

void ProcessBspLeaf(_Inout_ bsp_extracted_t& bsp, _In_ int32_t modelIndex, _In_ int32_t index)
{
    bsp_leaf_t* leaf = &bsp.leaves[index];
    for (uint16_t f = 0; f < leaf->num_leaf_faces; ++f)
    {
        bsp_face_t* face = &bsp.faces.get()[bsp.leaf_faces[leaf->first_leaf_face + f]];
        bsp_plane_t* plane = &bsp.planes.get()[face->plane];
        bsp_textureinfo_t* texture = &bsp.textures.get()[face->texture_info];

        // validate that our face isn't empty (can't make a triangle with only 1 edge)
        if (face->num_edges < 2)
        {
            continue;
        }

        // if we don't already have a triangle list started for this texture,
        // start one now. Otherwise, we'll append the exiting one
        auto it = bsp.triangles[modelIndex].find(texture->texture_name);
        if (it == bsp.triangles[modelIndex].end())
        {
            bsp.triangles[modelIndex][texture->texture_name] = std::vector<bsp_triangle_t>();
            it = bsp.triangles[modelIndex].find(texture->texture_name);
        }

        // get a ref to the triangle list
        auto& list = it->second;

        // faces are defined as edge lists. The first 3 vertices are our initial triangle.
        bsp_pointF_t v0, v1, v2, v;
        GetEdgeInCorrectOrder(bsp, face->first_edge, &v0, &v1);
        GetEdgeInCorrectOrder(bsp, face->first_edge + 1, &v, &v2);
        if (memcmp(&v1, &v, sizeof(v)) != 0) // check that winding order makes sense
        {
            wprintf(L"Bad data!");
            return;
        }
        AddTriangle(bsp, list, plane, texture, v0, v1, v2);

        // Then, for every subsequent vertex v, we use (v0, v-1, v) as the triangle
        for (uint32_t fe = face->first_edge + 2; fe < face->first_edge + face->num_edges; ++fe)
        {
            GetEdgeInCorrectOrder(bsp, fe, &v1, &v2);
            AddTriangle(bsp, list, plane, texture, v0, v1, v2);
        }
    }
}

void ProcessBspNode(_Inout_ bsp_extracted_t& bsp, _In_ int32_t modelIndex, _In_ int32_t index)
{
    bsp_node_t* node = &bsp.nodes[index];

    if (node->front_child < 0)
    {
        int32_t leafIndex = -(node->front_child + 1);
        ProcessBspLeaf(bsp, modelIndex, leafIndex);
    }
    else
    {
        ProcessBspNode(bsp, modelIndex, node->front_child);
    }

    if (node->back_child < 0)
    {
        int32_t leafIndex = -(node->back_child + 1);
        ProcessBspLeaf(bsp, modelIndex, leafIndex);
    }
    else
    {
        ProcessBspNode(bsp, modelIndex, node->back_child);
    }
}

void GenerateTriangles(_Inout_ bsp_extracted_t& bsp)
{
    bsp.triangles.resize(bsp.numModels);
    for (uint32_t i = 0; i < bsp.numModels; ++i)
    {
        auto index = (bsp.models[i].root >= 0) ? bsp.models[i].root : -(bsp.models[i].root + 1);
        ProcessBspNode(bsp, i, index);
    }
}

void ConvertBSPIntoWorldContentAndContentList(
    _In_ const std::wstring& outputName,
    _Inout_ std::ifstream& inputfile, 
    _In_ std::vector<pak_entry_t>& entries,
    _In_ const std::vector<uint8_t>& bsp,
    _Inout_ std::vector<std::wstring>& textureNameList, 
    _Inout_ std::vector<std::wstring>& modelNameList, 
    _Inout_ std::vector<std::shared_ptr<GDK::GeometryContent>>& contentList,
    _Inout_ std::shared_ptr<GDK::GameWorldContent>& gameWorldContent)
{
    const bsp_fileheader_t* bspHeader = nullptr;
    const bsp_lump_t* lump = nullptr;

    bsp_extracted_t extractedBSP;

    bspHeader = (bsp_fileheader_t*)bsp.data();

#define LOAD_LUMP(uptr, count, index, type) \
    lump = &bspHeader->lumps[index]; \
    count = lump->length / sizeof(type); \
    uptr.reset(new (std::nothrow) type[count]); \
    memcpy_s(reinterpret_cast<void*>(uptr.get()), lump->length, bsp.data() + lump->offset, lump->length);

    LOAD_LUMP(extractedBSP.planes, extractedBSP.numPlanes, PLANES_LUMP, bsp_plane_t);
    LOAD_LUMP(extractedBSP.vertices, extractedBSP.numVertices, VERTICES_LUMP, bsp_pointF_t);
    LOAD_LUMP(extractedBSP.nodes, extractedBSP.numNodes, NODES_LUMP, bsp_node_t);
    LOAD_LUMP(extractedBSP.textures, extractedBSP.numTextures, TEXTURES_LUMP, bsp_textureinfo_t);
    LOAD_LUMP(extractedBSP.faces, extractedBSP.numFaces, FACES_LUMP, bsp_face_t);
    LOAD_LUMP(extractedBSP.leaves, extractedBSP.numLeaves, LEAVES_LUMP, bsp_leaf_t);
    LOAD_LUMP(extractedBSP.leaf_faces, extractedBSP.numLeafFaces, LEAF_FACES_LUMP, uint16_t);
    LOAD_LUMP(extractedBSP.edges, extractedBSP.numEdges, EDGES_LUMP, bsp_edge_t);
    LOAD_LUMP(extractedBSP.face_edges, extractedBSP.numFaceEdges, FACE_EDGES_LUMP, int32_t);
    LOAD_LUMP(extractedBSP.models, extractedBSP.numModels, MODELS_LUMP, bsp_model_t);

#undef LOAD_LUMP

    lump = &bspHeader->lumps[ENTITIES_LUMP];
    std::unique_ptr<char[]> entityBlock(new char[lump->length + 1]);
    memcpy_s(entityBlock.get(), lump->length, bsp.data() + lump->offset, lump->length);
    entityBlock[lump->length] = 0;

    GenerateTriangles(extractedBSP);

    std::vector<GDK::GameWorldContent::ModelEntry> models;
    std::vector<GDK::GameWorldContent::ModelChunk> chunks;
    std::vector<GDK::GameWorldContent::EntityEntry> entities;
    std::vector<GDK::GameWorldContent::EntityProperty> entityProperties;

    for (uint32_t i = 0; i < extractedBSP.numModels; ++i)
    {
        GDK::GameWorldContent::ModelEntry model;
        model.offset = chunks.size();

        for (auto chunk : extractedBSP.triangles[i])
        {
            // Add .wal texture
            std::string chunkTexture = "textures/" + chunk.first + ".wal";
            textureNameList.push_back(AnsiToWide(chunkTexture));

            // Add model name
            std::string modelName = (i == 0) ? "" : std::to_string(i) + "\\";
            modelNameList.push_back(AnsiToWide(modelName));

            // Read .wal texture dimensions
            std::vector<uint8_t> walData;
            ReadPakEntryToBuffer(inputfile, entries, chunkTexture, walData);

            wal_fileheader_t* walHeader = (wal_fileheader_t*)walData.data();
            GDK::Float2 textureDimensions;
            textureDimensions.x = (float)walHeader->width;
            textureDimensions.y = (float)walHeader->height;

            // Add .geometry
            std::shared_ptr<GDK::GeometryContent> geometryContent;

            std::unique_ptr<byte_t[]> vertexData;
            std::unique_ptr<uint32_t[]> indexData;
            std::unique_ptr<GDK::GeometryContent::Animation[]> animationData;

            uint32_t attributeCount = 3;
            std::unique_ptr<GDK::GeometryContent::AttributeDesc[]> attributes(new GDK::GeometryContent::AttributeDesc[3]);
            attributes.get()[0] = GDK::GeometryContent::AttributeDesc(0, GDK::GeometryContent::AttributeType::Float3, GDK::GeometryContent::AttributeName::Position, 0);
            attributes.get()[1] = GDK::GeometryContent::AttributeDesc(12, GDK::GeometryContent::AttributeType::Float3, GDK::GeometryContent::AttributeName::Normal, 0);
            attributes.get()[2] = GDK::GeometryContent::AttributeDesc(24, GDK::GeometryContent::AttributeType::Float2, GDK::GeometryContent::AttributeName::TexCoord, 0);

            std::vector<bsp_gdkvertex_t> vertices;
            std::vector<uint32_t> indices;

            for (auto triangle : chunk.second)
            {
                indices.push_back((uint32_t)vertices.size());
                vertices.push_back(bsp_gdkvertex_t(triangle.a, triangle.normal, textureDimensions));

                indices.push_back((uint32_t)vertices.size());
                vertices.push_back(bsp_gdkvertex_t(triangle.b, triangle.normal, textureDimensions));

                indices.push_back((uint32_t)vertices.size());
                vertices.push_back(bsp_gdkvertex_t(triangle.c, triangle.normal, textureDimensions));
            }

            // Convert the vertices and indices into buffers appropriate for GDK
            vertexData.reset(new byte_t[sizeof(bsp_gdkvertex_t) * vertices.size()]);
            indexData.reset(new uint32_t[indices.size()]);

            memcpy(vertexData.get(), vertices.data(), sizeof(bsp_gdkvertex_t) * vertices.size());
            memcpy(indexData.get(), indices.data(), sizeof(uint32_t) * indices.size());

            animationData.reset(new GDK::GeometryContent::Animation[1]);
            animationData.get()[0].StartFrame = 0;
            animationData.get()[0].EndFrame = 1;
            strcpy_s(animationData.get()[0].Name, "noname");

            geometryContent = GDK::GeometryContent::Create(attributeCount, attributes, (uint32_t)sizeof(bsp_gdkvertex_t), (uint32_t)vertices.size(), vertexData, (uint32_t)indices.size(), indexData, 1, 1, animationData);
            contentList.push_back(geometryContent);

            // Prep world data
            std::wstring geoName = AnsiToWide(chunk.first);
            std::wstring texName = AnsiToWide(chunk.first);
            ReplaceCharacter(geoName, L'/', L'%');
            ReplaceCharacter(texName, L'/', L'\\');
            geoName = outputName + L"\\" + modelNameList[modelNameList.size() - 1] +  L"textures%" + geoName + L".wal.geometry";
            texName = L"textures\\" + texName + L".wal.texture";

            GDK::GameWorldContent::ModelChunk modelChunk;
            wcscpy_s(modelChunk.geometry, geoName.c_str());
            wcscpy_s(modelChunk.texture, texName.c_str());
            chunks.push_back(modelChunk);
        }

        model.numChunks = chunks.size() - model.offset;
        models.push_back(model);
    }

    // Process Entities
    GDK::GameWorldContent::EntityEntry entity;
    GDK::GameWorldContent::EntityProperty prop;
    char* e = entityBlock.get();
    while (*e != 0)
    {
        while (*e == '\n')
        {
            ++e;
        }
        if (*e == '{')
        {
            // start new entity
            entity.offset = entityProperties.size();
        }
        else if (*e == '}')
        {
            // end entity
            entity.numProperties = entityProperties.size() - entity.offset;
            entities.push_back(entity);
        }
        else
        {
            // read a property
            char* end = e;
            while (*end != '\n' && *end != 0)
            {
                ++end;
            }
            std::string line(e, end);

            // parse out name/value pair
            auto firstQuote = line.find('\"');
            auto secondQuote = line.find('\"', firstQuote + 1);
            auto thirdQuote = line.find('\"', secondQuote + 1);
            auto fourthQuote = line.find('\"', thirdQuote + 1);

            std::wstring name = AnsiToWide(line.substr(firstQuote + 1, secondQuote - firstQuote - 1));
            std::wstring value = AnsiToWide(line.substr(thirdQuote + 1, fourthQuote - thirdQuote - 1));

            // Convert from any quake2-specific data to GDK format
            if (name == L"origin")
            {
                GDK::Vector3 position = GDK::StringUtils::Parse<GDK::Vector3>(value);
                value = GDK::StringUtils::ToString(GDK::Vector3(position.x, position.z, position.y));
            }
            else if (name == L"angle")
            {
                float angle = GDK::StringUtils::Parse<float>(value);
                if (angle >= 0)
                {
                    value = GDK::StringUtils::ToString(GDK::Math::ToDegrees(GDK::Math::PiOver2 - GDK::Math::ToRadians(angle)));
                }
            }

            wcscpy_s(prop.name, name.c_str());
            wcscpy_s(prop.value, value.c_str());
            entityProperties.push_back(prop);

            e = end;
            if (*e == 0)
            {
                break;
            }
        }
        ++e;
    }

    gameWorldContent = GDK::GameWorldContent::Create(
        outputName, 
        static_cast<uint32_t>(models.size()), models.data(),
        static_cast<uint32_t>(chunks.size()), chunks.data(),
        static_cast<uint32_t>(entities.size()), entities.data(),
        static_cast<uint32_t>(entityProperties.size()), entityProperties.data());
}
