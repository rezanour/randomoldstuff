#include <Precomp.h>
#include "GraphicsSystemImpl.h"
#include "GraphicsSceneImpl.h"
#include "TextureImpl.h"
#include "LightImpl.h"
#include "AmbientLightPass.h"

_Use_decl_annotations_
std::shared_ptr<AmbientLightPass> AmbientLightPass::Create(const std::shared_ptr<GraphicsSystem>& graphics, const ComPtr<ID3D11DeviceContext>& context,
    const std::shared_ptr<Texture>& diffuse, const std::shared_ptr<Texture>& target)
{
    std::shared_ptr<AmbientLightPass> pass(new AmbientLightPass(graphics, context));
    pass->SetShaderResource(0, diffuse);
    pass->SetRenderTarget(0, target);
    pass->SetBlendState(graphics->GetAdditiveBlendState());
    return pass;
}

_Use_decl_annotations_
AmbientLightPass::AmbientLightPass(const std::shared_ptr<GraphicsSystem>& graphics, const ComPtr<ID3D11DeviceContext>& context) :
    RenderPass(graphics, context)
{
    LoadShaders(L"vsPassthrough.cso", VertexFormat::Position2DProj, L"psAmbientLight.cso");
    CreatePSConstantBuffer(0, sizeof(_psPerDraw));
}

void AmbientLightPass::HandleDrawScene(_In_ const std::shared_ptr<IGraphicsScene>& scene, _In_ const XMFLOAT4X4& view, _In_ const XMFLOAT4X4& projection)
{
    UNREFERENCED_PARAMETER(view);
    UNREFERENCED_PARAMETER(projection);

    auto theScene = static_cast<GraphicsScene*>(scene.get());
    theScene->GetAllAmbientLights(_lights);
    for (auto light : _lights)
    {
        _psPerDraw.LightColor = light->GetColor();
        UpdatePSConstantBuffer(0, &_psPerDraw, sizeof(_psPerDraw));
        DrawFullScreenQuad();
    }
    _lights.clear();
}
