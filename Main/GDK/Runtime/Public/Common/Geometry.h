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
        struct Vertex2D
        {
            Vertex2D() {}
            Vertex2D(_In_ const Vector2& pos, _In_ const Vector2& tex) : position(pos), texCoord(tex) {}
            Vertex2D(_In_ const Float2& pos, _In_ const Float2& tex) : position(pos.x, pos.y), texCoord(tex.x, tex.y) {}

            Vector2 position;
            Vector2 texCoord;
        };

        struct Vertex3D
        {
            Vertex3D() {}
            Vertex3D(_In_ const Vector3& pos, _In_ const Vector3& norm, _In_ const Vector2& tex) : position(pos), normal(norm), texCoord(tex) {}
            Vertex3D(_In_ const Float3& pos, _In_ const Float3& norm, _In_ const Float2& tex) : position(pos.x, pos.y, pos.z), normal(norm.x, norm.y, norm.z), texCoord(tex.x, tex.y) {}

            Vector3 position;
            Vector3 normal;
            Vector2 texCoord;
        };

        static const uint32_t Stride2D = sizeof(Vertex2D);
        static const uint32_t Stride3D = sizeof(Vertex3D);

        enum class Type
        {
            Geometry2D,
            Geometry3D
        };

        Type GetType() const { return _type; }

    protected:
        Geometry(_In_ Type type) : _type(type) {}

    private:
        Type _type;
    };
}
