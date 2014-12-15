#pragma once
#include <vector>
#include <string>
#include "bmp.h"

#pragma pack(push,1)

struct wal_fileheader_t
{
    int8_t   name[32];
    uint32_t width;         // width (in pixels) of the largest mipmap level
    uint32_t height;        // height (in pixels) of the largest mipmap level
    int16_t  offset[4];     // byte offset of the start of each of the 4 mipmap levels
    int8_t   next_name[32]; // name of the next texture in the animation
    uint32_t flags;
    uint32_t contents;
    uint32_t value;
};

#pragma pack(pop)

void ConvertWALTo32bitBMP(_In_ std::vector<uint8_t>& wal, _In_ std::vector<bmp_paletteentry_t>& pal, _Inout_ std::vector<uint8_t>& bmp);
