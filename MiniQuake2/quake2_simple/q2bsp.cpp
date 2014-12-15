#include "stdafx.h"
#include "q2bsp.h"
#include "q2bspp.h"
#include "q2renderp.h"

#pragma pack(push, 1)

struct bsp_pointF_t
{
    float x;
    float y;
    float z;
};

struct bsp_pointS_t
{
    int16_t x;
    int16_t y;
    int16_t z;
};

// a table entry for a "data section" or lump in the file
struct bsp_lump_t
{
    uint32_t offset;
    uint32_t length;
};

/*
    Lump descriptions

    N.B.    Taken from www.flipcode.com's bsp description.
            According to the site, the ones marked ? aren't needed for rendering.

    Index   Name                    Description
    0       Entities                MAP entity text buffer
    1       Planes                  Plane array
    2       Vertices                Vertex array
    3       Visibility              Compressed PVS data and directory for all clusters
    4       Nodes                   Internal node array for the BSP tree
    5       Texture Information     Face texture application array
    6       Faces                   Face array
    7       Lightmaps               Lightmaps
    8       Leaves                  Internal leaf array of the BSP tree
    9       Leaf Face Table         Index lookup table for referencing the face array from a leaf
    10      Leaf Brush Table        ?
    11      Edges                   Edge array
    12      Face Edge Table         Index lookup table for referencing the edge array from a face
    13      Models                  Submodels using the same faces, etc... Each is a seperate mini-bsp
    14      Brushes                 ?
    15      Brush Sides             ?
    16      Pop                     ?
    17      Areas                   ?
    18      Area Portals            ?
*/

// Lumps we actually use
#define ENTITIES_LUMP   0
#define PLANES_LUMP     1
#define VERTICES_LUMP   2
#define NODES_LUMP      4
#define TEXTURES_LUMP   5
#define FACES_LUMP      6
#define LEAVES_LUMP     8
#define LEAF_FACES_LUMP 9
#define EDGES_LUMP      11
#define FACE_EDGES_LUMP 12
#define MODELS_LUMP     13

struct bsp_fileheader_t
{
    static const uint32_t c_Magic = 0x50534249; // "IBSP"
    static const uint32_t c_SupportedVersion = 38;
    uint32_t magic;
    uint32_t version; // should always be 38 for Quake2

    bsp_lump_t lumps[19]; // there are 19 data sections in the file
};

struct bsp_edge_t
{
    // No direction can be assumed, as an edge can be shared between
    // two faces with different winding. Face contains direction info
    uint16_t v0;
    uint16_t v1;
};

// NOTE: faces reference int32_t's in the face_edge lump. (array of int32's)
// This provides a redirection over the edge array to enable edge directions
// if edgeIndex is positive, then edge.v0 is start of edge.
// if edgeIndex is negative, then edge.v1 is start of edge.

struct bsp_face_t
{
    uint16_t plane;         // index into the planes list
    uint16_t plane_side;    // if nonzero, face normal is opposite plane normal

    uint32_t first_edge;    // index of first edge in the face_edge list
    uint16_t num_edges;     // number of consecutive face_edges

    uint16_t texture_info;          // index into the texture list
    uint8_t lightmap_styles[4];     // bitflags for the lightmaps
    uint32_t lightmap_offset;       // offset of the lightmap (in bytes) in the lightmap lump (not indexed, just back to back lightmaps)
};


// Planes are stored in ABCD format:
// f(x, y, z) = Ax + By + Cz - D
//
// which is P * N - D.
// 0    on the plane
// > 0  in front of the plane
// < 0  behind the plane
//
struct bsp_plane_t
{
    bsp_pointF_t normal;     // A, B, C components of equation
    float distance;         // D in equation
    uint32_t type; // not used in rendering, maybe contains collision flags or something?
};

struct bsp_node_t
{
    // bsp data
    uint32_t plane;         // index into plane array of splitting plane
    int32_t front_child;    // recursive index of front child. negative is leaf, -(index + 1)
    int32_t back_child;     // recursive index of back child. negative is leaf, -(index + 1)

