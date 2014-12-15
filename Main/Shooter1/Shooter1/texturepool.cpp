#include "precomp.h"
#include "texturepool.h"

uint32_t TexturePool::s_nextId = 0;

_Use_decl_annotations_
TexturePool::TexturePool(ID3D11Device* device, uint32_t width, uint32_t height, DXGI_FORMAT format, bool supportMips, uint32_t maxTextures) :
    _id(s_nextId++), _width(width), _height(height), _format(format), _maxElements(maxTextures), _numElements(0), _supportMips(supportMips)
{
    assert(device != nullptr);

    D3D11_TEXTURE2D_DESC td = {};
    td.ArraySize = maxTextures;
    td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    td.Format = format;
    td.Height = height;
    td.MipLevels = 1;
    td.SampleDesc.Count = 1;
    td.Usage = D3D11_USAGE_DEFAULT;
    td.Width = width;

    if (_supportMips)
    {
        td.MipLevels = 0;
    }

    ComPtr<ID3D11Texture2D> texture;
    CHECKHR(device->CreateTexture2D(&td, nullptr, &texture));
    CHECKHR(device->CreateShaderResourceView(texture.Get(), nullptr, &_srv));
}

_Use_decl_annotations_
bool TexturePool::HasAvailableSpace(uint32_t width, uint32_t height, DXGI_FORMAT format, bool supportsMips, uint32_t numTextures) const
{
    return (width == _width &&
            height == _height &&
            format == _format &&
            supportsMips == _supportMips &&
            (_numElements + numTextures <= _maxElements));
}

_Use_decl_annotations_
uint32_t TexturePool::ReserveRange(uint32_t numTextures)
{
    uint32_t index = static_cast<uint32_t>(-1);
    if (_numElements + numTextures <= _maxElements)
    {
        index = _numElements;
        _numElements += numTextures;
    }
    return index;
}
