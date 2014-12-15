#pragma once

class GraphicsSystem;
struct ITexture;

class Renderer2D : public BaseObject<Renderer2D>
{
public:
    static std::shared_ptr<Renderer2D> Create(_In_ const std::shared_ptr<GraphicsSystem>& system, _In_ const ComPtr<ID3D11DeviceContext>& context, _In_ const ComPtr<ID3D11RenderTargetView>& target, _In_ const ComPtr<ID3D11DepthStencilView>& depthBuffer);

    //
    // IRenderer2D
    //

    void DrawImage(_In_ const std::shared_ptr<ITexture>& texture, _In_ int32_t x, _In_ int32_t y, _In_ uint32_t width, _In_ uint32_t height);
    void DrawImage(_In_ const std::shared_ptr<ITexture>& texture, _In_ const RECT& source, _In_ const RECT& dest);

    //
    // Internal
    //

    // Render the batched contents of the renderer to the specified render target. This clears the batched contents for the next frame
    void Render();

private:
    Renderer2D(_In_ const std::shared_ptr<GraphicsSystem>& system, _In_ const ComPtr<ID3D11DeviceContext>& context, _In_ const ComPtr<ID3D11RenderTargetView>& target, _In_ const ComPtr<ID3D11DepthStencilView>& depthBuffer);

    void PrepareForFrame();

private:
    std::weak_ptr<GraphicsSystem> _system;
    ComPtr<ID3D11RenderTargetView> _target;
    ComPtr<ID3D11DepthStencilView> _depthBuffer;
    ComPtr<ID3D11DeviceContext> _context;

    ComPtr<ID3D11VertexShader> _vertexShader;
    ComPtr<ID3D11PixelShader> _pixelShader;
    ComPtr<ID3D11SamplerState> _sampler;
    ComPtr<ID3D11BlendState> _blendState;
    ComPtr<ID3D11InputLayout> _inputLayout;
    ComPtr<ID3D11Buffer> _quadVertices;
    ComPtr<ID3D11Buffer> _quadIndices;
    uint32_t _numIndices;
    float _rtWidth;
    float _rtHeight;

    struct Vertex2D
    {
        XMFLOAT2 Position;
        XMFLOAT2 TexCoord;
    };

    //
    // Instancing support
    //

    static const uint32_t MaxInstances = 16000;

    struct vsPerFrame
    {
        uint32_t NumInstances;
        XMFLOAT3 Padding;
    };
    vsPerFrame _vsPerFrame;
    ComPtr<ID3D11Buffer> _vsPerFrameBuffer;

    struct DrawInstance
    {
        XMFLOAT4 Source;
        XMFLOAT4 Dest;
        uint32_t InstanceId;
    };

    struct Batch
    {
        uint32_t NumInstances;
        ComPtr<ID3D11Buffer> InstanceBuffer;
        std::unique_ptr<DrawInstance[]> Instances;
    };

    std::map<ComPtr<ID3D11ShaderResourceView>, std::shared_ptr<Batch>> _batches;
    std::vector<std::shared_ptr<Batch>> _freeBatches;
};
