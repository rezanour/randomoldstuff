#pragma once

struct Vertex2D
{
    XMFLOAT2 Position;
};

struct Vertex
{
    XMFLOAT3 Position;
    XMFLOAT4 TexCoord;

    Vertex() :
        Position(0, 0, 0), TexCoord(0, 0, 0, 0)
    {}

    Vertex(const XMFLOAT3& position, const XMFLOAT2& texCoord, uint32_t textureIndex) :
        Position(position), TexCoord(texCoord.x, texCoord.y, textureIndex == 0 ? 1.0f : 0.0f, textureIndex == 1 ? 1.0f : 0.0f)
    {}
};

class Block3DRenderer : public IBlock3DRenderer
{
public:
    Block3DRenderer(_In_ ID3D11Device* device, _In_ ID3D11ShaderResourceView* walls, _In_ ID3D11ShaderResourceView* sprites, bool useLinearFiltering);

    void SetLevelColors(const XMFLOAT3& ceiling, const XMFLOAT3& floor) override;
    void SetWallLayout(_In_count_(64 * 64) const int16_t* data) override;

    // Render setup. Saves off current pipeline state and sets up camera.
    void Begin(const D3D11_VIEWPORT& viewport, const XMFLOAT4X4& view, const XMFLOAT4X4& projection, bool preservePipelineState) override;

    // Draw a billboarded sprite
    void DrawSprite(const XMFLOAT2& position, uint16_t spriteIndex) override;

    // Draw a dynamic wall cube at non-aligned position (secrets, etc...)
    void DrawCube(const XMFLOAT2& position, uint16_t wallIndex) override;

    // Draw a dynamic door
    void DrawDoor(const XMFLOAT2& position, bool horizontal, uint16_t imageIndex) override;

    // Complete rendering.
    void End() override;

private:
    Block3DRenderer(const Block3DRenderer&);
    Block3DRenderer& operator= (const Block3DRenderer&);

    void AddQuad(const XMFLOAT3& a, const XMFLOAT3& b, const XMFLOAT3& c, const XMFLOAT3& d, uint16_t imageIndex, uint32_t textureIndex, bool flipTextureHorizontally);
    void AddCube(const XMFLOAT2& position, const XMFLOAT2& halfWidths, uint16_t imageIndex);

private:
    // FL 9.1 only supports 16 bit indices, so we can only describe 64k vertices.
    // However, there are workloads with more than 64k vertices and we don't want
    // to split up the calls and send multiple packets to the GPU. Instead,
    // we just won't use indexing and see if that is a net win or not.
    static const uint32_t MaxQuads = 65536;
    static const uint32_t VertexStride = sizeof(Vertex);
    static const uint32_t VertexOffset = 0;

    ComPtr<ID3D11Device> _device;
    ComPtr<ID3D11DeviceContext> _context;
    XMFLOAT4 _ceilingColor;
    XMFLOAT4 _floorColor;
    D3D11_VIEWPORT _viewport;

    // Misc
    XMFLOAT2 _wallsSizeInBlocks;
    XMFLOAT2 _spritesSizeInBlocks;
    uint32_t _numStaticVertices;
    uint32_t _numVertices;
    std::unique_ptr<Vertex[]> _vertices;
    bool _preservePipelineState;

    // When view changes, we recompute relative corner offsets
    // for the given view. This is then used for every sprite added.
    XMFLOAT2 _billboardCorners[2];

    struct ClearConstants
    {
        XMFLOAT4 CeilingColor;
        XMFLOAT4 FloorColor;
    };

    struct Constants
    {
        XMFLOAT4X4  View;
        XMFLOAT4X4  Projection;
        XMFLOAT2    Texture1SizeInBlocks;
        XMFLOAT2    Texture2SizeInBlocks;
    };

    // Saved off pipeline state to restore after rendering
    D3D11_VIEWPORT _prevViewports[8];
    uint32_t _prevNumViewports;
    ComPtr<ID3D11InputLayout> _prevInputLayout;
    D3D11_PRIMITIVE_TOPOLOGY _prevTopology;
    ComPtr<ID3D11Buffer> _prevVB;
    uint32_t _prevStride;
    uint32_t _prevVBOffset;
    ComPtr<ID3D11Buffer> _prevCB;
    ComPtr<ID3D11Buffer> _prevPSCB;
    ComPtr<ID3D11VertexShader> _prevVertexShader;
    ComPtr<ID3D11ClassInstance> _prevVSClass;
    uint32_t _prevVSInstances;
    ComPtr<ID3D11PixelShader> _prevPixelShader;
    ComPtr<ID3D11ClassInstance> _prevPSClass;
    uint32_t _prevPSInstances;
    ComPtr<ID3D11ShaderResourceView> _prevSRV;
    ComPtr<ID3D11SamplerState> _prevSampler;
    ComPtr<ID3D11RasterizerState> _prevRS;
    ComPtr<ID3D11DepthStencilState> _prevDepthStencilState;
    UINT _prevStencilRef;

    // Pipeline state
    // Clearing the screen in two colors, requires a quad & shaders :(
    ComPtr<ID3D11InputLayout> _clearInputLayout;
    ComPtr<ID3D11Buffer> _clearVB;
    ComPtr<ID3D11Buffer> _clearConstantBuffer;
    ComPtr<ID3D11VertexShader> _clearVertexShader;
    ComPtr<ID3D11PixelShader> _clearPixelShader;
    // All rendering is in quads, using the same vertex & index buffer
    ComPtr<ID3D11InputLayout> _inputLayout;
    ComPtr<ID3D11Buffer> _vertexBuffer;
    ComPtr<ID3D11Buffer> _constantBuffer;
    ComPtr<ID3D11VertexShader> _vertexShader;
    ComPtr<ID3D11PixelShader> _pixelShader;
    ComPtr<ID3D11ShaderResourceView> _wallsSRV;
    ComPtr<ID3D11ShaderResourceView> _spritesSRV;
    ComPtr<ID3D11SamplerState> _pointSampler;
};
