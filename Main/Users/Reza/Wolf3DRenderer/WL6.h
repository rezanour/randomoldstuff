#pragma once

#pragma pack(push, 1)

struct VSwapHeader
{
    uint16_t NumChunks;
    uint16_t SpritesOffset;
    uint16_t SoundsOffset;
};

struct SpriteHeader
{
    uint16_t Left;
    uint16_t Right;
    uint16_t Offsets[64];
};

#pragma pack(pop)

uint32_t GetWolf3DColor(uint8_t index);
