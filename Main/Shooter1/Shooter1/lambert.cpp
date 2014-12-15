#include "precomp.h"
#include "materialpriv.h"
#include "lambert.h"

Lambert::Lambert() :
    Material(MaterialType::Matte)
{
    SetVertexShader("lambert_vs.cso", VertexFormat::StaticGeometry);
    SetPixelShader("lambert_ps.cso");

    D3D11_BUFFER_DESC bd = {};
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.ByteWidth = sizeof(PerFrame);
    bd.StructureByteStride = bd.ByteWidth;
    bd.Usage = D3D11_USAGE_DEFAULT;

    ComPtr<ID3D11Device> device;
    GetContext()->GetDevice(&device);

    CHECKHR(device->CreateBuffer(&bd, nullptr, &_perFrameCB));

    bd.ByteWidth = sizeof(PerObject);
    bd.StructureByteStride = bd.ByteWidth;
    CHECKHR(device->CreateBuffer(&bd, nullptr, &_perObjectCB));
}

_Use_decl_annotations_
void Lambert::Draw(const XMFLOAT4X4& view, const XMFLOAT4X4& projection, GameObject** objects, uint32_t numObjects)
{
    auto& context = GetContext();

    context->PSSetSamplers(0, 1, GetGraphics().GetLinearWrapSampler().GetAddressOf());

    ID3D11Buffer* cbs[] = { _perFrameCB.Get(), _perObjectCB.Get() };
    context->VSSetConstantBuffers(0, _countof(cbs), cbs);

    _perFrame.View = view;
    _perFrame.Projection = projection;
    context->UpdateSubresource(_perFrameCB.Get(), 0, nullptr, &_perFrame, sizeof(_perFrame), 0);

    uint32_t stride = 0;
    static const uint32_t offset = 0;
    for (uint32_t i = 0; i < numObjects; ++i)
    {
        auto& geometry = objects[i]->GetGeometry();
        if (!geometry.IsValid())
        {
            continue;
        }

        auto& texture = objects[i]->GetTexture(TextureType::Diffuse);
        if (!texture.IsValid())
        {
            // TODO: Supply bright pink or something to call this out
        }

        _perObject.World = objects[i]->GetWorldTransform();
        _perObject.TexIndex = texture.GetIndex();
        context->UpdateSubresource(_perObjectCB.Get(), 0, nullptr, &_perObject, sizeof(_perObject), 0);

        stride = geometry.GetPool()->GetStride();
        context->IASetVertexBuffers(0, 1, geometry.GetPool()->GetVerticesAddress(), &stride, &offset);
        context->IASetIndexBuffer(geometry.GetPool()->GetIndices(), DXGI_FORMAT_R32_UINT, 0);

        context->PSSetShaderResources(0, 1, texture.GetPool()->GetAddress());

        context->DrawIndexed(geometry.GetIndexCount(), geometry.GetIndicesIndex(), geometry.GetVerticesIndex());
    }
}
