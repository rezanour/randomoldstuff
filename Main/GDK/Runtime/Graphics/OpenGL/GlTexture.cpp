#include "GlTexture.h"
#include <GDKError.h>

using namespace GDK;

_Use_decl_annotations_
std::shared_ptr<GlTexture> GlTexture::Create(const std::shared_ptr<TextureContent>& content)
{
    return std::shared_ptr<GlTexture>(GDKNEW GlTexture(content->GetWidth(), content->GetHeight(), content->GetFormat(), content->GetPixels()));
}

_Use_decl_annotations_
std::shared_ptr<GlTexture> GlTexture::Create(uint32_t width, uint32_t height, TextureFormat format, const byte_t* pixels)
{
    return std::shared_ptr<GlTexture>(GDKNEW GlTexture(width, height, format, pixels));
}

_Use_decl_annotations_
GlTexture::GlTexture(uint32_t width, uint32_t height, TextureFormat format, const byte_t* pixels) : 
    RuntimeTexture(width, height), _handle(0)
{
    // Create the texture object
    CHECK_GL(glGenTextures(1, &_handle));
    CHECK_TRUE(_handle != 0);

    // Bind it so we can set the data
    Bind(0);

    // Set the data on the texture
    // TODO: Support the format properly.
    UNREFERENCED_PARAMETER(format);
    CHECK_GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, static_cast<const void *>(pixels)));
    CHECK_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    CHECK_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    CHECK_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    CHECK_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));


    // And unbind the texture
    Unbind(0);
}

GlTexture::~GlTexture()
{
    if (_handle)
    {
        CHECK_GL(glDeleteTextures(1, &_handle));
        _handle = 0;
    }
}

_Use_decl_annotations_
void GlTexture::Bind(uint32_t slot)
{
    CHECK_GL(glActiveTexture(GL_TEXTURE0 + slot));
    CHECK_GL(glBindTexture(GL_TEXTURE_2D, _handle));
}

_Use_decl_annotations_
void GlTexture::Unbind(uint32_t slot)
{
    CHECK_GL(glActiveTexture(GL_TEXTURE0 + slot));
    CHECK_GL(glBindTexture(GL_TEXTURE_2D, 0));
}