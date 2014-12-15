#pragma once

#include "RenderPass.h"

class RenderTransparentPass;
struct DebugLine;
struct DebugTriangle;

class BuildTransparentListPass : public BaseObject<BuildTransparentListPass>, public RenderPass
{
public:
    static std::shared_ptr<BuildTransparentListPass> Create(_In_ const std::shared_ptr<GraphicsSystem>& graphics, _In_ const ComPtr<ID3D11DeviceContext>& context, _In_ const std::shared_ptr<Texture>& target, _In_ const std::shared_ptr<Texture>& depth);

private:
    BuildTransparentListPass(_In_ const std::shared_ptr<GraphicsSystem>& graphics, _In_ const ComPtr<ID3D11DeviceContext>& context);

    void HandleDrawScene(_In_ const std::shared_ptr<IGraphicsScene>& scene, _In_ const XMFLOAT4X4& view, _In_ const XMFLOAT4X4& projection) override;

    // Private access to chain passes
    friend class RenderTransparentPass;

    const ComPtr<ID3D11Buffer>& GetNodesBuffer() const
    {
        return _nodesBuffer;
    }

    const ComPtr<ID3D11Buffer>& GetOffsetBuffer() const
    {
        return _startOffsetBuffer;
    }

private:
    ComPtr<ID3D11Buffer> _nodesBuffer;
    ComPtr<ID3D11Buffer> _startOffsetBuffer;
    ComPtr<ID3D11UnorderedAccessView> _nodesUAV;
    ComPtr<ID3D11UnorderedAccessView> _startOffsetUAV;
    ComPtr<ID3D11RenderTargetView> _debugRT;
    ComPtr<ID3D11DepthStencilView> _depth;

    static const uint32_t MaxDebugVertices = 16000;
    ComPtr<ID3D11Buffer> _vertexBuffer;
    std::vector<DebugLine> _debugLines;
    std::vector<DebugTriangle> _debugTriangles;

    struct vsPerFrame
    {
        XMFLOAT4X4 ViewProjection;
    };

    struct vsPerDraw
    {
        XMFLOAT4X4 World;
    };

    struct psPerFrame
    {
        uint32_t ScreenWidth;
        XMFLOAT3 Padding;
    };

    vsPerFrame _vsPerFrame;
    vsPerDraw _vsPerDraw;
    psPerFrame _psPerFrame;
};

class RenderTransparentPass : public BaseObject<RenderTransparentPass>, public RenderPass
{
public:
    static std::shared_ptr<RenderTransparentPass> Create(_In_ const std::shared_ptr<GraphicsSystem>& graphics, _In_ const ComPtr<ID3D11DeviceContext>& context, _In_ const std::shared_ptr<Texture>& target, _In_ const std::shared_ptr<Texture>& depth, _In_ const std::shared_ptr<Texture>& background, _In_ const std::shared_ptr<BuildTransparentListPass>& buildPass);

private:
    RenderTransparentPass(_In_ const std::shared_ptr<GraphicsSystem>& graphics, _In_ const ComPtr<ID3D11DeviceContext>& context);

    void HandleDrawScene(_In_ const std::shared_ptr<IGraphicsScene>& scene, _In_ const XMFLOAT4X4& view, _In_ const XMFLOAT4X4& projection) override;

private:
    ComPtr<ID3D11ShaderResourceView> _nodesBuffer;
    ComPtr<ID3D11ShaderResourceView> _startOffsetBuffer;
    ComPtr<ID3D11ShaderResourceView> _backgroundSRV;
    std::shared_ptr<Texture> _background;
    std::shared_ptr<Texture> _backgroundSource;

    struct psPerFrame
    {
        uint32_t ScreenWidth;
        XMFLOAT3 Padding;
    };

    psPerFrame _psPerFrame;
};
