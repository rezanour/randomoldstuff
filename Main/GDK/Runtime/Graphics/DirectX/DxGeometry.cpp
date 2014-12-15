#include "DxGeometry.h"
#include "DxGraphicsDevice.h"
#include <GDKError.h>

namespace GDK
{
    using Microsoft::WRL::ComPtr;

    std::shared_ptr<DxGeometry> DxGeometry::Create(_In_ const std::shared_ptr<GraphicsDevice>& graphicsDevice, _In_ const std::shared_ptr<GeometryContent>& content)
    {
        uint32_t stride = content->GetVertexStride();
        uint32_t numVertices = content->GetNumVertices();

        const byte_t* positions = content->GetAttributeData(GeometryContent::AttributeType::Float3, GeometryContent::AttributeName::Position, 0);
        const byte_t* normals = content->GetAttributeData(GeometryContent::AttributeType::Float3, GeometryContent::AttributeName::Normal, 0);
        const byte_t* texCoords = content->GetAttributeData(GeometryContent::AttributeType::Float2, GeometryContent::AttributeName::TexCoord, 0);

        CHECK_NOT_NULL(positions);
        CHECK_NOT_NULL(normals);
        CHECK_NOT_NULL(texCoords);

        std::unique_ptr<Vertex3D[]> vertices(new Vertex3D[numVertices]);
        for (uint32_t i = 0; i < numVertices; ++i)
        {
            Float3 pos = *reinterpret_cast<const Float3*>(positions);
            Float3 norm = *reinterpret_cast<const Float3*>(normals);
            Float2 tex = *reinterpret_cast<const Float2*>(texCoords);

            positions += stride;
            normals += stride;
            texCoords += stride;

            vertices.get()[i] = Vertex3D(pos, norm, tex);
        }

        return std::shared_ptr<DxGeometry>(GDKNEW DxGeometry(graphicsDevice, Type::Geometry3D, content->GetNumFrames(), vertices.get(), numVertices, content->GetIndices(), content->GetNumIndices()));
    }

    std::shared_ptr<DxGeometry> DxGeometry::Create(_In_ const std::shared_ptr<GraphicsDevice>& graphicsDevice, _In_ Type type, _In_ uint32_t numFrames, _In_ const void* vertices, _In_ uint32_t numVertices, _In_ const uint32_t* indices, _In_ uint32_t numIndices)
    {
        return std::shared_ptr<DxGeometry>(GDKNEW DxGeometry(graphicsDevice, type, numFrames, vertices, numVertices, indices, numIndices));
    }

    DxGeometry::DxGeometry(_In_ const std::shared_ptr<GraphicsDevice>& graphicsDevice, _In_ Type type, _In_ uint32_t numFrames, _In_ const void* vertices, _In_ uint32_t numVertices, _In_ const uint32_t* indices, _In_ uint32_t numIndices) :
        RuntimeGeometry(type),
        _device(graphicsDevice),
        _numVertices(numVertices),
        _numIndices(numIndices),
        _numIndicesPerFrame(numIndices / numFrames),
        _numFrames(numFrames),
        _stride((type == Type::Geometry3D) ? Geometry::Stride3D : Geometry::Stride2D)
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
        desc.StructureByteStride = _stride;
        desc.ByteWidth = desc.StructureByteStride * numVertices;

        initialData.pSysMem = vertices;
        initialData.SysMemPitch = desc.ByteWidth;
        initialData.SysMemSlicePitch = 0;

        CHECK_HR(dxGraphicsDevice->GetDevice()->CreateBuffer(&desc, &initialData, &_vertices));
    }

    void DxGeometry::Bind()
    {
        uint32_t strides[] = { _stride };
        static uint32_t offsets[] = { 0 };

        ID3D11Buffer* vertexBuffers[] = { _vertices.Get() };

        CHECK_FALSE(_device.expired());
        auto device = _device.lock();
        DxGraphicsDevice* dxGraphicsDevice = static_cast<DxGraphicsDevice*>(device.get());
        dxGraphicsDevice->GetContext()->IASetIndexBuffer(_indices.Get(), DXGI_FORMAT_R32_UINT, 0);
        dxGraphicsDevice->GetContext()->IASetVertexBuffers(0, _countof(vertexBuffers), vertexBuffers, strides, offsets);
    }

    void DxGeometry::Unbind()
    {
        static ID3D11Buffer* vertexBuffers[] = { nullptr };
        static uint32_t zeros[] = { 0 };

        CHECK_FALSE(_device.expired());
        auto device = _device.lock();
        DxGraphicsDevice* dxGraphicsDevice = static_cast<DxGraphicsDevice*>(device.get());
        dxGraphicsDevice->GetContext()->IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);
        dxGraphicsDevice->GetContext()->IASetVertexBuffers(0, _countof(vertexBuffers), vertexBuffers, zeros, zeros);
    }

    void DxGeometry::Update(_In_ const void* vertices, _In_ uint32_t numBytes)
    {
        CHECK_FALSE(_device.expired());
        auto device = _device.lock();
        DxGraphicsDevice* dxGraphicsDevice = static_cast<DxGraphicsDevice*>(device.get());

        dxGraphicsDevice->GetContext()->UpdateSubresource(_vertices.Get(), 0, nullptr, vertices, numBytes, 0);
    }

    void DxGeometry::Draw(_In_ uint32_t frame)
    {
        CHECK_RANGE(frame, 0, _numFrames - 1);

        uint32_t baseIndex = frame * _numIndicesPerFrame;

        CHECK_FALSE(_device.expired());
        auto device = _device.lock();
        DxGraphicsDevice* dxGraphicsDevice = static_cast<DxGraphicsDevice*>(device.get());
        dxGraphicsDevice->GetContext()->DrawIndexed(_numIndicesPerFrame, baseIndex, 0);
    }
}