    // bounding box of node
    bsp_pointS_t bbox_min;   // REMEMBER: z is UP here...
    bsp_pointS_t bbox_max;

    // faces contained in the node.
    // NOTE NOTE:
    // In the original game, this is only used for collision.
    // and the compressed PVS is used for rendering.
    // For GDK, we use these directly to generate the mesh, since we don't use PVS
    uint16_t first_face;
    uint16_t num_faces;
};

struct bsp_leaf_t
{
    uint32_t brush_or; // ? unused for rendering...

    uint16_t cluster; // -1 (0xFFFF) for cluster indicates no PVS
    uint16_t area; // unused?

    bsp_pointS_t bbox_min;
    bsp_pointS_t bbox_max;

    uint16_t first_leaf_face; // leaf face array is just a level of uint16_t indirection over face array. Not sure why???
    uint16_t num_leaf_faces;

    uint16_t first_leaf_brush; // ? not used?
    uint16_t num_leaf_brushes;
};

struct bsp_textureinfo_t
{
    // to generate texture coordinates for vertex (x, y, z):
    // u = x * u_axis.x + y * u_axis.y + z * u_axis.z + u_offset;
    // v = x * v_axis.x + y * v_axis.y + z * v_axis.z + v_offset;

    bsp_pointF_t u_axis;
    float u_offset;
    bsp_pointF_t v_axis;
    float v_offset;

    uint32_t flags;
    uint32_t value;

    char texture_name[32];

    uint32_t next_texinfo; // animated textures reference each other :)
};

struct bsp_model_t
{
    bsp_pointF_t bbox_min;
    bsp_pointF_t bbox_max;
    bsp_pointF_t origin;            // origin of model, usually (0,0,0)
    int32_t root;                   // index of first BSP node
    int32_t first_face;             // index of Faces
    int32_t num_faces;              // number of Faces
};

#pragma pack(pop)

struct Bsp
{
    unsigned char* data;
    bsp_fileheader_t* header;

    char* entities;
    bsp_plane_t* planes;
    bsp_pointF_t* vertices;
    bsp_node_t* nodes;
    bsp_textureinfo_t* textures;
    bsp_face_t* faces;
    bsp_leaf_t* leaves;
    uint16_t* leafFaces;
    bsp_edge_t* edges;
    int32_t* faceEdges;
    bsp_model_t* models;

    int entityLength;
    int numPlanes;
    int numVertices;
    int numNodes;
    int numTextures;
    int numFaces;
    int numLeafFaces;
    int numLeaves;
    int numEdges;
    int numFaceEdges;
    int numModels;
};

static Bsp* g_bsp = nullptr;

static void 
BspCleanup(Bsp* bsp)
{
    delete [] bsp->data;
}

