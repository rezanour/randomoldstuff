#include "stdafx.h"

void Convert32bitBMPToTextureContent(_In_ std::vector<uint8_t>& bmp, _Inout_ std::shared_ptr<GDK::TextureContent>& content)
{
    bmp_fileheader_t* bmfh = (bmp_fileheader_t*)bmp.data();
    bmp_infoheader_t* bmih = (bmp_infoheader_t*)(bmp.data() + sizeof(bmp_fileheader_t));
    uint8_t* bmpData = (bmp.data() + bmfh->offBits);

    std::unique_ptr<uint8_t[]> pixels;
    std::unique_ptr<GDK::RectangleF[]> frames(new GDK::RectangleF[1]);

    pixels.reset(new uint8_t[bmih->sizeImage]);
    memcpy_s(pixels.get(), bmih->sizeImage, bmpData, bmih->sizeImage);

    // For some reason the 32-bit BMP data appears to be BGRA instead of RGBA.
    // Because of this, I must perform a pixel swap before creating a texture.
    for (uint32_t i = 0; i < bmih->sizeImage; i += 4)
    {
        uint8_t pixel = pixels.get()[i];
        pixels.get()[i] = pixels.get()[i + 2];
        pixels.get()[i + 2] = pixel;
    }

    frames.get()[0] = GDK::RectangleF(0.0f, 0.0f, 1.0f, 1.0f);

    content = GDK::TextureContent::Create(bmih->width, bmih->height, GDK::TextureFormat::R8G8B8A8, pixels, 1, frames);
}