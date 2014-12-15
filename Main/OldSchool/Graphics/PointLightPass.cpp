#include <Precomp.h>
#include "GraphicsSystemImpl.h"
#include "GraphicsSceneImpl.h"
#include "GeometryImpl.h"
#include "TextureImpl.h"
#include "LightImpl.h"
#include "PointLightPass.h"

_Use_decl_annotations_
std::shared_ptr<PointLightPass> PointLightPass::Create(const std::shared_ptr<GraphicsSystem>& graphics, const ComPtr<ID3D11DeviceContext>& context,
                                                       const std::shared_ptr<Texture>& light, const std::shared_ptr<Texture>& diffuse,
                                                       const std::shared_ptr<Texture>& normals, const std::shared_ptr<Texture>& depth)
{
    return std::shared_ptr<PointLightPass>(new PointLightPass(graphics, context, light, diffuse, normals, depth));
}

_Use_decl_annotations_
PointLightPass::PointLightPass(const std::shared_ptr<GraphicsSystem>& graphics, const ComPtr<ID3D11DeviceContext>& context,
                               const std::shared_ptr<Texture>& light, const std::shared_ptr<Texture>& diffuse,
                               const std::shared_ptr<Texture>& normals, const std::shared_ptr<Texture>& depth) :
    RenderPass(graphics, context)
{
    auto& device = GetDevice();

    std::vector<D3D11_INPUT_ELEMENT_DESC> elems;
    GetVertexFormatElements(VertexFormat::Position3D, elems);

    D3D11_INPUT_ELEMENT_DESC elem = {};
    elem.InputSlot = 1;
    elem.InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
    elem.Format = DXGI_FORMAT_R32G32B32_FLOAT;
    elem.InstanceDataStepRate = 1;
    elem.SemanticName = "COLOR";
    elems.push_back(elem);

    elem.AlignedByteOffset = sizeof(XMFLOAT3);
    elem.SemanticName = "POSITION";
    elem.SemanticIndex = 1;
    elems.push_back(elem);

    elem.AlignedByteOffset += sizeof(XMFLOAT3);
    elem.Format = DXGI_FORMAT_R32_FLOAT;
    elem.SemanticName = "TEXCOORD";
    elem.SemanticIndex = 0;
    elems.push_back(elem);

    LoadShaders(L"vsPointLight.cso", elems, L"psPointLight.cso");

    SetShaderResource(0, diffuse);
    SetShaderResource(1, normals);
    SetShaderResource(2, depth);

    SetRenderTarget(0, light);

    SetBlendState(graphics->GetAdditiveBlendState());

    CreateVSConstantBuffer(0, sizeof(vsPerFrame));

    CreatePSConstantBuffer(0, sizeof(psPerFrame));

    _instances.reset(new PointLightInstance[MaxPointLightsPerDraw]);

    D3D11_BUFFER_DESC bd = {};
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.ByteWidth = sizeof(PointLightInstance) * MaxPointLightsPerDraw;
    bd.StructureByteStride = sizeof(PointLightInstance);
    bd.Usage = D3D11_USAGE_DEFAULT;

    HRESULT hr = device->CreateBuffer(&bd, nullptr, &_instanceBuffer);
    if (FAILED(hr))
    {
        throw std::exception();
    }

    Position3DVertex vertices[] = {
        { XMFLOAT3(-1.0f, -1.0f, 0.0f) },
        { XMFLOAT3(-1.0f, 1.0f, 0.0f) },
        { XMFLOAT3(1.0f, 1.0f, 0.0f) },
        { XMFLOAT3(1.0f, -1.0f, 0.0f) }
    };

    uint32_t indices[] =
    {
        0, 1, 2, 0, 2, 3
    };

    _quad = graphics->CreateGeometry(_countof(vertices), VertexFormat::Position3D, vertices, _countof(indices), indices);
}

void PointLightPass::HandleDrawScene(_In_ const std::shared_ptr<IGraphicsScene>& scene, _In_ const XMFLOAT4X4& view, _In_ const XMFLOAT4X4& projection)
{
    _vsPerFrame.View = view;
    _vsPerFrame.Projection = projection;
    UpdateVSConstantBuffer(0, &_vsPerFrame, sizeof(_vsPerFrame));

    _psPerFrame.Projection33 = projection._33;
    _psPerFrame.Projection43 = projection._43;
    UpdatePSConstantBuffer(0, &_psPerFrame, sizeof(_psPerFrame));

    auto theScene = static_cast<GraphicsScene*>(scene.get());
    theScene->GetAllPointLights(_lights);

    auto& context = GetContext();

    auto quad = static_cast<Geometry*>(_quad.get());
    context->IASetIndexBuffer(quad->GetIndices().Get(), DXGI_FORMAT_R32_UINT, 0);
    ID3D11Buffer* vbs[] = { quad->GetVertices().Get(), _instanceBuffer.Get() };
    uint32_t strides[] = { quad->GetVertexStride(), sizeof(PointLightInstance) };
    uint32_t offsets[] = { 0, 0 };
    context->IASetVertexBuffers(0, _countof(vbs), vbs, strides, offsets);

    uint32_t numInstances = 0;

    for (auto light : _lights)
    {
        auto& instance = _instances[numInstances++];
        instance.Color = light->GetColor();
        instance.Position = light->GetPosition();
        instance.Radius = light->GetRadius();

        if (numInstances == MaxPointLightsPerDraw)
        {
            // Draw batch
            context->UpdateSubresource(_instanceBuffer.Get(), 0, nullptr, _instances.get(), sizeof(PointLightInstance)* numInstances, 0);
            context->DrawIndexedInstanced(quad->GetNumIndices(), numInstances, 0, 0, 0);
            numInstances = 0;
        }
    }

    if (numInstances > 0)
    {
        // Draw remaining instances
        D3D11_BOX box = {};
        box.back = 1;
        box.right = numInstances * sizeof(PointLightInstance);
        box.bottom = 1;

        context->UpdateSubresource(_instanceBuffer.Get(), 0, &box, _instances.get(), sizeof(PointLightInstance)* numInstances, 0);
        context->DrawIndexedInstanced(quad->GetNumIndices(), numInstances, 0, 0, 0);
        numInstances = 0;
    }

    _lights.clear();
}
