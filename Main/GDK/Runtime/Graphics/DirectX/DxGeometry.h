#pragma once

#include "..\RuntimeGeometry.h"
#include <d3d11.h>
#include <wrl\client.h>

namespace GDK
{
    using Microsoft::WRL::ComPtr;

    class DxGeometry : public RuntimeGeometry
    {
    public:
        static std::shared_ptr<DxGeometry> Create(_In_ const std::shared_ptr<GraphicsDevice>& graphicsDevice, _In_ const std::shared_ptr<GeometryContent>& content);
        static std::shared_ptr<DxGeometry> Create(_In_ const std::shared_ptr<GraphicsDevice>& graphicsDevice, _In_ Type type, _In_ uint32_t numFrames, _In_ const void* vertices, _In_ uint32_t numVertices, _In_ const uint32_t* indices, _In_ uint32_t numIndices);

        virtual void Bind() override;
        virtual void Unbind() override;

        virtual void Update(_In_ const void* vertices, _In_ uint32_t numBytes) override;
        virtual void Draw(_In_ uint32_t frame) override;

    private:
        DxGeometry(_In_ const std::shared_ptr<GraphicsDevice>& graphicsDevice, _In_ Type type, _In_ uint32_t numFrames, _In_ const void* vertices, _In_ uint32_t numVertices, _In_ const uint32_t* indices, _In_ uint32_t numIndices);

        std::weak_ptr<GraphicsDevice> _device;
        ComPtr<ID3D11Buffer> _vertices;
        ComPtr<ID3D11Buffer> _indices;
        uint32_t _stride;
        uint32_t _numVertices;
        uint32_t _numIndices;
        uint32_t _numFrames;
        uint32_t _numIndicesPerFrame;
    };
}
