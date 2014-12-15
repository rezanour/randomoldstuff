#include <Precomp.h>
#include "..\VRES.h"
#include "Diffuse.h"
#include "DiffuseVS.h"
#include "DiffusePS.h"

using namespace VRES;

_Use_decl_annotations_
std::shared_ptr<Material> DiffuseMaterial::Create(VRES::Renderer* renderer, const ComPtr<ID3D11DeviceContext>& context)
{
    std::shared_ptr<Material> material(new DiffuseMaterial(renderer, context));
    if (!static_cast<DiffuseMaterial*>(material.get())->Initialize())
    {
        return nullptr;
    }

    return material;
}


_Use_decl_annotations_
DiffuseMaterial::DiffuseMaterial(VRES::Renderer* renderer, const ComPtr<ID3D11DeviceContext>& context) :
    Material("diffuse", renderer, context)
{
}

bool DiffuseMaterial::Initialize()
{
    if (!CreateVertexShader(VertexFormat::StaticMesh, g_DiffuseVS, _countof(g_DiffuseVS)))
    {
        return false;
    }

    if (!CreatePixelShader(g_DiffusePS, _countof(g_DiffusePS)))
    {
        return false;
    }

    if (!CreateCB(ShaderStage::Vertex, 0, sizeof(FrameConstants)))
    {
        return false;
    }

    if (!CreateCB(ShaderStage::Vertex, 1, sizeof(ObjectConstants)))
    {
        return false;
    }

    if (!CreateCB(ShaderStage::Pixel, 0, sizeof(LightConstants)))
    {
        return false;
    }

    SetSampler(ShaderStage::Pixel, 0, Renderer()->LinearWrapSampler());

    return true;
}

_Use_decl_annotations_
void DiffuseMaterial::RenderModels(const OutputDescription& output, const Model* const* models, uint32_t numModels, const Light* const* lights, uint32_t numLights)
{
    FrameConstants* frameConstants = (FrameConstants*)LockCB(ShaderStage::Vertex, 0);
    assert(frameConstants);
    frameConstants->View = output.View;
    frameConstants->Projection = output.Projection;
    UnlockCB(ShaderStage::Vertex, 0);

    LightConstants* lightConstants = (LightConstants*)LockCB(ShaderStage::Pixel, 0);
    assert(lightConstants);
    for (uint32_t i = 0; i < numLights; ++i)
    {
        lightConstants->Lights[i].Type = (uint32_t)lights[i]->Type();
        lightConstants->Lights[i].Color = lights[i]->Color();

        if (lights[i]->Type() == LightType::Directional)
        {
            lightConstants->Lights[i].Position = lights[i]->Transform().Forward();
        }
        else if (lights[i]->Type() == LightType::Point)
        {
            lightConstants->Lights[i].Position = lights[i]->Transform().Position();
            lightConstants->Lights[i].Radius = ((PointLight*)lights[i])->Radius();
        }
    }
    lightConstants->NumLights = numLights;

#if 0 // Use middle eye specular, instead of per-eye
    lightConstants->CameraPosition = output.CameraPosition;
#else
    XMVECTOR det;
    XMStoreFloat3(&lightConstants->CameraPosition, XMMatrixInverse(&det, XMLoadFloat4x4(&output.View)).r[3]);
#endif
    UnlockCB(ShaderStage::Pixel, 0);

    for (uint32_t i = 0; i < numModels; ++i)
    {
        assert(models[i]->Format() == VertexFormat::StaticMesh);

        ObjectConstants* objectConstants = (ObjectConstants*)LockCB(ShaderStage::Vertex, 1);
        assert(objectConstants);
        objectConstants->World = models[i]->Transform().World();
        UnlockCB(ShaderStage::Vertex, 1);

        static const uint32_t stride = sizeof(VertexStaticMesh);
        static const uint32_t offset = 0;
        Context()->IASetVertexBuffers(0, 1, models[i]->VertexBuffer().GetAddressOf(), &stride, &offset);
        Context()->IASetIndexBuffer(models[i]->IndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);

        ID3D11ShaderResourceView* srvs[] = { models[i]->Texture(TextureType::Diffuse).Get(), models[i]->Texture(TextureType::Normal).Get() };
        Context()->PSSetShaderResources(0, _countof(srvs), srvs);

        Context()->DrawIndexed(models[i]->NumIndices(), 0, 0);
    }
}
