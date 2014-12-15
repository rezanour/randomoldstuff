#include "precomp.h"

void LoadSpriteFontFileData(_In_z_ const char* path, _Out_ SpriteFontFileData* fileData)
{
    std::vector<uint8_t> spriteFontData;
    ReadFileToBuffer(path, spriteFontData);

    uint8_t* data = (uint8_t*)spriteFontData.data();

    // skip magic number
    data += (sizeof(uint8_t)* 8);

    // Read glyph count
    fileData->GlyphCount = *((uint32_t*)data);
    data += sizeof(uint32_t);

    fileData->Glyphs = new Glyph[fileData->GlyphCount];

    // Read glyphs
    for (uint32_t i = 0; i < fileData->GlyphCount; i++)
    {
        fileData->Glyphs[i] = (*(Glyph*)data);
        data += sizeof(Glyph);
    }

    // Read line spacing
    fileData->LineSpacing = *((float*)data);
    data += sizeof(float);

    // Read default character
    fileData->DefaultCharacter = *(uint32_t*)(data);
    data += sizeof(uint32_t);

    // Read the texture data.
    fileData->ImageData.Width = *((uint32_t*)data); data += sizeof(uint32_t);
    fileData->ImageData.Height = *((uint32_t*)data); data += sizeof(uint32_t);
    fileData->ImageData.Format = *((DXGI_FORMAT*)data); data += sizeof(DXGI_FORMAT);
    uint32_t textureStride = *((uint32_t*)data); data += sizeof(uint32_t);
    uint32_t textureRows = *((uint32_t*)data); data += sizeof(uint32_t);

    // Compute pitch
    size_t rowPitch = 0;
    size_t slicePitch = 0;
    ComputePitch(fileData->ImageData.Format, fileData->ImageData.Width, fileData->ImageData.Height, rowPitch, slicePitch);

    fileData->ImageData.Pitch = static_cast<uint32_t>(rowPitch);

    // Allocate texture memory
    fileData->ImageData.DataSize = textureStride * textureRows;
    fileData->ImageData.Data = reinterpret_cast<uint8_t*>(_aligned_malloc(fileData->ImageData.DataSize, 16));
    CHECKNOTNULL(fileData->ImageData.Data);

    memcpy(fileData->ImageData.Data, data, fileData->ImageData.DataSize);
}

void FreeSpriteFontFileData(_In_ SpriteFontFileData* fileData)
{
    if (fileData)
    {
        FreeImageFileData(&fileData->ImageData);

        if (fileData->Glyphs)
        {
            delete[] fileData->Glyphs;
        }

        memset(fileData, 0, sizeof(*fileData));
    }
}