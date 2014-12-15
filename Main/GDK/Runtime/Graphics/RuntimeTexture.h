#pragma once

#include <Texture.h>

namespace GDK
{
    class RuntimeTexture : public Texture
    {
    public:
        virtual void Bind(_In_ uint32_t slot) = 0;
        virtual void Unbind(_In_ uint32_t slot) = 0;

    protected:
        RuntimeTexture(_In_ uint32_t width, _In_ uint32_t height) :
            Texture(width, height)
        {}
    };
}
