#include "stdafx.h"

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
    bsp_gdkvertex_t(_In_ const bsp_triangle_vertex_t& v, _In_ const bsp_pointF_t& normal, _In_ const Vector2& textureDimensions)
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

    Vector3 position;
    Vector3 normal;
    Vector2 texCoord;
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

