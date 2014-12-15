#pragma once

#include "RenderPass.h"

class Light;

class PointLightPass : public BaseObject<PointLightPass>, public RenderPass
{
public:
    static std::shared_ptr<PointLightPass> Create(_In_ const std::shared_ptr<GraphicsSystem>& graphics, _In_ const ComPtr<ID3D11DeviceContext>& context,
                                                  _In_ const std::shared_ptr<Texture>& light, _In_ const std::shared_ptr<Texture>& diffuse,
                                                  _In_ const std::shared_ptr<Texture>& normals, _In_ const std::shared_ptr<Texture>& depth);

private:
    PointLightPass(_In_ const std::shared_ptr<GraphicsSystem>& graphics, _In_ const ComPtr<ID3D11DeviceContext>& context,
                   _In_ const std::shared_ptr<Texture>& light, _In_ const std::shared_ptr<Texture>& diffuse,
                   _In_ const std::shared_ptr<Texture>& normals, _In_ const std::shared_ptr<Texture>& depth);

    void HandleDrawScene(_In_ const std::shared_ptr<IGraphicsScene>& scene, _In_ const XMFLOAT4X4& view, _In_ const XMFLOAT4X4& projection) override;

private:
    std::shared_ptr<IGeometry> _quad;
    ComPtr<ID3D11Buffer> _instanceBuffer;

    struct PointLightInstance
    {
        XMFLOAT3 Color;
        XMFLOAT3 Position;
        float Radius;
    };

    static const uint32_t MaxPointLightsPerDraw = 1024;
    std::unique_ptr<PointLightInstance[]> _instances;

    // Constants
    struct vsPerFrame
    {
        XMFLOAT4X4 View;
        XMFLOAT4X4 Projection;
    };

    struct psPerFrame
    {
        float Projection33; // m33 of projection matrix
        float Projection43; // m43 of projection matrix
        XMFLOAT2 Padding;
    };

    vsPerFrame _vsPerFrame;
    psPerFrame _psPerFrame;

    // Scratch to avoid per frame allocations
    std::vector<std::shared_ptr<Light>> _lights;
};
