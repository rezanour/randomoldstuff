#pragma once

#include "RenderPass.h"

class Model;

class ZPrepass : public BaseObject<ZPrepass>, public RenderPass
{
public:
    static std::shared_ptr<ZPrepass> Create(_In_ const std::shared_ptr<GraphicsSystem>& graphics, _In_ const ComPtr<ID3D11DeviceContext>& context, _In_ const std::shared_ptr<Texture>& depth);

private:
    ZPrepass(_In_ const std::shared_ptr<GraphicsSystem>& graphics, _In_ const ComPtr<ID3D11DeviceContext>& context);

    void HandleDrawScene(_In_ const std::shared_ptr<IGraphicsScene>& scene, _In_ const XMFLOAT4X4& view, _In_ const XMFLOAT4X4& projection) override;

private:
    // Constants
    struct vsPerFrame
    {
        XMFLOAT4X4 View;
        XMFLOAT4X4 Projection;
    };

    struct vsPerObject
    {
        XMFLOAT4X4 World;
    };

    vsPerFrame _vsPerFrame;
    vsPerObject _vsPerObject;

    // Scratch to avoid per frame allocations
    std::vector<std::shared_ptr<Model>> _models;
};
