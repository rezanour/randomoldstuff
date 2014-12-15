#include "StdAfx.h"
#include "VertexBuffer.h"
#include "Renderer.h"

using namespace Lucid3D;
using namespace GDK;

VertexBuffer::VertexBuffer(_In_ BufferPtr spBuffer, _In_ size_t numVertices, _In_ size_t vertexStride)
    : _spBuffer(spBuffer), _numVertices(numVertices), _vertexStride(vertexStride)
{
}

HRESULT VertexBuffer::Create(_In_ Renderer* pRenderer, _In_ const void* pData, _In_ size_t numVertices, _In_ size_t vertexStride, _Out_ VertexBufferPtr& spVertexBuffer)
{
    HRESULT hr = S_OK;

    DevicePtr spDevice;
    BufferPtr spBuffer;

    D3D11_BUFFER_DESC bufferDesc = {0};
    D3D11_SUBRESOURCE_DATA initialData = {0};

    ISNOTNULL(pRenderer, E_INVALIDARG);
    ISNOTNULL(pData, E_INVALIDARG);

    spDevice = pRenderer->GetDevice();

    size_t pitch = vertexStride * numVertices;

    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.StructureByteStride = static_cast<uint>(vertexStride);
    bufferDesc.ByteWidth = static_cast<uint>(pitch);
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    initialData.pSysMem = pData;
    initialData.SysMemPitch = static_cast<uint>(pitch);
    initialData.SysMemSlicePitch = static_cast<uint>(pitch);

    CHECKHR(spDevice->CreateBuffer(&bufferDesc, &initialData, &spBuffer));
    {
        std::string name("VertexBuffer");
        spBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, name.size(), name.c_str());
    }

    spVertexBuffer.attach(new VertexBuffer(spBuffer, numVertices, vertexStride));

EXIT
    if (FAILED(hr))
    {
        spVertexBuffer.reset();
    }

    return hr;
}

