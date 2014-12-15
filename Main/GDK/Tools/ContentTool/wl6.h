#pragma once
#include <stdint.h>
#include <string>
#include <vector>
#include <GDK.h>

#define mapheadFilePath  "maphead.wl6"
#define gamemapsFilePath "gamemaps.wl6"
#define vgaheadFilePath  "vgahead.wl6"
#define vgadictFilePath  "vgadict.wl6"
#define vgagraphFilePath "vgagraph.wl6"
#define vswapFilePath    "vswap.wl6"

#pragma pack(push,1)

struct w3d_level_t
{
    uint32_t layer1Offset;
    uint32_t layer2Offset;
    uint32_t layer3Offset;
    uint16_t layer1SizeBytes;
    uint16_t layer2SizeBytes;
    uint16_t layer3SizeBytes;
    uint16_t layerWidth;
    uint16_t layerHeight;
    int8_t   name[16];
    int8_t   signature[4];
};

struct w3d_spriteData_t
{
    uint16_t leftpix;
    uint16_t rightpix;
    uint16_t offsets[64];
};

struct w3d_VSwapHeader_t
{
    uint16_t numChunks;
    uint16_t spritesOffset;
    uint16_t soundsOffset;
};

struct w3d_VSwapChunk_t
{
    uint32_t offset; // offset of chunk in file
    uint16_t length; // chunk data byte size
};

struct w3d_layer_t
{
    uint32_t  offset;
    uint16_t  sizeBytes;
    uint16_t  width;
    uint16_t  height;
    uint16_t* data; // memory was allocated using malloc()
};

struct w3d_levelData_t
{
    uint16_t* layer1; // walls (memory was allocated using malloc())
    uint16_t* layer2; // objects (memory was allocated using malloc())
    uint16_t* layer3; // other (memory was allocated using malloc())
};

struct w3d_picture_t
{
    uint16_t width;
    uint16_t height;
};

struct w3d_huffnode_t
{
    uint16_t bit0;
    uint16_t bit1;
};

struct w3d_huffnode32_t
{
    uint32_t bit0;
    uint32_t bit1;
};

struct w3d_paletteentry_t
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

enum w3d_VSwapChunkType_t
{
    allChunks,
    spriteChunksOnly,
    soundChunksOnly,
    wallsChunksOnly
};

struct w3d_VSwapChunkInfo_t
{
    w3d_VSwapChunk_t chunk;
    size_t id;
};

struct w3d_mapfileHeader_t
{
    uint16_t fileMarker;
};

enum w3d_layerType_t
{
    wallsLayer,
    itemsLayer,
    miscLayer
};

#pragma pack(pop)

void ReadVSwapChunkList(_In_ std::vector<uint8_t>& vswap, _In_ const w3d_VSwapChunkType_t& chunkType, _Inout_ std::vector<w3d_VSwapChunkInfo_t>& chunks);
void ReadVSwapChunkToBuffer(_In_ std::vector<uint8_t>& vswap, _In_ const w3d_VSwapChunk_t& chunk, _Inout_ std::vector<uint8_t>& buffer);
bool ConvertWL6WallTo32bitBMP(_In_ std::vector<uint8_t>& wall, _Inout_ std::vector<uint8_t>& bmp);
bool ConvertWL6SpriteTo32bitBMP(_In_ std::vector<uint8_t>& sprite, _Inout_ std::vector<uint8_t>& bmp);
void ReadW3DLevelList(_In_ std::vector<uint8_t>& maphead, _In_ std::vector<uint8_t>& maps, _In_ std::vector<w3d_level_t>& levels);
void ReadW3dLevelLayerToBuffer(_In_ std::vector<uint8_t>& maps, _In_ const w3d_level_t level, _In_ const w3d_layerType_t layerType, _In_ std::vector<uint8_t>& buffer);
void ConvertWallsLayerToGeometryContentList(_In_ std::vector<uint8_t>& layer, _Inout_ std::vector<std::wstring>& textureNameList, _Inout_ std::vector<std::shared_ptr<GDK::GeometryContent>>& contentList, _In_ const float scaleFactor);
std::string GetW3DLevelName(_In_ const w3d_level_t& level);
void Create1x1Texture(_In_ const uint8_t red, _In_ const uint8_t green, _In_ const uint8_t blue, _In_ const uint8_t alpha, _Inout_ std::shared_ptr<GDK::TextureContent>& content);
void CreateDoorModel(_In_ float scaleFactor, _Inout_ std::shared_ptr<GDK::GeometryContent>& content);