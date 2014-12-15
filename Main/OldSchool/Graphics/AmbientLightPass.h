#pragma once

#include "RenderPass.h"

class Light;

class AmbientLightPass : public BaseObject<AmbientLightPass>, public RenderPass
{
public:
    static std::shared_ptr<AmbientLightPass> Create(_In_ const std::shared_ptr<GraphicsSystem>& graphics, _In_ const ComPtr<ID3D11DeviceContext>& context,
        _In_ const std::shared_ptr<Texture>& diffuse, _In_ const std::shared_ptr<Texture>& target);

private:
    AmbientLightPass(_In_ const std::shared_ptr<GraphicsSystem>& graphics, _In_ const ComPtr<ID3D11DeviceContext>& context);

    void HandleDrawScene(_In_ const std::shared_ptr<IGraphicsScene>& scene, _In_ const XMFLOAT4X4& view, _In_ const XMFLOAT4X4& projection) override;

private:
    struct psPerDraw
    {
        XMFLOAT3 LightColor;
        float Padding;
    };
    psPerDraw _psPerDraw;

    // Scratch to avoid per frame allocations
    std::vector<std::shared_ptr<Light>> _lights;
};
