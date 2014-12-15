#pragma once

#include <Geometry.h>

namespace GDK
{
    class RuntimeGeometry : public Geometry
    {
    public:
        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        virtual void Update(_In_ const void* vertices, _In_ uint32_t numBytes) = 0;
        virtual void Draw(_In_ uint32_t frame) = 0;

    protected:
        RuntimeGeometry(_In_ Type type) :
            Geometry(type)
        {}
    };
}
