#pragma once

#include <stde\non_copyable.h>
#include "GraphicsTypes.h"

namespace Lucid3D
{
    class Renderer;

    class VertexBuffer;
    typedef stde::ref_counted_ptr<VertexBuffer> VertexBufferPtr;

    class VertexBuffer : stde::non_copyable, public GDK::RefCountedBase<GDK::IRefCounted>
    {
    public:
        static HRESULT Create(_In_ Renderer* pRenderer, _In_ const void* pData, _In_ size_t numVertices, _In_ size_t vertexStride, _Out_ VertexBufferPtr& spVertexBuffer);

        BufferPtr GetBuffer() const { return _spBuffer; }
        size_t GetNumVertices() const { return _numVertices; }
        size_t GetVertexStride() const { return _vertexStride; }

    private:
        VertexBuffer(_In_ BufferPtr spBuffer, _In_ size_t numVertices, _In_ size_t vertexStride);

        BufferPtr _spBuffer;
        size_t _numVertices;
        size_t _vertexStride;
    };
}

