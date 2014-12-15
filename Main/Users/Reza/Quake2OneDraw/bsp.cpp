#include "precomp.h"
#include "wal.h"
#include <DirectXCollision.h>

#pragma pack(push, 1)

// a table entry for a "data section" or lump in the file
struct BSP_LUMP
{
    uint32_t offset;
    uint32_t length;
};

/*
    Lump descriptions

    NOTE    Taken from www.flipcode.com's bsp description.
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
    13      Models                  List of roots of each sub model, model 0 == main static geometry
    14      Brushes                 ?
    15      Brush Sides             ?
    16      Pop                     ?
    17      Areas                   ?
    18      Area Portals            ?
*/

// Lumps we actually use
static const uint32_t PLANES_LUMP = 1;
static const uint32_t VERTICES_LUMP = 2;
static const uint32_t NODES_LUMP = 4;
static const uint32_t TEXTURES_LUMP = 5;
static const uint32_t FACES_LUMP = 6;
static const uint32_t LEAVES_LUMP = 8;
static const uint32_t LEAF_FACES_LUMP = 9;
static const uint32_t EDGES_LUMP = 11;
static const uint32_t FACE_EDGES_LUMP = 12;
static const uint32_t MODEL_LUMP = 13;

struct BSP_HEADER
{
    static const uint32_t c_Magic = 0x50534249; // "IBSP"
    static const uint32_t c_SupportedVersion = 38;
 
    uint32_t magic;
    uint32_t version; // should always be 38 for Quake2

    BSP_LUMP lumps[19]; // there are 19 data sections in the file
};

// No direction can be assumed, as an edge can be shared between
// two faces with different winding. Face contains direction info
struct BSP_EDGE { uint16_t v0, v1; };

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
    uint32_t type;          // not used in rendering, maybe contains collision flags or something?
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
    uint32_t brush_or;  // ? unused for rendering...

    uint16_t cluster;   // -1 (0xFFFF) for cluster indicates no PVS
    uint16_t area;      // unused?

    BSP_POINT3S bbox_min;
    BSP_POINT3S bbox_max;

    uint16_t first_leaf_face;   // leaf face array is just a level of uint16_t indirection over face array. Not sure why???
    uint16_t num_leaf_faces;

    uint16_t first_leaf_brush;  // ? not used?
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

struct BSP_MODEL
{
    float minx, miny, minz;
    float maxx, maxy, maxz;
    float x, y, z; // origin
    int32_t head;
    uint32_t firstFace;
    uint32_t numFaces;
};

#pragma pack(pop)

struct Bsp::BspContext
{
    BSP_PLANE* planes;
    BSP_POINT3F* vertices;
    BSP_NODE* nodes;
    BSP_TEXTUREINFO* textures;
    BSP_FACE* faces;
    BSP_LEAF* leaves;
    uint16_t* leafFaces;
    BSP_EDGE* edges;
    int32_t* faceEdges;
    BSP_MODEL* models;
};

Bsp::Bsp(const ComPtr<ID3D11Device>& device, const std::unique_ptr<Pak>& pak, const std::unique_ptr<uint8_t[]>& chunk) :
    _nextTexture(0)
{
    // Init WalParser
    uint32_t size;
    auto palChunk = pak->GetChunk("pics/colormap.pcx", &size);
    _walParser.reset(new WalParser(palChunk, size));

    // read the header
    const BSP_HEADER* header = reinterpret_cast<const BSP_HEADER*>(chunk.get());

    // check magic number & version
    CHECKTRUE(header->magic == header->c_Magic);
    CHECKTRUE(header->version == header->c_SupportedVersion);

    // get pointers to the lumps
    BspContext bsp;
    bsp.planes = reinterpret_cast<BSP_PLANE*>(chunk.get() + header->lumps[PLANES_LUMP].offset);
    bsp.vertices = reinterpret_cast<BSP_POINT3F*>(chunk.get() + header->lumps[VERTICES_LUMP].offset);
    bsp.nodes = reinterpret_cast<BSP_NODE*>(chunk.get() + header->lumps[NODES_LUMP].offset);
    bsp.textures = reinterpret_cast<BSP_TEXTUREINFO*>(chunk.get() + header->lumps[TEXTURES_LUMP].offset);
    bsp.faces = reinterpret_cast<BSP_FACE*>(chunk.get() + header->lumps[FACES_LUMP].offset);
    bsp.leaves = reinterpret_cast<BSP_LEAF*>(chunk.get() + header->lumps[LEAVES_LUMP].offset);
    bsp.leafFaces = reinterpret_cast<uint16_t*>(chunk.get() + header->lumps[LEAF_FACES_LUMP].offset);
    bsp.edges = reinterpret_cast<BSP_EDGE*>(chunk.get() + header->lumps[EDGES_LUMP].offset);
    bsp.faceEdges = reinterpret_cast<int32_t*>(chunk.get() + header->lumps[FACE_EDGES_LUMP].offset);
    bsp.models = reinterpret_cast<BSP_MODEL*>(chunk.get() + header->lumps[MODEL_LUMP].offset);

    D3D11_TEXTURE2D_DESC td = {};
    td.ArraySize = 128;
    td.Width = 512;
    td.Height = 512;
    td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    td.MipLevels = 1;
    td.SampleDesc.Count = 1;
    td.Usage = D3D11_USAGE_DEFAULT;

    CHECKHR(device->CreateTexture2D(&td, nullptr, &_textures));
    CHECKHR(device->CreateShaderResourceView(_textures.Get(), nullptr, &_srv));

    // generate geometry & textures for model 0 (main static level)
    _root = ParseNode(device, pak, &bsp, bsp.models[0].head);

    D3D11_BUFFER_DESC bd = {};
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.ByteWidth = GetStride() * _vertices.size();
    bd.StructureByteStride = GetStride();
    bd.Usage = D3D11_USAGE_DEFAULT;

    D3D11_SUBRESOURCE_DATA init = {};
    init.pSysMem = _vertices.data();
    init.SysMemPitch = bd.ByteWidth;

    CHECKHR(device->CreateBuffer(&bd, &init, &_vertexBuffer));
}

