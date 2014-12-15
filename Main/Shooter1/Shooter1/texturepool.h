#pragma once

class Graphics;

class TexturePool : public TrackedObject<MemoryTag::TexturePool>
{
public:
    uint32_t GetId() const { return _id; }
    uint32_t GetWidth() const { return _width; }
    uint32_t GetHeight() const { return _height; }
    DXGI_FORMAT GetFormat() const { return _format; }

    ID3D11ShaderResourceView* Get() const { return _srv.Get(); }
    ID3D11ShaderResourceView** GetAddress() { return _srv.GetAddressOf(); }

    bool HasAvailableSpace(_In_ uint32_t width, _In_ uint32_t height, _In_ DXGI_FORMAT format, _In_ bool supportMips, _In_ uint32_t numTextures) const;
    uint32_t ReserveRange(_In_ uint32_t numTextures);

private:
    friend class Graphics;
    TexturePool(_In_ ID3D11Device* device, _In_ uint32_t width, _In_ uint32_t height, _In_ DXGI_FORMAT format, _In_ bool supportMips, _In_ uint32_t maxTextures);

private:
    ComPtr<ID3D11ShaderResourceView> _srv;

    static uint32_t s_nextId;
    uint32_t _id;
    uint32_t _width : 16;
    uint32_t _height : 15;
    bool _supportMips : 1;
    DXGI_FORMAT _format;
    uint32_t _maxElements;
    uint32_t _numElements;
};