void
BspLoad(
    PCSTR filename
    )
{
    if (g_bsp != nullptr)
    {
        BspCleanup(g_bsp);
        delete g_bsp;
        g_bsp = nullptr;
    }

    g_bsp = new Bsp;
    g_bsp->data = (unsigned char*)PakLoadFile(filename);

    g_bsp->header = (bsp_fileheader_t*)g_bsp->data;
    if (g_bsp->header->magic != g_bsp->header->c_Magic)
    {
        RIP(L"Invalid bsp file.\n");
    }

    g_bsp->entities = (char*)(g_bsp->data + g_bsp->header->lumps[ENTITIES_LUMP].offset);
    g_bsp->entityLength = g_bsp->header->lumps[ENTITIES_LUMP].length;

    g_bsp->planes = (bsp_plane_t*)(g_bsp->data + g_bsp->header->lumps[PLANES_LUMP].offset);
    g_bsp->numPlanes = g_bsp->header->lumps[PLANES_LUMP].length / sizeof(bsp_plane_t);

    g_bsp->vertices = (bsp_pointF_t*)(g_bsp->data + g_bsp->header->lumps[VERTICES_LUMP].offset);
    g_bsp->numVertices = g_bsp->header->lumps[VERTICES_LUMP].length / sizeof(bsp_pointF_t);

    g_bsp->nodes = (bsp_node_t*)(g_bsp->data + g_bsp->header->lumps[NODES_LUMP].offset);
    g_bsp->numNodes = g_bsp->header->lumps[NODES_LUMP].length / sizeof(bsp_node_t);

    g_bsp->textures = (bsp_textureinfo_t*)(g_bsp->data + g_bsp->header->lumps[TEXTURES_LUMP].offset);
    g_bsp->numTextures = g_bsp->header->lumps[TEXTURES_LUMP].length / sizeof(bsp_textureinfo_t);

    g_bsp->faces = (bsp_face_t*)(g_bsp->data + g_bsp->header->lumps[FACES_LUMP].offset);
    g_bsp->numFaces = g_bsp->header->lumps[FACES_LUMP].length / sizeof(bsp_face_t);

    g_bsp->leafFaces = (uint16_t*)(g_bsp->data + g_bsp->header->lumps[LEAF_FACES_LUMP].offset);
    g_bsp->numLeafFaces = g_bsp->header->lumps[LEAF_FACES_LUMP].length / sizeof(uint16_t);

    g_bsp->leaves = (bsp_leaf_t*)(g_bsp->data + g_bsp->header->lumps[LEAVES_LUMP].offset);
    g_bsp->numLeaves = g_bsp->header->lumps[LEAVES_LUMP].length / sizeof(bsp_leaf_t);

    g_bsp->edges = (bsp_edge_t*)(g_bsp->data + g_bsp->header->lumps[EDGES_LUMP].offset);
    g_bsp->numEdges = g_bsp->header->lumps[EDGES_LUMP].length / sizeof(bsp_edge_t);

    g_bsp->faceEdges = (int32_t*)(g_bsp->data + g_bsp->header->lumps[FACE_EDGES_LUMP].offset);
    g_bsp->numFaceEdges = g_bsp->header->lumps[FACE_EDGES_LUMP].length / sizeof(int32_t);

    g_bsp->models = (bsp_model_t*)(g_bsp->data + g_bsp->header->lumps[MODELS_LUMP].offset);
    g_bsp->numModels = g_bsp->header->lumps[MODELS_LUMP].length / sizeof(bsp_model_t);

    for (auto i = 0; i < g_bsp->numTextures; ++i)
    {
        // repurpose the value to be our cached index
        g_bsp->textures[i].value = UINT32_MAX;
    }

    RenderpClearTextures();
}




// Peg the params to simplify the calls below

static GDK::Vector3 g_position;
static float g_rotation;

static GDK::Vector3 g_planeNormals[6];
static float g_planeDists[6];



static bool
InFrontOfPlane(
    _In_ const bsp_plane_t* plane
    )
{
    float d = g_position.x * plane->normal.x + g_position.z * plane->normal.y + g_position.y * plane->normal.z;
    return d > plane->distance;
}