_Use_decl_annotations_
std::unique_ptr<D3D11_INPUT_ELEMENT_DESC[]> Bsp::GetInputElements(uint32_t* size)
{
    *size = 4;
    std::unique_ptr<D3D11_INPUT_ELEMENT_DESC[]> elems(new D3D11_INPUT_ELEMENT_DESC[4]);

    ZeroMemory(elems.get(), 4 * sizeof(D3D11_INPUT_ELEMENT_DESC));
    elems[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    elems[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    elems[0].SemanticName = "POSITION";

    elems[1].AlignedByteOffset = sizeof(XMFLOAT3);
    elems[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    elems[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    elems[1].SemanticName = "NORMAL";

    elems[2].AlignedByteOffset = 2 * sizeof(XMFLOAT3);
    elems[2].Format = DXGI_FORMAT_R32G32_FLOAT;
    elems[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    elems[2].SemanticName = "TEXCOORD";

    elems[3].AlignedByteOffset = 2 * sizeof(XMFLOAT3) + sizeof(XMFLOAT2);
    elems[3].Format = DXGI_FORMAT_R32_UINT;
    elems[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    elems[3].SemanticIndex = 1;
    elems[3].SemanticName = "TEXCOORD";

    return elems;
}

uint32_t Bsp::GetStride()
{
    return sizeof(Vertex);
}

_Use_decl_annotations_
void Bsp::QueryVisibleTriangles(const XMFLOAT4X4& cameraWorld, const XMFLOAT4X4& projection, uint32_t* indices, uint32_t maxIndices, uint32_t* numIndices) const
{
    *numIndices = 0;

#if 0
    // Brute force test

    UNREFERENCED_PARAMETER(position);

    const Triangle* triangle = _triangles.data();
    for (size_t i = 0; i < _triangles.size() && (*numIndices + 3 <= maxIndices); ++i, ++triangle)
    {
        indices[*numIndices] = triangle->i0;
        ++(*numIndices);
        indices[*numIndices] = triangle->i1;
        ++(*numIndices);
        indices[*numIndices] = triangle->i2;
        ++(*numIndices);
    }
#else

    XMFLOAT3 position;
    XMMATRIX world = XMLoadFloat4x4(&cameraWorld);
    XMStoreFloat3(&position, world.r[3]);

    BoundingFrustum frustum(XMLoadFloat4x4(&projection));
    frustum.Transform(frustum, world);

    XMFLOAT3 Corners[8];
    frustum.GetCorners(Corners);

    BoundingBox box;
    BoundingBox::CreateFromPoints(box, 8, Corners, sizeof(XMFLOAT3));

    XMFLOAT3 aabbMin = XMFLOAT3(box.Center.x - box.Extents.x, box.Center.y - box.Extents.y, box.Center.z - box.Extents.z);
    XMFLOAT3 aabbMax = XMFLOAT3(box.Center.x + box.Extents.x, box.Center.y + box.Extents.y, box.Center.z + box.Extents.z);
    ProcessNode(position, aabbMin, aabbMax, _root, indices, maxIndices, numIndices);

#endif
}

int32_t Bsp::ParseNode(const ComPtr<ID3D11Device>& device, const std::unique_ptr<Pak>& pak, BspContext* bsp, int32_t index)
{
    if (index >= 0)
    {
        BSP_NODE* n = &bsp->nodes[index];
        BSP_PLANE* p = &bsp->planes[n->plane];

        Node node(p->normal, p->distance, n->bbox_min, n->bbox_max);
        node.Front = ParseNode(device, pak, bsp, n->front_child);
        node.Back = ParseNode(device, pak, bsp, n->back_child);

        _nodes.push_back(node);
        return _nodes.size() - 1;
    }
    else
    {
        BSP_LEAF* l = &bsp->leaves[index * -1 - 1];
        Leaf leaf(l->bbox_min, l->bbox_max);

        leaf.FirstTriangle = (uint32_t)_triangles.size();

        for (uint16_t i = 0; i < l->num_leaf_faces; ++i)
        {
            BSP_FACE* f = &bsp->faces[bsp->leafFaces[l->first_leaf_face + i]];
            BSP_TEXTUREINFO* tex = &bsp->textures[f->texture_info];

            uint32_t width = 512, height = 512;

            // Hack, we use a special flag value to indicate that we've seen
            // this texture before, and then teh value member to store what index
            if (tex->flags != 0xdeadbeef)
            {
                tex->flags = 0xdeadbeef;
                tex->value = _nextTexture++;

                uint32_t size;
                char textureName[200] = {};
                sprintf_s(textureName, "textures/%s.wal", tex->texture_name);
                auto chunk = pak->GetChunk(textureName, &size);

                _walParser->LoadWalIntoTextureIndex(device, _textures, tex->value, chunk, &width, &height);

                _textureInfos.emplace_back(TextureInfo { width, height });
            }
            else
            {
                width = _textureInfos[tex->value].Width;
                height = _textureInfos[tex->value].Height;
            }

            BSP_PLANE* plane = &bsp->planes[f->plane];

            uint32_t firstVertex = (uint32_t)_vertices.size();

            for (uint16_t j = 0; j < f->num_edges; ++j)
            {
                int32_t edgeIndex = bsp->faceEdges[f->first_edge + j];
                BSP_POINT3F v0;
                BSP_POINT3F v1;
                if (edgeIndex >= 0)
                {
                    BSP_EDGE* edge = &bsp->edges[edgeIndex];
                    v0 = bsp->vertices[edge->v0];
                    v1 = bsp->vertices[edge->v1];
                }
                else
                {
                    BSP_EDGE* edge = &bsp->edges[-edgeIndex];
                    v0 = bsp->vertices[edge->v1];
                    v1 = bsp->vertices[edge->v0];
                }

                if (j == 0)
                {
                    _vertices.push_back(Vertex(v0, plane->normal, tex->u_axis, tex->u_offset, tex->v_axis, tex->v_offset, tex->value, width, height));
                }
                _vertices.push_back(Vertex(v1, plane->normal, tex->u_axis, tex->u_offset, tex->v_axis, tex->v_offset, tex->value, width, height));

                if (j > 0)
                {
                    _triangles.push_back(Triangle(firstVertex, (uint32_t)_vertices.size() - 2, (uint32_t)_vertices.size() - 1));
                }
            }
        }

        leaf.NumTriangles = (uint32_t)_triangles.size() - leaf.FirstTriangle;
        if (leaf.NumTriangles == 0 && leaf.FirstTriangle > 0)
        {
            leaf.FirstTriangle--;
        }
        _leaves.push_back(leaf);
        return (int32_t)_leaves.size() * -1;
    }
}

_Use_decl_annotations_
void Bsp::ProcessNode(const XMFLOAT3& position, const XMFLOAT3& aabbMin, const XMFLOAT3& aabbMax, int32_t index, uint32_t* indices, uint32_t maxIndices, uint32_t* numIndices) const
{
    if (index >= 0)
    {
        const Node* node = &_nodes[index];

        if (node->Min.x > aabbMax.x || node->Min.y > aabbMax.y || node->Min.z > aabbMax.z ||
            aabbMin.x > node->Max.x || aabbMin.y > node->Max.y || aabbMin.y > node->Max.y)
        {
            return;
        }

        if (position.x * node->Plane.x + position.y * node->Plane.y + position.z * node->Plane.z - node->Plane.w >= 0)
        {
            ProcessNode(position, aabbMin, aabbMax, node->Front, indices, maxIndices, numIndices);
            ProcessNode(position, aabbMin, aabbMax, node->Back, indices, maxIndices, numIndices);
        }
        else
        {
            ProcessNode(position, aabbMin, aabbMax, node->Back, indices, maxIndices, numIndices);
            ProcessNode(position, aabbMin, aabbMax, node->Front, indices, maxIndices, numIndices);
        }
    }
    else
    {
        const Leaf* leaf = &_leaves[index * -1 -1];

        if (leaf->Min.x > aabbMax.x || leaf->Min.y > aabbMax.y || leaf->Min.z > aabbMax.z ||
            aabbMin.x > leaf->Max.x || aabbMin.y > leaf->Max.y || aabbMin.y > leaf->Max.y)
        {
            return;
        }

        const Triangle* triangle = &_triangles[leaf->FirstTriangle];
        for (uint32_t i = 0; i < leaf->NumTriangles && (*numIndices + 3 <= maxIndices); ++i, ++triangle)
        {
            indices[*numIndices] = triangle->i0;
            ++(*numIndices);
            indices[*numIndices] = triangle->i1;
            ++(*numIndices);
            indices[*numIndices] = triangle->i2;
            ++(*numIndices);
        }
    }
}
