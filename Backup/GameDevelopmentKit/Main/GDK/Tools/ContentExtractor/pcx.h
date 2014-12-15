#pragma once

#define PCX_COLORMAP_ENTRIES 48
#define PCX_PADDING_ENTRIES  58

#pragma pack(push,1)

struct PCXFILEHEADER
{
    char id;
    char version;
    char encoding;
    char bpp;
    short x1;
    short y1;
    short x2;
    short y2;
    short hres;
    short yres;
    char colorMap[PCX_COLORMAP_ENTRIES];
    char reserved;
    char planes;
    short bpl;
    short pal;
    char padding[PCX_PADDING_ENTRIES];
};

#pragma pack(pop)

HRESULT SavePCXAsTextureContent(const wchar_t* path, const char* name, byte_t* data, uint32_t size);
HRESULT ConvertPCXToBMP(PCXFILEHEADER* header, byte_t* data, uint32_t size, std::vector<byte_t>& bmpData, std::vector<RGBQUAD>& bmpal, BITMAPINFOHEADER& bmheader, bool flipImage = false);
