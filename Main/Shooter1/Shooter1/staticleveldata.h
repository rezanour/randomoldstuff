#pragma once

struct StaticGeometryVertex;
class BIH;
class BspCompiler;
class KdTreeCompiler2;
struct MaterialSource;
class TextureStreamer;

class StaticLevelData
{
public:
    StaticLevelData(const ComPtr<ID3D11DeviceContext>& context, _In_count_(numVertices) const StaticGeometryVertex* vertices, uint32_t numVertices,
                    _In_count_(numIndices) const uint32_t* indices, uint32_t numIndices, _Inout_ std::unique_ptr<MaterialSource[]>& materials, uint32_t numMaterials,
                    size_t textureBudgetBytes);
    ~StaticLevelData();

    void Draw(const XMFLOAT4X4& cameraWorld, const XMFLOAT4X4& view, const XMFLOAT4X4& projection);

private:
    struct MaterialMapEntry
    {
        uint32_t    Index;      // Diffuse, Normal is always immediately after it
        uint32_t    MipBias;    // float Bias value multiplied by 100 then cast to uint. Divide by 100 before applying
    };

    struct MaterialResidency
    {
        uint64_t    LastFrameUsed : 63;
        bool        Resident : 1;
    };

    struct PerFrame
    {
        XMFLOAT4X4  View;
        XMFLOAT4X4  Projection;
    };

private:
    StaticLevelData(const StaticLevelData&);

    void EnsureMaterial(uint32_t materialId);

private:
    //
    // Spatial partitioning
    //
    std::unique_ptr<BIH> _spatial;
    //std::unique_ptr<KdTreeCompiler2> _spatial;

    //
    // Resources
    //
    ComPtr<ID3D11DeviceContext> _context;
    ComPtr<ID3D11Buffer> _vertices;
    ComPtr<ID3D11Buffer> _indices;
    ComPtr<ID3D11Buffer> _perFrameCB;
    ComPtr<ID3D11VertexShader> _vertexShader;
    ComPtr<ID3D11PixelShader> _pixelShader;
    ComPtr<ID3D11InputLayout> _inputLayout;
    ComPtr<ID3D11ShaderResourceView> _textureArray;
    ComPtr<ID3D11ShaderResourceView> _materialMap;
    std::unique_ptr<uint32_t[]> _visibleIndices;
    std::unique_ptr<uint32_t[]> _visibleMaterials;
    uint32_t _numIndices;
    uint32_t _numVisibleIndices;
    uint32_t _numVisibleMaterials;
    PerFrame _perFrame;

    //
    // Materials
    //
    std::unique_ptr<MaterialMapEntry[]> _mapEntries;
    std::unique_ptr<MaterialResidency[]> _residency;
    uint32_t _numMaterials;
    uint32_t _textureArraySize;
    uint32_t _numResident;

    //
    // Streaming
    //
    std::unique_ptr<TextureStreamer> _textureStreamer;
};
