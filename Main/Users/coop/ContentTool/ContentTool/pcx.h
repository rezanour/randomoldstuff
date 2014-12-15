#pragma once

#define PCX_COLORMAP_ENTRIES 48
#define PCX_PADDING_ENTRIES  58

#pragma pack(push,1)

struct pcx_fileheader_t
{
    int8_t  id;
    int8_t  version;
    int8_t  encoding;
    int8_t  bpp;
    int16_t x1;
    int16_t y1;
    int16_t x2;
    int16_t y2;
    int16_t hres;
    int16_t yres;
    int8_t  colorMap[PCX_COLORMAP_ENTRIES];
    int8_t  reserved;
    int8_t  planes;
    int16_t bpl;
    int16_t pal;
    int8_t  padding[PCX_PADDING_ENTRIES];
};

#pragma pack(pop)

bool ConvertPCXTo8bitBMP(_In_ std::vector<uint8_t>& pcx, _Inout_ std::vector<uint8_t>& bmp);
bool ConvertPCXTo32bitBMP(_In_ std::vector<uint8_t>& pcx, _Inout_ std::vector<uint8_t>& bmp);
bool GetPaletteFromPCX(_In_ std::vector<uint8_t>& pcx, _Inout_ std::vector<bmp_paletteentry_t>& pal);
void GetCommonPalette(_In_ std::ifstream& inputfile, _In_ std::vector<pak_entry_t>& entries, _Inout_ std::vector<bmp_paletteentry_t>& pal);