#include <Precomp.h>
#include "FullscreenPass.h"

_Use_decl_annotations_
std::shared_ptr<FullscreenPass> FullscreenPass::Create(
    const std::shared_ptr<GraphicsSystem>& graphics,
    const ComPtr<ID3D11DeviceContext>& context,
    const std::shared_ptr<Texture>& source1,
    const std::shared_ptr<Texture>& source2,
    const std::shared_ptr<Texture>& target,
    const wchar_t* pixelShader
    )
{
    std::shared_ptr<FullscreenPass> pass(new FullscreenPass(graphics, context));
    pass->LoadShaders(L"vsPassthrough.cso", VertexFormat::Position2DProj, pixelShader);
    pass->SetShaderResource(0, source1);
    pass->SetShaderResource(1, source2);
    pass->SetRenderTarget(0, target);
    return pass;
}

_Use_decl_annotations_
FullscreenPass::FullscreenPass(const std::shared_ptr<GraphicsSystem>& graphics, const ComPtr<ID3D11DeviceContext>& context) :
    RenderPass(graphics, context)
{
}

void FullscreenPass::HandleDrawScene(_In_ const std::shared_ptr<IGraphicsScene>& scene, _In_ const XMFLOAT4X4& view, _In_ const XMFLOAT4X4& projection)
{
    UNREFERENCED_PARAMETER(scene);
    UNREFERENCED_PARAMETER(view);
    UNREFERENCED_PARAMETER(projection);
}
