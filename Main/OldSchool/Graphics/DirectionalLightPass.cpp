#include <Precomp.h>
#include "GraphicsSystemImpl.h"
#include "GraphicsSceneImpl.h"
#include "TextureImpl.h"
#include "LightImpl.h"
#include "DirectionalLightPass.h"

_Use_decl_annotations_
std::shared_ptr<DirectionalLightPass> DirectionalLightPass::Create(const std::shared_ptr<GraphicsSystem>& graphics, const ComPtr<ID3D11DeviceContext>& context,
                                                                   const std::shared_ptr<Texture>& light, const std::shared_ptr<Texture>& diffuse,
                                                                   const std::shared_ptr<Texture>& normals, const std::shared_ptr<Texture>& depth)
{
    return std::shared_ptr<DirectionalLightPass>(new DirectionalLightPass(graphics, context, light, diffuse, normals, depth));
}

_Use_decl_annotations_
DirectionalLightPass::DirectionalLightPass(const std::shared_ptr<GraphicsSystem>& graphics, const ComPtr<ID3D11DeviceContext>& context,
                                           const std::shared_ptr<Texture>& light, const std::shared_ptr<Texture>& diffuse,
                                           const std::shared_ptr<Texture>& normals, const std::shared_ptr<Texture>& depth) :
    RenderPass(graphics, context)
{
    UNREFERENCED_PARAMETER(depth);

    LoadShaders(L"vsPassthroughViewRay.cso", VertexFormat::Position2DProj, L"psDirectionalLight.cso");

    SetShaderResource(0, diffuse);
    SetShaderResource(1, normals);

    SetRenderTarget(0, light);

    SetBlendState(graphics->GetAdditiveBlendState());

    CreateVSConstantBuffer(0, sizeof(vsPerFrame));
    CreatePSConstantBuffer(0, sizeof(psPerDraw));
}

void DirectionalLightPass::HandleDrawScene(_In_ const std::shared_ptr<IGraphicsScene>& scene, _In_ const XMFLOAT4X4& view, _In_ const XMFLOAT4X4& projection)
{
    XMVECTOR det;
    XMStoreFloat4x4(&_vsPerFrame.InvProjection, XMMatrixInverse(&det, XMLoadFloat4x4(&projection)));
    UpdateVSConstantBuffer(0, &_vsPerFrame, sizeof(_vsPerFrame));

    XMMATRIX v = XMLoadFloat4x4(&view);

    auto theScene = static_cast<GraphicsScene*>(scene.get());
    theScene->GetAllDirectionalLights(_lights);

    // Draw lights in batches of 'LightsPerDraw' (8)
    uint32_t numLights = 0;
    for (auto light : _lights)
    {
        auto color = light->GetColor();
        auto direction = light->GetPosition();
        _psPerDraw.Color[numLights] = XMFLOAT4(color.x, color.y, color.z, 1.0f);
        _psPerDraw.Direction[numLights] = XMFLOAT4(direction.x, direction.y, direction.z, 0.0f);
        XMStoreFloat4(&_psPerDraw.Direction[numLights], XMVector3TransformNormal(XMLoadFloat4(&_psPerDraw.Direction[numLights]), v));

        if (++numLights == LightsPerDraw)
        {
            // Draw the batch
            _psPerDraw.NumLights = numLights;
            UpdatePSConstantBuffer(0, &_psPerDraw, sizeof(_psPerDraw));
            DrawFullScreenQuad();
            numLights = 0;
        }
    }

    if (numLights > 0)
    {
        // Draw remaining lights that didn't fit into a full batch
        _psPerDraw.NumLights = numLights;
        UpdatePSConstantBuffer(0, &_psPerDraw, sizeof(_psPerDraw));
        DrawFullScreenQuad();
    }

    _lights.clear();
}
