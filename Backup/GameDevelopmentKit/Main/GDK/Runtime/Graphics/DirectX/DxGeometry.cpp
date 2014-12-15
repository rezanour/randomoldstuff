#include "DxGeometry.h"
#include "DxGraphicsDevice.h"
#include <GDKError.h>

namespace GDK
{
    using Microsoft::WRL::ComPtr;

    std::shared_ptr<DxGeometry> DxGeometry::Create(_In_ const std::shared_ptr<GraphicsDevice>& graphicsDevice, _In_ const std::shared_ptr<GeometryContent>& content)
    {
        uint32_t numVertices = content->GetNumVertices();
        auto contentVertices = content->GetVertices();
        std::unique_ptr<Vertex[]> vertices(new Vertex[numVertices]);
        for (uint32_t i = 0; i < numVertices; ++i)
        {
            vertices.get()[i].position = *reinterpret_cast<const Vector3*>(&contentVertices[i].Position);
            vertices.get()[i].normal = *reinterpret_cast<const Vector3*>(&contentVertices[i].Normal);
            vertices.get()[i].texCoord = *reinterpret_cast<const Vector2*>(&contentVertices[i].TextureCoord0);
        }

        return std::shared_ptr<DxGeometry>(GDKNEW DxGeometry(graphicsDevice, content->GetNumFrames(), vertices.get(), numVertices, content->GetIndices(), content->GetNumIndices()));
    }

    std::shared_ptr<DxGeometry> DxGeometry::Create(_In_ const std::shared_ptr<GraphicsDevice>& graphicsDevice, _In_ uint32_t numFrames, _In_ const Vertex* vertices, _In_ uint32_t numVertices, _In_ const uint32_t* indices, _In_ uint32_t numIndices)
    {
        return std::shared_ptr<DxGeometry>(GDKNEW DxGeometry(graphicsDevice, numFrames, vertices, numVertices, indices, numIndices));
    }

    DxGeometry::DxGeometry(_In_ const std::shared_ptr<GraphicsDevice>& graphicsDevice, _In_ uint32_t numFrames, _In_ const Vertex* vertices, _In_ uint32_t numVertices, _In_ const uint32_t* indices, _In_ uint32_t numIndices) :
        _device(graphicsDevice),
        _numVertices(numVertices),
        _numIndices(numIndices),
        _numVerticesPerFrame(numVertices / numFrames),
        _numIndicesPerFrame(numIndices / numFrames),
        _numFrames(numFrames)
    {
        DxGraphicsDevice* dxGraphicsDevice = static_cast<DxGraphicsDevice*>(graphicsDevice.get());

        D3D11_BUFFER_DESC desc = {};
        desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        desc.StructureByteStride = sizeof(uint32_t);
        desc.ByteWidth = desc.StructureByteStride * numIndices;
        desc.Usage = D3D11_USAGE_DEFAULT;

        D3D11_SUBRESOURCE_DATA initialData = {};
        initialData.pSysMem = indices;
        initialData.SysMemPitch = desc.ByteWidth;
        initialData.SysMemSlicePitch = 0;

        CHECK_HR(dxGraphicsDevice->GetDevice()->CreateBuffer(&desc, &initialData, &_indices));

        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        desc.StructureByteStride = Geometry::Stride;
        desc.ByteWidth = desc.StructureByteStride * numVertices;

        initialData.pSysMem = vertices;
        initialData.SysMemPitch = desc.ByteWidth;
        initialData.SysMemSlicePitch = 0;

        CHECK_HR(dxGraphicsDevice->GetDevice()->CreateBuffer(&desc, &initialData, &_vertices));
    }

    void DxGeometry::Bind()
    {
        static uint32_t strides[] = { Stride };
        static uint32_t offsets[] = { 0 };

        ID3D11Buffer* vertexBuffers[] = { _vertices.Get() };

        DxGraphicsDevice* dxGraphicsDevice = static_cast<DxGraphicsDevice*>(_device.get());
        dxGraphicsDevice->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        dxGraphicsDevice->GetContext()->IASetIndexBuffer(_indices.Get(), DXGI_FORMAT_R32_UINT, 0);
        dxGraphicsDevice->GetContext()->IASetVertexBuffers(0, _countof(vertexBuffers), vertexBuffers, strides, offsets);
    }

    void DxGeometry::Unbind()
    {
        static ID3D11Buffer* vertexBuffers[] = { nullptr };
        static uint32_t zeros[] = { 0 };

        DxGraphicsDevice* dxGraphicsDevice = static_cast<DxGraphicsDevice*>(_device.get());
        dxGraphicsDevice->GetContext()->IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);
        dxGraphicsDevice->GetContext()->IASetVertexBuffers(0, _countof(vertexBuffers), vertexBuffers, zeros, zeros);
    }

    void DxGeometry::Draw(_In_ uint32_t frame)
    {
        CHECK_RANGE(frame, 0, _numFrames - 1);

        uint32_t baseVertex = frame * _numVerticesPerFrame;
        uint32_t baseIndex = frame * _numIndicesPerFrame;

        DxGraphicsDevice* dxGraphicsDevice = static_cast<DxGraphicsDevice*>(_device.get());
        dxGraphicsDevice->GetContext()->DrawIndexed(_numIndicesPerFrame, baseIndex, baseVertex);
    }
}
