#pragma once
#include <stdint.h>
#include <GDK.h>

#pragma pack(push,1)

const uint16_t BitmapFileType = 0x4D42;

struct bmp_fileheader_t 
{
    uint16_t type;
    uint32_t size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offBits;
};

struct bmp_paletteentry_t
{
    uint8_t blue;
    uint8_t green;
    uint8_t red;
    uint8_t reserved;
};

struct bmp_infoheader_t
{
    uint32_t size;
    int32_t  width;
    int32_t  height;
    uint16_t planes;
    uint16_t bitCount;
    uint32_t compression;
    uint32_t sizeImage;
    int32_t  xPelsPerMeter;
    int32_t  yPelsPerMeter;
    uint32_t clrUsed;
    uint32_t clrImportant;
};

#pragma pack(pop)

void Convert32bitBMPToTextureContent(_In_ std::vector<uint8_t>& bmp, _Inout_ std::shared_ptr<GDK::TextureContent>& content);