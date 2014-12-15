#include "StdAfx.h"
#include "IndexBuffer.h"
#include "Renderer.h"

using namespace Lucid3D;
using namespace GDK;

IndexBuffer::IndexBuffer(_In_ BufferPtr spBuffer, _In_ size_t numIndices, _In_ size_t indexStride, _In_ D3D11_PRIMITIVE_TOPOLOGY topology)
    : _spBuffer(spBuffer), _numIndices(numIndices), _indexStride(indexStride), _topology(topology)
{
}

HRESULT IndexBuffer::Create(_In_ Renderer* pRenderer, _In_ const void* pData, _In_ size_t numIndices, _In_ size_t indexStride, _In_ D3D11_PRIMITIVE_TOPOLOGY topology, _Out_ IndexBufferPtr& spIndexBuffer)
{
    HRESULT hr = S_OK;

    DevicePtr spDevice;
    BufferPtr spBuffer;

    D3D11_BUFFER_DESC bufferDesc = {0};
    D3D11_SUBRESOURCE_DATA initialData = {0};

    ISNOTNULL(pRenderer, E_INVALIDARG);
    ISNOTNULL(pData, E_INVALIDARG);

    spDevice = pRenderer->GetDevice();

    size_t pitch = indexStride * numIndices;

    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.StructureByteStride = static_cast<uint>(indexStride);
    bufferDesc.ByteWidth = static_cast<uint>(pitch);
    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    initialData.pSysMem = pData;
    initialData.SysMemPitch = static_cast<uint>(pitch);
    initialData.SysMemSlicePitch = static_cast<uint>(pitch);

    CHECKHR(spDevice->CreateBuffer(&bufferDesc, &initialData, &spBuffer));
    {
        std::string name("IndexBuffer");
        spBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, name.size(), name.c_str());
    }

    spIndexBuffer.attach(new IndexBuffer(spBuffer, numIndices, indexStride, topology));

EXIT
    if (FAILED(hr))
    {
        spIndexBuffer.reset();
    }

    return hr;
}

