#include "precomp.h"
#include "wal.h"

#pragma pack(push,1)

struct WAL_HEADER
{
    char         name[32];
    unsigned int width;         // width (in pixels) of the largest mipmap level
    unsigned int height;        // height (in pixels) of the largest mipmap level
    int          offset[4];     // byte offset of the start of each of the 4 mipmap levels
    char         next_name[32]; // name of the next texture in the animation
    unsigned int flags;
    unsigned int contents;
    unsigned int value;
};

#define PCX_COLORMAP_ENTRIES 48
#define PCX_PADDING_ENTRIES  58

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

WalParser::WalParser(std::unique_ptr<uint8_t[]>& palettePcx, uint32_t size)
{
    memcpy_s(_palette, sizeof(RGB) * _countof(_palette), (palettePcx.get() + size - 768), sizeof(RGB) * _countof(_palette));
}

void WalParser::LoadWalIntoTextureIndex(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11Texture2D>& texture, uint32_t index, const std::unique_ptr<uint8_t[]>& chunk, _Out_ uint32_t* width, _Out_ uint32_t* height) const
{
    const WAL_HEADER* header = reinterpret_cast<const WAL_HEADER*>(chunk.get());
    std::unique_ptr<uint32_t[]> pixels(new uint32_t[header->width * header->height]);

    *width = header->width;
    *height = header->height;

    const uint8_t* indices = (chunk.get() + header->offset[0]);
    for (uint32_t i = 0; i < header->width * header->height; ++i, ++indices)
    {
        RGB rgb = _palette[*indices];
        pixels[i] = 0xFF000000 | ((uint32_t)rgb.B << 16) | ((uint32_t)rgb.G << 8) | (uint32_t)rgb.R;
    }

    ComPtr<ID3D11DeviceContext> context;
    device->GetImmediateContext(&context);

    D3D11_BOX box = {};
    box.right = header->width;
    box.bottom = header->height;
    box.back = 1;
    context->UpdateSubresource(texture.Get(), D3D11CalcSubresource(0, index, 1), &box, pixels.get(), header->width * sizeof(uint32_t), header->width * header->height * sizeof(uint32_t));
}
