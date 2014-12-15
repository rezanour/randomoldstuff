#include <Precomp.h>
#include "GraphicsSystemImpl.h"
#include "GraphicsSceneImpl.h"
#include "GeometryImpl.h"
#include "TextureImpl.h"
#include "ModelImpl.h"
#include "ZPrepass.h"

_Use_decl_annotations_
std::shared_ptr<ZPrepass> ZPrepass::Create(const std::shared_ptr<GraphicsSystem>& graphics, const ComPtr<ID3D11DeviceContext>& context, const std::shared_ptr<Texture>& depth)
{
    std::shared_ptr<ZPrepass> pass(new ZPrepass(graphics, context));
    pass->SetDepthBuffer(depth);
    return pass;
}

_Use_decl_annotations_
ZPrepass::ZPrepass(const std::shared_ptr<GraphicsSystem>& graphics, const ComPtr<ID3D11DeviceContext>& context) :
RenderPass(graphics, context)
{
    LoadShaders(L"vsZPrepass.cso", VertexFormat::Position3D, nullptr);
    CreateVSConstantBuffer(0, sizeof(vsPerFrame));
    CreateVSConstantBuffer(1, sizeof(vsPerObject));
}

void ZPrepass::HandleDrawScene(_In_ const std::shared_ptr<IGraphicsScene>& scene, _In_ const XMFLOAT4X4& view, _In_ const XMFLOAT4X4& projection)
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

        auto geo = static_cast<Geometry*>(geometry.get());
        uint32_t stride = geo->GetVertexStride();
        uint32_t offset = 0;
        context->IASetIndexBuffer(geo->GetIndices().Get(), DXGI_FORMAT_R32_UINT, 0);
        context->IASetVertexBuffers(0, 1, geo->GetVertices().GetAddressOf(), &stride, &offset);
        context->DrawIndexed(geo->GetNumIndices(), 0, 0);
    }

    _models.clear();
}
