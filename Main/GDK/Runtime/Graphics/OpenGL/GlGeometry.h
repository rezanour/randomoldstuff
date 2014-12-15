#pragma once

#include "../RuntimeGeometry.h"
#include "GDKGL.h"

namespace GDK
{
    class GlGeometry : public RuntimeGeometry
    {
    public:
        static std::shared_ptr<GlGeometry> Create(_In_ const std::shared_ptr<GeometryContent>& content);
        static std::shared_ptr<GlGeometry> Create(_In_ uint32_t numFrames, _In_ Type type, _In_ const void* vertices, _In_ uint32_t numVertices, _In_ const uint32_t* indices, _In_ uint32_t numIndices);

        ~GlGeometry();

        virtual void Bind() override;
        virtual void Unbind() override;

        virtual void Update(_In_ const void* vertices, _In_ uint32_t numBytes) override;
        virtual void Draw(_In_ uint32_t frame) override;

    private:
        GlGeometry(_In_ uint32_t numFrames, _In_ Type type, _In_ const void* vertices, _In_ uint32_t numVertices, _In_ const uint32_t* indices, _In_ uint32_t numIndices);

        GLuint _vaoHandle;
        GLuint _vboHandle;
        GLuint _iboHandle;
        uint32_t _numVertices;
        uint32_t _numIndices;
        uint32_t _numFrames;
        uint32_t _numVerticesPerFrame;
        uint32_t _numIndicesPerFrame;
    };
}