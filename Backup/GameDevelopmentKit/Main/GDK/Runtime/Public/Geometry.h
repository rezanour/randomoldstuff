#pragma once

#include "Platform.h"
#include "RuntimeObject.h"
#include "GraphicsDevice.h"

namespace GDK
{
    class GraphicsDevice;

    class Geometry : public RuntimeObject<Geometry>
    {
    public:
        struct Vertex
        {
            Vector3 position;
            Vector3 normal;
            Vector2 texCoord;
        };
        static const uint32_t Stride = sizeof(Vertex);

        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        // At runtime, we assume that the caller knows exactly what animation they're playing, and what geometry object
        // they're talking to, so the number of frames & animation names are known. This keeps this interface minimal.
        virtual void Draw(_In_ uint32_t frame) = 0;

    protected:
        Geometry() {}
    };
}
