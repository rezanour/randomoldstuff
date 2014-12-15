#pragma once

#include "RenderPass.h"

class FullscreenPass : public BaseObject<FullscreenPass>, public RenderPass
{
public:
    static std::shared_ptr<FullscreenPass> Create(
        _In_ const std::shared_ptr<GraphicsSystem>& graphics,
        _In_ const ComPtr<ID3D11DeviceContext>& context,
        _In_ const std::shared_ptr<Texture>& source1,
        _In_ const std::shared_ptr<Texture>& source2,
        _In_ const std::shared_ptr<Texture>& target,
        _In_z_ const wchar_t* pixelShader
        );

private:
    FullscreenPass(_In_ const std::shared_ptr<GraphicsSystem>& graphics, _In_ const ComPtr<ID3D11DeviceContext>& context);

    void HandleDrawScene(_In_ const std::shared_ptr<IGraphicsScene>& scene, _In_ const XMFLOAT4X4& view, _In_ const XMFLOAT4X4& projection) override;
};
