#include <Precomp.h>
#include "GraphicsSystemImpl.h"
#include "GraphicsSceneImpl.h"
#include "GeometryImpl.h"
#include "TextureImpl.h"
#include "ModelImpl.h"
#include "GBufferPass.h"

_Use_decl_annotations_
std::shared_ptr<GBufferPass> GBufferPass::Create(const std::shared_ptr<GraphicsSystem>& graphics, const ComPtr<ID3D11DeviceContext>& context, const std::shared_ptr<Texture>& diffuse, const std::shared_ptr<Texture>& normals, const std::shared_ptr<Texture>& depth)
{
    return std::shared_ptr<GBufferPass>(new GBufferPass(graphics, context, diffuse, normals, depth));
}

_Use_decl_annotations_
GBufferPass::GBufferPass(const std::shared_ptr<GraphicsSystem>& graphics, const ComPtr<ID3D11DeviceContext>& context, const std::shared_ptr<Texture>& diffuse, const std::shared_ptr<Texture>& normals, const std::shared_ptr<Texture>& depth) :
    RenderPass(graphics, context)
{
    auto& device = GetDevice();

    LoadShaders(L"vsGBuffer.cso", VertexFormat::PositionNormalTangentTexture, L"psGBuffer.cso");

    SetSampler(0, graphics->GetLinearWrapSampler()); // Diffuse
    SetSampler(1, graphics->GetLinearWrapSampler()); // Normal
    SetSampler(2, graphics->GetLinearWrapSampler()); // Specular

    SetRenderTarget(0, diffuse);
    SetRenderTarget(1, normals);
    SetDepthBuffer(depth);
#if defined(ENABLE_ZPREPASS)
    SetDepthState(graphics->GetDepthReadNoWriteState());
#endif

    CreateVSConstantBuffer(0, sizeof(vsPerFrame));
    CreateVSConstantBuffer(1, sizeof(vsPerObject));

    const uint8_t nullDiffuse[] = { 255, 0, 255, 255 };
    _nullDiffuse = Texture::CreateTexture(device, 1, 1, DXGI_FORMAT_R8G8B8A8_UNORM, nullDiffuse, sizeof(uint32_t), false, false);

    const uint8_t nullNormal[] = { 128, 128, 255, 0 };
    _nullNormals = Texture::CreateTexture(device, 1, 1, DXGI_FORMAT_R8G8B8A8_UNORM, nullNormal, sizeof(uint32_t), false, false);

    const uint8_t nullSpecular[] = { 0, 0, 0, 0 };
    _nullSpecular = Texture::CreateTexture(device, 1, 1, DXGI_FORMAT_R8G8B8A8_UNORM, nullSpecular, sizeof(uint32_t), false, false);
}

void GBufferPass::HandleDrawScene(_In_ const std::shared_ptr<IGraphicsScene>& scene, _In_ const XMFLOAT4X4& view, _In_ const XMFLOAT4X4& projection)
{
    _vsPerFrame.View = view;
    _vsPerFrame.Projection = projection;
    UpdateVSConstantBuffer(0, &_vsPerFrame, sizeof(_vsPerFrame));

    auto& context = GetContext();

    auto theScene = static_cast<GraphicsScene*>(scene.get());
    theScene->GetAllOpaqueModels(_models);

    for (auto model : _models)
    {
        // If the model doesn't have an owner, it has no transform
        auto owner = model->GetOwner();
        if (owner == nullptr)
        {
            continue;
        }

        // If the model doesn't have any geometry, it can't be drawn
        auto geometry = model->GetGeometry();
        if (geometry == nullptr)
        {
            continue;
        }

        _vsPerObject.World = owner->GetTransform().GetWorld();
        UpdateVSConstantBuffer(1, &_vsPerObject, sizeof(_vsPerObject));

        auto diffuseTexture = static_cast<Texture*>(model->GetTexture(TextureUsage::Diffuse).get());
        auto normalTexture = static_cast<Texture*>(model->GetTexture(TextureUsage::Normal).get());
        auto specularTexture = static_cast<Texture*>(model->GetTexture(TextureUsage::Specular).get());

        SetShaderResource(0, diffuseTexture != nullptr ? diffuseTexture->shared_from_this() : _nullDiffuse);
        SetShaderResource(1, normalTexture != nullptr ? normalTexture->shared_from_this() : _nullNormals);
        SetShaderResource(2, specularTexture != nullptr ? specularTexture->shared_from_this() : _nullSpecular);
        UpdateShaderResources();

        auto geo = static_cast<Geometry*>(geometry.get());
        uint32_t stride = geo->GetVertexStride();
        uint32_t offset = 0;
        context->IASetIndexBuffer(geo->GetIndices().Get(), DXGI_FORMAT_R32_UINT, 0);
        context->IASetVertexBuffers(0, 1, geo->GetVertices().GetAddressOf(), &stride, &offset);
        context->DrawIndexed(geo->GetNumIndices(), 0, 0);
    }

    _models.clear();
}
