#pragma once

#include "RenderPass.h"

class Model;

class GBufferPass : public BaseObject<GBufferPass>, public RenderPass
{
public:
    static std::shared_ptr<GBufferPass> Create(_In_ const std::shared_ptr<GraphicsSystem>& graphics, _In_ const ComPtr<ID3D11DeviceContext>& context, _In_ const std::shared_ptr<Texture>& diffuse, _In_ const std::shared_ptr<Texture>& normals, _In_ const std::shared_ptr<Texture>& depth);

private:
    GBufferPass(_In_ const std::shared_ptr<GraphicsSystem>& graphics, _In_ const ComPtr<ID3D11DeviceContext>& context, _In_ const std::shared_ptr<Texture>& diffuse, _In_ const std::shared_ptr<Texture>& normals, _In_ const std::shared_ptr<Texture>& depth);

    void HandleDrawScene(_In_ const std::shared_ptr<IGraphicsScene>& scene, _In_ const XMFLOAT4X4& view, _In_ const XMFLOAT4X4& projection) override;

private:
    std::shared_ptr<Texture> _nullDiffuse;
    std::shared_ptr<Texture> _nullNormals;
    std::shared_ptr<Texture> _nullSpecular;

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
