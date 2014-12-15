#pragma once

typedef struct
{
    uint32_t Width;
    uint32_t Height;
    uint32_t Pitch;
    DXGI_FORMAT Format;
    uint8_t* Data;
    uint32_t DataSize;
    uint8_t MipCount;
}ImageFileData;

void LoadImageFileMetadataOnly(_In_z_ const char* path, _Out_ ImageFileData* fileData);
void LoadImageFileData(_In_z_ const char* path, _Out_ ImageFileData* fileData);
void FreeImageFileData(_In_ ImageFileData* fileData);

typedef struct
{
    uint32_t Character;
    RECT Subrect;
    float XOffset;
    float YOffset;
    float XAdvance;
} Glyph;

typedef struct
{
    ImageFileData ImageData;
    float LineSpacing;
    uint32_t DefaultCharacter;
    uint32_t GlyphCount;
    Glyph* Glyphs;

}SpriteFontFileData;

void LoadSpriteFontFileData(_In_z_ const char* path, _Out_ SpriteFontFileData* fileData);
void FreeSpriteFontFileData(_In_ SpriteFontFileData* fileData);

typedef struct
{
    const WAVEFORMATEX* Format;    // points to a location within Data
    uint8_t* Data;
    uint32_t DataSize;
    const uint8_t* StartAudioData; // points to a location within Data
    uint32_t StartAudioDataSize;
}SoundFileData;

void LoadSoundFileData(_In_z_ const char* path, _Out_ SoundFileData* fileData);
void FreeSoundFileData(_In_ SoundFileData* fileData);

typedef struct
{
    int Format; // TBD format type
    uint8_t* Vertices;
    uint32_t NumVertices;
    uint32_t* Indices;
    uint32_t NumIndices;
    uint32_t Stride;
    char** MaterialTextures;
    uint32_t NumMaterials;
}GeometryFileData;

void LoadGeometryFileData(_In_z_ const char* path, _Out_ GeometryFileData* fileData);
void FreeGeometryFileData(_In_ GeometryFileData* fileData);