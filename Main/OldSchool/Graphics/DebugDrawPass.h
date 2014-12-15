#pragma once

#include "RenderPass.h"

struct DebugLine;
struct DebugTriangle;

class DebugDrawPass : public BaseObject<DebugDrawPass>, public RenderPass
{
public:
    static std::shared_ptr<DebugDrawPass> Create(_In_ const std::shared_ptr<GraphicsSystem>& graphics, _In_ const ComPtr<ID3D11DeviceContext>& context, _In_ const std::shared_ptr<Texture>& target, _In_ const std::shared_ptr<Texture>& depth);

private:
    DebugDrawPass(_In_ const std::shared_ptr<GraphicsSystem>& graphics, _In_ const ComPtr<ID3D11DeviceContext>& context);

    void HandleDrawScene(_In_ const std::shared_ptr<IGraphicsScene>& scene, _In_ const XMFLOAT4X4& view, _In_ const XMFLOAT4X4& projection) override;

private:
    static const uint32_t MaxDebugVertices = 16000;
    ComPtr<ID3D11Buffer> _vertexBuffer;

    // Constants
    struct vsPerFrame
    {
        XMFLOAT4X4 ViewProjection;
    };
    vsPerFrame _vsPerFrame;

    std::vector<DebugLine> _debugLines;
    std::vector<DebugTriangle> _debugTriangles;
};
