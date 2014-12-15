#pragma once

#pragma pack(push, 1)

// NOTE NOTE NOTE!!!
//
// All coordinates in the BSP use a coordinate system where x, y are the ground plane,
// and z is UP. This can be confusing when reading through the equations and math.
//
// We convert these (at the last possible point) before saving them in our own GDK formats
//

struct BSP_POINT3F
{
    float x;
    float y;
    float z;
};

struct BSP_POINT3S
{
    int16_t x;
    int16_t y;
    int16_t z;
};

// a table entry for a "data section" or lump in the file
struct BSP_LUMP
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
    13      Models                  ?
    14      Brushes                 ?
    15      Brush Sides             ?
    16      Pop                     ?
    17      Areas                   ?
    18      Area Portals            ?
*/

// Lumps we actually use
#define PLANES_LUMP 1
#define VERTICES_LUMP 2
#define NODES_LUMP 4
#define TEXTURES_LUMP 5
#define FACES_LUMP 6
#define LEAVES_LUMP 8
#define LEAF_FACES_LUMP 9
#define EDGES_LUMP 11
#define FACE_EDGES_LUMP 12

struct BSP_HEADER
{
    static const uint32_t c_Magic = 0x50534249; // "IBSP"
    static const uint32_t c_SupportedVersion = 38;
    uint32_t magic;
    uint32_t version; // should always be 38 for Quake2

    BSP_LUMP lumps[19]; // there are 19 data sections in the file
};

struct BSP_EDGE
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

struct BSP_FACE
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
// This means you can plug in a point p (x, y, z) and if you get
// 0    on the plane
// > 0  in front of the plane
// < 0  behind the plane
//
struct BSP_PLANE
{
    BSP_POINT3F normal;     // A, B, C components of equation
    float distance;         // D in equation
    uint32_t type; // not used in rendering, maybe contains collision flags or something?
};

struct BSP_NODE
{
    // bsp data
    uint32_t plane;         // index into plane array of splitting plane
    int32_t front_child;    // recursive index of front child. negative is leaf, -(index + 1)
    int32_t back_child;     // recursive index of back child. negative is leaf, -(index + 1)

    // bounding box of node
    BSP_POINT3S bbox_min;   // REMEMBER: z is UP here...
    BSP_POINT3S bbox_max;

    // faces contained in the node.
    // NOTE NOTE:
    // In the original game, this is only used for collision.
    // and the compressed PVS is used for rendering.
    // For GDK, we use these directly to generate the mesh, since we don't use PVS
    uint16_t first_face;
    uint16_t num_faces;
};

struct BSP_LEAF
{
    uint32_t brush_or; // ? unused for rendering...

    uint16_t cluster; // -1 (0xFFFF) for cluster indicates no PVS
    uint16_t area; // unused?

    BSP_POINT3S bbox_min;
    BSP_POINT3S bbox_max;

    uint16_t first_leaf_face; // leaf face array is just a level of uint16_t indirection over face array. Not sure why???
    uint16_t num_leaf_faces;

    uint16_t first_leaf_brush; // ? not used?
    uint16_t num_leaf_brushes;
};

struct BSP_TEXTUREINFO
{
    // to generate texture coordinates for vertex (x, y, z):
    // u = x * u_axis.x + y * u_axis.y + z * u_axis.z + u_offset;
    // v = x * v_axis.x + y * v_axis.y + z * v_axis.z + v_offset;

    BSP_POINT3F u_axis;
    float u_offset;
    BSP_POINT3F v_axis;
    float v_offset;

    uint32_t flags;
    uint32_t value;

    char texture_name[32];

    uint32_t next_texinfo; // animated textures reference each other :)
};

#pragma pack(pop)

HRESULT SaveBspAsObjectsUnderRoot(_In_z_ const wchar_t* pakFile, _In_count_(numBytes) const byte_t* data, _In_ uint32_t numBytes, _In_z_ const wchar_t* worldFolder);
