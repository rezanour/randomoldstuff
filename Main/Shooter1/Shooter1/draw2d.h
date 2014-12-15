#pragma once

#include "texture.h"

//
// NOTE: This class isn't meant to be used directly
// by game code. It's a helper object for Graphics.
//
class Draw2D : public TrackedObject<MemoryTag::Draw2D>
{
private:
    friend class Graphics;
    Draw2D(_In_ ID3D11DeviceContext* context);
    Draw2D(const Draw2D&);

    void Add(_In_ const Texture& texture, _In_ const RECT& source, _In_ const RECT& dest, _In_ const XMFLOAT4& color);
    void AddFullScreen(_In_ const Texture& texture, _In_ const RECT& source, _In_ const RECT& dest, _In_ const XMFLOAT4& color);
    void Draw();

private:
    struct PerFrame
    {
        XMFLOAT2 InvScreenSize;
        XMFLOAT2 CanvasOffset;
        uint32_t TotalInstances;
        XMUINT3 Padding;
    };

    struct Vertex2D
    {
        XMFLOAT2 Offset;
    };

    struct Instance2D
    {
        Instance2D() {}
        Instance2D(_In_ const RECT& source, _In_ const RECT& dest, _In_ const XMFLOAT4& color, uint32_t index, uint32_t textureIndex, bool ignoreTransform = false)
            :
            DestRect(dest.left, dest.top, dest.right, dest.bottom),
            SourceRect(source.left, source.top, source.right, source.bottom),
            Color(color),
            IndexAndTexIndex((index << 16) | textureIndex)
        {
            if (ignoreTransform)
            {
                IndexAndTexIndex |= 0x80000000;
            }
        }

        XMINT4 DestRect;
        XMINT4 SourceRect;
        XMFLOAT4 Color;
        // Index used to compute draw order/depth, and Texture index in array. Packed into single uint.
        // If high bit of index is set, ignore virtual canvas transform
        uint32_t IndexAndTexIndex;
    };

    struct Batch2D
    {
        Texture Texture;
        ComPtr<ID3D11Buffer> InstanceBuffer;
        std::unique_ptr<Instance2D[]> Instances;
        uint32_t NumInstances;
    };

    std::shared_ptr<Batch2D> FindBatch(_In_ const Texture& texture, _Inout_ std::vector<std::shared_ptr<Batch2D>>& batches);

private:
    ComPtr<ID3D11DeviceContext> _context;
    ComPtr<ID3D11InputLayout> _inputLayout;
    ComPtr<ID3D11Buffer> _vertexBuffer;
    ComPtr<ID3D11Buffer> _constantBuffer;
    ComPtr<ID3D11VertexShader> _vertexShader;
    ComPtr<ID3D11PixelShader> _pixelShader;
    ComPtr<ID3D11RasterizerState> _rasterizer;
    ComPtr<ID3D11BlendState> _blendState;

    PerFrame _constants;
    std::vector<std::shared_ptr<Batch2D>> _batches;             // sorted on TexturePool ID
    std::vector<std::shared_ptr<Batch2D>> _fullScreenBatches;   // sorted on TexturePool ID
    std::vector<std::shared_ptr<Batch2D>> _recycle;
    uint32_t _maxInstancesPerBatch;
    uint32_t _numInstances;
};
