#pragma once

#include "..\RuntimeTexture.h"
#include "GDKGL.h"

namespace GDK
{
    class GlTexture : public RuntimeTexture
    {
    public:
        static std::shared_ptr<GlTexture> Create(_In_ const std::shared_ptr<TextureContent>& content);
        static std::shared_ptr<GlTexture> Create(_In_ uint32_t width, _In_ uint32_t height, _In_ TextureFormat format, _In_ const byte_t* pixels);

        ~GlTexture();

        virtual void Bind(_In_ uint32_t slot) override;
        virtual void Unbind(_In_ uint32_t slot) override;

    private:
        GlTexture(_In_ uint32_t width, _In_ uint32_t height, _In_ TextureFormat format, _In_ const byte_t* pixels);

        GLuint _handle;
    };
}