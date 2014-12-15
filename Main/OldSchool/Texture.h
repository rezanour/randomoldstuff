#pragma once

struct __declspec(novtable) ITexture
{
    virtual DXGI_FORMAT GetFormat() const = 0;
    virtual uint32_t GetWidth() const = 0;
    virtual uint32_t GetHeight() const = 0;

    virtual void UpdateContents(_In_ const void* pixels, _In_ uint32_t pitch) = 0;
};
