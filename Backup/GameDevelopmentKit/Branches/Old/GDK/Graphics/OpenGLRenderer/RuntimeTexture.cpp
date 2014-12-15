#include "Precomp.h"
#include "RuntimeTexture.h"

using namespace GDK;
using namespace GDK::Content;
using Microsoft::WRL::ComPtr;

namespace GDK {
namespace Graphics {

ComPtr<RuntimeTexture> GDKAPI RuntimeTexture::Create(_In_ GDK::Content::ITextureResource* resource)
{
    CHECK_NOT_NULL(resource, E_INVALIDARG);

    size_t width, height;
    CHECKHR(resource->GetSize(0, reinterpret_cast<uint32_t*>(&width), reinterpret_cast<uint32_t*>(&height)));

    const byte_t* pixels = nullptr;
    CHECKHR(resource->GetPixels(0, &pixels));

    return CreateFromData(width, height, pixels);
}

ComPtr<RuntimeTexture> GDKAPI RuntimeTexture::CreateFromData(_In_ size_t width, _In_ size_t height, _In_ const byte_t* pixels)
{
    CHECK_NOT_NULL(pixels, E_INVALIDARG);

    ComPtr<RuntimeTexture> texture = Make<RuntimeTexture>(L"");

    glGenTextures(1, &texture->_texture);
    glBindTexture(GL_TEXTURE_2D, texture->_texture);

    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    CHECK_GL(glGetError());

    return texture.Detach();
}

RuntimeTexture::RuntimeTexture(_In_ const std::wstring& name) :
    _name(name),
    _isBound(false),
    _texture(0)
{
}

RuntimeTexture::~RuntimeTexture()
{
    if (_texture != 0)
    {
        glDeleteTextures(1, &_texture);
    }
}

void RuntimeTexture::Bind()
{
    _isBound = true;
    glBindTexture(GL_TEXTURE_2D, _texture);
}

void RuntimeTexture::Unbind()
{
    _isBound = false;
}

} // Graphics
} // GDK