static void
DrawNode(
    _In_ int32_t index
    )
{
    if (index < 0)
    {
        // Leaf
        int32_t leafIndex = -(index + 1);
        bsp_leaf_t* leaf = &g_bsp->leaves[leafIndex];

        uint16_t leafFaceIndex = leaf->first_leaf_face;
        uint16_t numLeafFaces = leaf->num_leaf_faces;
        while (numLeafFaces-- > 0)
        {
            bsp_face_t* face = &g_bsp->faces[g_bsp->leafFaces[leafFaceIndex++]];

            GDK::Vector3 triangle[3];
            GDK::Vector2 uvs[3];
            uint16_t numVerts = 0;

            bsp_textureinfo_t* texture = &g_bsp->textures[face->texture_info];

            // repurpose the value parameter to be our cached index
            if (texture->value == UINT32_MAX)
            {
                texture->value = RenderpRegisterTexture(texture->texture_name);
            }

            uint16_t edgeIndex = face->first_edge;
            uint16_t numEdges = face->num_edges;
            while (numEdges-- > 0)
            {
                int32_t faceEdge = g_bsp->faceEdges[edgeIndex++];
                bsp_edge_t* edge = &g_bsp->edges[abs(faceEdge)];

                bsp_pointF_t v0 = g_bsp->vertices[(faceEdge > 0) ? edge->v0 : edge->v1];
                bsp_pointF_t v1 = g_bsp->vertices[(faceEdge > 0) ? edge->v1 : edge->v0];

                GDK::Vector2 uv0(
                    v0.x * texture->u_axis.x + v0.y * texture->u_axis.y + v0.z * texture->u_axis.z + texture->u_offset,
                    v0.x * texture->v_axis.x + v0.y * texture->v_axis.y + v0.z * texture->v_axis.z + texture->v_offset);

                GDK::Vector2 uv1(
                    v1.x * texture->u_axis.x + v1.y * texture->u_axis.y + v1.z * texture->u_axis.z + texture->u_offset,
                    v1.x * texture->v_axis.x + v1.y * texture->v_axis.y + v1.z * texture->v_axis.z + texture->v_offset);

                if (numVerts == 0)
                {
                    // first edge
                    triangle[0] = GDK::Vector3(v0.x, v0.z, v0.y);
                    triangle[1] = GDK::Vector3(v1.x, v1.z, v1.y);
                    uvs[0] = uv0;
                    uvs[1] = uv1;
                    numVerts = 2;
                }
                else if (numVerts == 2)
                {
                    // second edge
                    triangle[2] = GDK::Vector3(v1.x, v1.z, v1.y);
                    uvs[2] = uv1;

                    RenderpDrawTriangle(texture->value, triangle[0], uvs[0], triangle[1], uvs[1], triangle[2], uvs[2]);

                    numVerts = 3;
                }
                else
                {
                    // other edges
                    triangle[1] = triangle[2];
                    uvs[1] = uvs[2];
                    triangle[2] = GDK::Vector3(v1.x, v1.z, v1.y);
                    uvs[2] = uv1;
                    RenderpDrawTriangle(texture->value, triangle[0], uvs[0], triangle[1], uvs[1], triangle[2], uvs[2]);
                }
            }
        }

    }
    else
    {
        // Node
        bsp_node_t* node = &g_bsp->nodes[index];

        // throw out nodes that aren't visible
        GDK::Vector3 bbmin(node->bbox_min.x, node->bbox_min.z, node->bbox_min.y);
        GDK::Vector3 bbmax(node->bbox_max.x, node->bbox_max.z, node->bbox_max.y);

        // approx as sphere
        GDK::Vector3 center = (bbmin + bbmax) * 0.5f;
        float radius = ((bbmax - bbmin) * 0.5f).Length();

        // todo: implement the other clip planes
        for (int i = 0; i < 2; ++i)
        {
            if (GDK::Vector3::Dot(center, g_planeNormals[i]) - g_planeDists[0] > radius)
            {
                // not visible
                return;
            }
        }

        bool inFront = InFrontOfPlane(&g_bsp->planes[node->plane]);
        if (inFront)
        {
            // Draw front, then back child
            DrawNode(node->front_child);
            DrawNode(node->back_child);
        }
        else
        {
            // Draw back, then front child
            DrawNode(node->back_child);
            DrawNode(node->front_child);
        }
    }
}

void
BsppDrawFrontToBack(
    _In_ const GDK::Vector3& position,
    _In_ float rotation
    )
{
    g_position = position;
    g_rotation = rotation;

    GDK::Matrix rot(GDK::Matrix::CreateRotationY(rotation));

    g_planeNormals[0] = rot.GetForward();
    g_planeDists[0] = GDK::Vector3::Dot(g_planeNormals[0], position) + 10000;

    g_planeNormals[1] = -rot.GetForward();
    g_planeDists[1] = GDK::Vector3::Dot(g_planeNormals[1], position) - 0.01f;



    DrawNode(g_bsp->models[0].root);
}