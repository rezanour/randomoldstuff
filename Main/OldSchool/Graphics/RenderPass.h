#pragma once

#include <GraphicsSystem.h>
#include <GraphicsScene.h>
#include "Shaders.h"

class GraphicsSystem;
class Geometry;
class Texture;

class RenderPass
{
public:
    virtual ~RenderPass();

    void DrawScene(_In_ const std::shared_ptr<IGraphicsScene>& scene, _In_ const XMFLOAT4X4& view, _In_ const XMFLOAT4X4& projection);

protected:
    RenderPass(_In_ const std::shared_ptr<GraphicsSystem>& graphics, _In_ const ComPtr<ID3D11DeviceContext>& context);

    virtual void HandleDrawScene(_In_ const std::shared_ptr<IGraphicsScene>& scene, _In_ const XMFLOAT4X4& view, _In_ const XMFLOAT4X4& projection) = 0;

    const ComPtr<ID3D11Device>& GetDevice() const;
    const ComPtr<ID3D11DeviceContext>& GetContext() const;

    // Configure pass
    void LoadShaders(_In_ const wchar_t* vertexShader, _In_ VertexFormat vertexFormat, _In_opt_ const wchar_t* pixelShader);
    void LoadShaders(_In_ const wchar_t* vertexShader, _In_ const std::vector<D3D11_INPUT_ELEMENT_DESC>& elems, _In_opt_ const wchar_t* pixelShader);

    void CreateVSConstantBuffer(_In_ uint32_t slot, _In_ uint32_t size);
    void CreatePSConstantBuffer(_In_ uint32_t slot, _In_ uint32_t size);

    void UpdateVSConstantBuffer(_In_ uint32_t slot, _In_ const void* data, _In_ uint32_t size);
    void UpdatePSConstantBuffer(_In_ uint32_t slot, _In_ const void* data, _In_ uint32_t size);

    void SetBlendState(_In_ const ComPtr<ID3D11BlendState>& blendState);
    void SetDepthState(_In_ const ComPtr<ID3D11DepthStencilState>& depthState);

    void SetSampler(_In_ uint32_t slot, _In_ const ComPtr<ID3D11SamplerState>& sampler);
    void SetShaderResource(_In_ uint32_t slot, _In_ const std::shared_ptr<Texture>& texture);
    void UpdateShaderResources();

    void SetRenderTarget(_In_ uint32_t slot, _In_ const std::shared_ptr<Texture>& texture);
    void SetRenderTarget(_In_ uint32_t slot, _In_ const ComPtr<ID3D11RenderTargetView>& rtv);
    void SetDepthBuffer(_In_ const std::shared_ptr<Texture>& texture);

    // Rendering
    void DrawFullScreenQuad();

private:
    void BeginPass();
    void EndPass();

private:
    std::weak_ptr<GraphicsSystem> _graphics;
    ComPtr<ID3D11Device> _device;
    ComPtr<ID3D11DeviceContext> _context;

    // Shaders
    ComPtr<ID3D11VertexShader> _vertexShader;
    ComPtr<ID3D11PixelShader> _pixelShader;

    // InputLayouts
    ComPtr<ID3D11InputLayout> _inputLayout;

    // Samplers
    ComPtr<ID3D11SamplerState> _samplers[4];

    // Blend State
    ComPtr<ID3D11BlendState> _blendState;

    // Depth State
    ComPtr<ID3D11DepthStencilState> _depthState;

    // Constant Buffers
    ComPtr<ID3D11Buffer> _vsConstantBuffers[4];
    ComPtr<ID3D11Buffer> _psConstantBuffers[4];

    // Shader Resource
    std::shared_ptr<Texture> _psShaderResources[4];

    // Render Targets
    ComPtr<ID3D11RenderTargetView> _renderTargets[4];

    // Depth Buffer
    std::shared_ptr<Texture> _depthBuffer;

    // Quad
    std::shared_ptr<Geometry> _quad;
};
