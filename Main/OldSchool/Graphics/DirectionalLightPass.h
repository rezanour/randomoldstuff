#pragma once

#include "RenderPass.h"

class Light;

class DirectionalLightPass : public BaseObject<DirectionalLightPass>, public RenderPass
{
public:
    static std::shared_ptr<DirectionalLightPass> Create(_In_ const std::shared_ptr<GraphicsSystem>& graphics, _In_ const ComPtr<ID3D11DeviceContext>& context,
                                                        _In_ const std::shared_ptr<Texture>& light, _In_ const std::shared_ptr<Texture>& diffuse,
                                                        _In_ const std::shared_ptr<Texture>& normals, _In_ const std::shared_ptr<Texture>& depth);

private:
    DirectionalLightPass(_In_ const std::shared_ptr<GraphicsSystem>& graphics, _In_ const ComPtr<ID3D11DeviceContext>& context,
                         _In_ const std::shared_ptr<Texture>& light, _In_ const std::shared_ptr<Texture>& diffuse,
                         _In_ const std::shared_ptr<Texture>& normals, _In_ const std::shared_ptr<Texture>& depth);

    void HandleDrawScene(_In_ const std::shared_ptr<IGraphicsScene>& scene, _In_ const XMFLOAT4X4& view, _In_ const XMFLOAT4X4& projection) override;

private:
    // Constants
    struct vsPerFrame
    {
        XMFLOAT4X4 InvProjection;
    };

    static const uint32_t LightsPerDraw = 8;

    struct psPerDraw
    {
        XMFLOAT4 Direction[LightsPerDraw];
        XMFLOAT4 Color[LightsPerDraw];
        uint32_t NumLights;
        XMFLOAT3 Padding;
    };

    vsPerFrame _vsPerFrame;
    psPerDraw _psPerDraw;

    // Scratch to avoid per frame allocations
    std::vector<std::shared_ptr<Light>> _lights;
};
