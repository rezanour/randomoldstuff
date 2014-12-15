#pragma once

#include <stde\non_copyable.h>
#include "GraphicsTypes.h"

namespace Lucid3D
{
    class Renderer;

    class IndexBuffer;
    typedef stde::ref_counted_ptr<IndexBuffer> IndexBufferPtr;

    class IndexBuffer : stde::non_copyable, public GDK::RefCountedBase<GDK::IRefCounted>
    {
    public:
        static HRESULT Create(_In_ Renderer* pRenderer, _In_ const void* pData, _In_ size_t numIndices, _In_ size_t indexStride, _In_ D3D11_PRIMITIVE_TOPOLOGY topology, _Out_ IndexBufferPtr& spIndexBuffer);

        BufferPtr GetBuffer() const { return _spBuffer; }
        size_t GetNumIndices() const { return _numIndices; }
        size_t GetIndexStride() const { return _indexStride; }
        D3D11_PRIMITIVE_TOPOLOGY GetTopology() const { return _topology; }

    private:
        IndexBuffer(_In_ BufferPtr spBuffer, _In_ size_t numIndices, _In_ size_t indexStride, _In_ D3D11_PRIMITIVE_TOPOLOGY topology);

        BufferPtr _spBuffer;
        size_t _numIndices;
        size_t _indexStride;
        D3D11_PRIMITIVE_TOPOLOGY _topology;
    };
}

