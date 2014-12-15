#include "Precomp.h"
#include "RuntimeGeometry.h"

using namespace GDK;
using namespace GDK::Content;
using Microsoft::WRL::ComPtr;

namespace GDK {
namespace Graphics {

ComPtr<RuntimeGeometry> GDKAPI RuntimeGeometry::Create(_In_ DxGraphicsDevice* device, _In_ GDK::Content::IGeometryResource* resource)
{
    CHECK_NOT_NULL(device, E_INVALIDARG);
    CHECK_NOT_NULL(resource, E_INVALIDARG);

    size_t numFrames = 0;
    CHECKHR(resource->GetFrameCount(reinterpret_cast<uint32_t*>(&numFrames)));
    size_t numVertices = 0;
    CHECKHR(resource->GetFrameVertices(0, reinterpret_cast<uint32_t*>(&numVertices), nullptr));

    std::unique_ptr<Vertex[]> buffer(new Vertex[numVertices * numFrames]);

    for (uint32_t frame = 0; frame < numFrames; ++frame)
    {
        const GEOMETRY_RESOURCE_VERTEX* vertices = nullptr;
        CHECKHR(resource->GetFrameVertices(frame, reinterpret_cast<uint32_t*>(&numVertices), &vertices));

        for (size_t i = 0, j = frame * numVertices; i < numVertices; ++i)
        {
            buffer[j].Position = *reinterpret_cast<const Vector3*>(&vertices[i].Position);
            buffer[j].Normal = *reinterpret_cast<const Vector3*>(&vertices[i].Normal);
            buffer[j].TexCoord = *reinterpret_cast<const Vector2*>(&vertices[i].TextureCoord0);
            ++j;
        }
    }

    size_t numIndices = 0;
    const uint32_t* indices = nullptr;
    CHECKHR(resource->GetFrameIndices(0, reinterpret_cast<uint32_t*>(&numIndices), &indices));
    return CreateFromData(device, numFrames, numVertices, buffer.get(), numIndices, indices);
}

ComPtr<RuntimeGeometry> GDKAPI RuntimeGeometry::CreateFromData(_In_ DxGraphicsDevice* device, _In_ size_t numFrames, _In_ size_t numVertices, _In_ const Vertex* vertices, _In_ size_t numIndices, _In_ const uint32_t* indices)
{
    CHECK_NOT_NULL(device, E_INVALIDARG);
    CHECK_NOT_NULL(vertices, E_INVALIDARG);

    ComPtr<RuntimeGeometry> geometry = Make<RuntimeGeometry>(L"");

    geometry->_numFrames = numFrames;
    geometry->_numVertices = static_cast<uint32_t>(numVertices);
    geometry->_numIndices = static_cast<uint32_t>(numIndices);

    D3D11_BUFFER_DESC desc = {0};
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    desc.StructureByteStride = sizeof(Vertex);
    desc.ByteWidth = desc.StructureByteStride * static_cast<UINT>(numFrames * numVertices);
    desc.Usage = D3D11_USAGE_DEFAULT;

    D3D11_SUBRESOURCE_DATA initData = {0};
    initData.pSysMem = vertices;
    initData.SysMemPitch = desc.ByteWidth;
    initData.SysMemSlicePitch = 0;

    CHECKHR(device->GetDevice()->CreateBuffer(&desc, &initData, &geometry->_vertices));

    desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    desc.StructureByteStride = sizeof(uint32_t);
    desc.ByteWidth = desc.StructureByteStride * static_cast<UINT>(numIndices);
    desc.Usage = D3D11_USAGE_DEFAULT;

    initData.pSysMem = indices;
    initData.SysMemPitch = desc.ByteWidth;
    initData.SysMemSlicePitch = 0;

    CHECKHR(device->GetDevice()->CreateBuffer(&desc, &initData, &geometry->_indices));

    return geometry.Detach();
}

RuntimeGeometry::RuntimeGeometry(_In_ const std::wstring& name) :
    _name(name),
    _isBound(false),
    _numFrames(1),
    _currentFrame(0)
{
}

void RuntimeGeometry::Bind(_In_ DxGraphicsDevice* device)
{
    _isBound = true;
    uint32_t stride = sizeof(Vertex);
    uint32_t offset = static_cast<uint32_t>(_currentFrame * _numVertices * sizeof(Vertex));
    device->GetContext()->IASetVertexBuffers(0, 1, _vertices.GetAddressOf(), &stride, &offset);
    device->GetContext()->IASetIndexBuffer(_indices.Get(), DXGI_FORMAT_R32_UINT, 0);
}

void RuntimeGeometry::Unbind(_In_ DxGraphicsDevice* device)
{
    _isBound = false;
    uint32_t offset = 0;
    ID3D11Buffer* vb = nullptr;
    device->GetContext()->IASetVertexBuffers(0, 1, &vb, &offset, &offset);
    device->GetContext()->IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);
}

void RuntimeGeometry::Draw(_In_ DxGraphicsDevice* device)
{
    device->GetContext()->DrawIndexed(_numIndices, 0, 0);
}

} // Graphics
} // GDK
