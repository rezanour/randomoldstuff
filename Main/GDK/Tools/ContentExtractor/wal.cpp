#include "stdafx.h"

HRESULT SaveWALAsTextureContent(const wchar_t* path, const char* name, byte_t* data, uint32_t size, std::vector<RGBQUAD> bmpal)
{
    if (!path || !data || !size)
    {
        return E_INVALIDARG;
    }

    HRESULT hr = S_OK;
    
    WAL_HEADER* walHeader = (WAL_HEADER*)data;
    std::shared_ptr<GDK::TextureContent> textureContent;
    size_t numBytes = 0;
    std::unique_ptr<byte_t[]> pixels;
    std::unique_ptr<GDK::RectangleF[]> frames(new GDK::RectangleF[1]);
    std::ofstream resourceFile(path,std::ios_base::binary);

    std::vector<byte_t> bmpData;
    BITMAPINFOHEADER bmih = {0};

    ULONG bytesWritten = 0;

    bmpData.resize(walHeader->width * walHeader->height);
    memcpy_s(bmpData.data(), bmpData.size(), data + walHeader->offset[0], bmpData.size());
    CHECKHR(Convert8bitBMPto32bit(bmpData, bmpal, bmih), L"Failed to convert 8-bit data to 32-bit BMP");

    numBytes = walHeader->width * walHeader->height * GDK::TextureContent::BytesPerPixel(GDK::TextureFormat::R8G8B8A8);
    pixels.reset(new byte_t[numBytes]);
    memcpy_s(pixels.get(), numBytes, bmpData.data(), numBytes);
    frames.get()[0] = GDK::RectangleF(0.0f, 0.0f, 1.0f, 1.0f);

    textureContent = GDK::TextureContent::Create(walHeader->width, walHeader->height, GDK::TextureFormat::R8G8B8A8, pixels, 1, frames);
    textureContent->Save(resourceFile);

Exit:

    return hr;
}
