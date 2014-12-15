#include "stdafx.h"
#include "q2wal.h"

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

struct bmp_paletteentry_t
{
    uint8_t blue;
    uint8_t green;
    uint8_t red;
    uint8_t alpha;
};

#pragma pack(pop)

static std::map<std::string, std::unique_ptr<wal_fileheader_t>> g_images;

static bmp_paletteentry_t g_palette[256] = {};
static bool g_paletteLoaded = false;

static
void EnsurePalette()
{
    if (!g_paletteLoaded)
    {
        void* colormap = PakLoadFile("pics/colormap.pcx");
        uint32_t fileSize = PakFileSize("pics/colormap.pcx");

        uint8_t* pcxPalette = (uint8_t*)colormap + (fileSize - 769);
        assert(*pcxPalette == 12); // contains palette
        pcxPalette++;

        for (int i = 0; i < _countof(g_palette); ++i)
        {
            g_palette[i].red = pcxPalette[0];
            g_palette[i].green = pcxPalette[1];
            g_palette[i].blue = pcxPalette[2];
            g_palette[i].alpha = 1.0f;
            pcxPalette += 3;
        }

        g_paletteLoaded = true;
    }
}

void
WalLoadImage(
    _In_ PCSTR filename,
    _Out_opt_ uint32_t* width,
    _Out_opt_ uint32_t* height
    )
{
    auto it = g_images.find(filename);
    if (it == g_images.end())
    {
        // not loaded yet, load now
        std::string name("textures/");
        name.append(filename);
        name.append(".wal");
        void* buffer = PakLoadFile(name.c_str());
        std::unique_ptr<wal_fileheader_t> image((wal_fileheader_t*)buffer);

        if (width) *width = image->width;
        if (height) *height = image->height;

        g_images.insert(it, std::make_pair(filename, std::move(image)));
    }
    else
    {
        if (width) *width = it->second->width;
        if (height) *height = it->second->height;
    }
}

void
WalCopyPixels(
    _In_ PCSTR filename,
    _In_ uint8_t* buffer
    )
{
    // ensure the image is loaded
    WalLoadImage(filename, nullptr, nullptr);
    EnsurePalette();

    auto it = g_images.find(filename);
    if (it == g_images.end())
    {
        RIP(L"Error loading file\n");
    }

    uint8_t* indices = ((uint8_t*)it->second.get() + it->second->offset[0]);
    uint8_t* output = buffer;
    int size = it->second->width * it->second->height;
    for (int i = 0; i < size; ++i)
    {
        const bmp_paletteentry_t* pal = &g_palette[*indices];
        output[0] = pal->red;
        output[1] = pal->green;
        output[2] = pal->blue;
        output[3] = pal->alpha;
        output += 4;
        ++indices;
    }
}

void
WalClearAllImages()
{
    g_images.clear();
}
