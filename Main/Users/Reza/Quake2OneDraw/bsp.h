#pragma once

class WalParser;

#pragma pack(push, 1)

// NOTE NOTE NOTE!!!
//
// All coordinates in the BSP use a coordinate system where x, y are the ground plane,
// and z is UP. This can be confusing when reading through the equations and math.
//
struct BSP_POINT3F { float x, y, z; };
struct BSP_POINT3S { int16_t x, y, z; };

#pragma pack(pop)


class Bsp
{
public:
    Bsp(const ComPtr<ID3D11Device>& device, const std::unique_ptr<Pak>& pak, const std::unique_ptr<uint8_t[]>& chunk);

    const ComPtr<ID3D11Buffer>& GetVertexBuffer() const { return _vertexBuffer; }
    const ComPtr<ID3D11ShaderResourceView>& GetTextures() const { return _srv; }

    void QueryVisibleTriangles(const XMFLOAT4X4& cameraWorld, const XMFLOAT4X4& projection, _Out_writes_to_(maxIndices, *numIndices) uint32_t* indices, uint32_t maxIndices, _Inout_ uint32_t* numIndices) const;

    static std::unique_ptr<D3D11_INPUT_ELEMENT_DESC[]> GetInputElements(_Out_ uint32_t* size);
    static uint32_t GetStride();

private:
    Bsp(const Bsp&);
    Bsp& operator= (const Bsp&);

private:
    struct Node
    {
        Node(const BSP_POINT3F& p, float d, const BSP_POINT3S& aabbMin, const BSP_POINT3S& aabbMax) :
            Plane(p.x, p.z, p.y, d), Min(aabbMin.x, aabbMin.z, aabbMin.y), Max(aabbMax.x, aabbMax.z, aabbMax.y)
        {}

        XMFLOAT3 Min, Max;

        XMFLOAT4 Plane;
        int32_t  Front;
        int32_t  Back;
    };

    struct Leaf
    {
        Leaf(const BSP_POINT3S& aabbMin, const BSP_POINT3S& aabbMax) :
            Min(aabbMin.x, aabbMin.z, aabbMin.y), Max(aabbMax.x, aabbMax.z, aabbMax.y)
        {}

        XMFLOAT3 Min, Max;
        uint32_t FirstTriangle;
        uint32_t NumTriangles;
    };

    struct Vertex
    {
        Vertex(const BSP_POINT3F& p, const BSP_POINT3F& n, const BSP_POINT3F& uAxis, float uOffset, const BSP_POINT3F& vAxis, float vOffset, uint32_t texIndex, uint32_t width, uint32_t height) :
            Position(p.x, p.z, p.y), Normal(n.x, n.z, n.y), TexIndex(texIndex)
        {
            TexCoord.x = (p.x * uAxis.x + p.y * uAxis.y + p.z * uAxis.z + uOffset) / (float)width;
            TexCoord.x -= floorf(TexCoord.x);
            TexCoord.x *= ((float)width / 512);

            TexCoord.y = (p.x * vAxis.x + p.y * vAxis.y + p.z * vAxis.z + vOffset) / (float)height;
            TexCoord.y -= floorf(TexCoord.y);
            TexCoord.y *= ((float)height / 512);
        }

        XMFLOAT3 Position;
        XMFLOAT3 Normal;
        XMFLOAT2 TexCoord;
        uint32_t TexIndex;
    };

    struct Triangle
    {
        Triangle(uint32_t i0, uint32_t i1, uint32_t i2) :
            i0(i0), i1(i1), i2(i2)
        {}

        uint32_t i0, i1, i2;
    };

    struct TextureInfo
    {
        uint32_t Width;
        uint32_t Height;
    };

    struct BspContext;

private:
    int32_t ParseNode(const ComPtr<ID3D11Device>& device, const std::unique_ptr<Pak>& pak, BspContext* bsp, int32_t index);
    void ProcessNode(const XMFLOAT3& position, const XMFLOAT3& aabbMin, const XMFLOAT3& aabbMax, int32_t index, _Out_writes_to_(maxIndices, *numIndices) uint32_t* indices, uint32_t maxIndices, _Inout_ uint32_t* numIndices) const;

private:
    int32_t _root;
    std::vector<Node> _nodes;
    std::vector<Leaf> _leaves;
    std::vector<Vertex> _vertices;
    std::vector<Triangle> _triangles;
    ComPtr<ID3D11Buffer> _vertexBuffer;
    ComPtr<ID3D11Texture2D> _textures;
    ComPtr<ID3D11ShaderResourceView> _srv;

    std::unique_ptr<WalParser> _walParser;
    std::vector<TextureInfo> _textureInfos;
    uint32_t _nextTexture;
};
