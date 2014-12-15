#include <Precomp.h>
#include "GraphicsSystemImpl.h"
#include "GraphicsSceneImpl.h"
#include "TextureImpl.h"
#include "DebugDrawPass.h"

_Use_decl_annotations_
std::shared_ptr<DebugDrawPass> DebugDrawPass::Create(const std::shared_ptr<GraphicsSystem>& graphics, const ComPtr<ID3D11DeviceContext>& context, const std::shared_ptr<Texture>& target, const std::shared_ptr<Texture>& depth)
{
    std::shared_ptr<DebugDrawPass> pass(new DebugDrawPass(graphics, context));

    pass->SetRenderTarget(0, target);
    pass->SetDepthBuffer(depth);
    pass->SetBlendState(graphics->GetPremultipliedAlphaBlendState());

    return pass;
}

_Use_decl_annotations_
DebugDrawPass::DebugDrawPass(const std::shared_ptr<GraphicsSystem>& graphics, const ComPtr<ID3D11DeviceContext>& context) :
    RenderPass(graphics, context)
{
    auto& device = GetDevice();

    LoadShaders(L"vsDebugDraw.cso", VertexFormat::Position3DColor, L"psDebugDraw.cso");

    CreateVSConstantBuffer(0, sizeof(vsPerFrame));

    D3D11_BUFFER_DESC bd = {};
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.StructureByteStride = sizeof(Position3DColorVertex);
    bd.ByteWidth = bd.StructureByteStride * MaxDebugVertices;
    bd.Usage = D3D11_USAGE_DEFAULT;

    if (FAILED(device->CreateBuffer(&bd, nullptr, &_vertexBuffer)))
    {
        throw std::exception();
    }
}

_Use_decl_annotations_
void DebugDrawPass::HandleDrawScene(const std::shared_ptr<IGraphicsScene>& scene, const XMFLOAT4X4& view, const XMFLOAT4X4& projection)
{
    ProfileMark profile(CodeTag::DebugRendering);

    XMStoreFloat4x4(&_vsPerFrame.ViewProjection, XMMatrixMultiply(XMLoadFloat4x4(&view), XMLoadFloat4x4(&projection)));
    UpdateVSConstantBuffer(0, &_vsPerFrame, sizeof(_vsPerFrame));

    GraphicsScene* theScene = static_cast<GraphicsScene*>(scene.get());

    auto& context = GetContext();

    uint32_t stride = GetVertexSize(VertexFormat::Position3DColor);
    uint32_t offset = 0;
    context->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);

    XMVECTOR det;
    XMMATRIX invView = XMMatrixInverse(&det, XMLoadFloat4x4(&view));
    static XMVECTOR cameraPos;
    static XMVECTOR cameraForward;

    cameraPos = invView.r[3];
    cameraForward = invView.r[2];

    theScene->GetDebugTriangles(_debugTriangles);
    if (_debugTriangles.size() > 0)
    {
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        std::qsort(_debugTriangles.data(), _debugTriangles.size(), sizeof(DebugTriangle),
            [](const void* p0, const void* p1)
            {
                const DebugTriangle* t0 = reinterpret_cast<const DebugTriangle*>(p0);
                const DebugTriangle* t1 = reinterpret_cast<const DebugTriangle*>(p1);

                XMVECTOR c0, c1;
                c0 = XMVectorScale(XMVectorAdd(XMVectorAdd(XMLoadFloat3(&t0->v0.Position), XMLoadFloat3(&t0->v1.Position)), XMLoadFloat3(&t0->v2.Position)), 0.3333f);
                c1 = XMVectorScale(XMVectorAdd(XMVectorAdd(XMLoadFloat3(&t1->v0.Position), XMLoadFloat3(&t1->v1.Position)), XMLoadFloat3(&t1->v2.Position)), 0.3333f);
                XMVECTOR z0 = XMVector3Dot(XMVectorSubtract(c0, cameraPos), cameraForward);
                XMVECTOR z1 = XMVector3Dot(XMVectorSubtract(c1, cameraPos), cameraForward);
                return XMVector3Less(z0, z1) ? 1 : -1;
            });

        uint32_t numVertices = min(static_cast<uint32_t>(_debugTriangles.size() * 3), MaxDebugVertices);

        D3D11_BOX box = {};
        box.right = numVertices * stride;
        box.bottom = 1;
        box.back = 1;
        context->UpdateSubresource(_vertexBuffer.Get(), 0, &box, _debugTriangles.data(), box.right, 0);

        context->Draw(numVertices, 0);
        _debugLines.clear();
    }

    theScene->GetDebugLines(_debugLines);
    if (_debugLines.size() > 0)
    {
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

        uint32_t numVertices = min(static_cast<uint32_t>(_debugLines.size() * 2), MaxDebugVertices);

        D3D11_BOX box = {};
        box.right = numVertices * stride;
        box.bottom = 1;
        box.back = 1;
        context->UpdateSubresource(_vertexBuffer.Get(), 0, &box, _debugLines.data(), box.right, 0);

        context->Draw(numVertices, 0);
        _debugLines.clear();
    }
}
