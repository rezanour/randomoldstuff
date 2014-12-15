#pragma once

#include "Platform.h"
#include "RuntimeObject.h"
#include "GraphicsDevice.h"

namespace GDK
{
    class GraphicsDevice;

    class Texture : public RuntimeObject<Texture>
    {
    public:
        virtual void Bind(_In_ uint32_t slot) = 0;
        virtual void Unbind(_In_ uint32_t slot) = 0;

    protected:
        Texture() { }
    };
}
