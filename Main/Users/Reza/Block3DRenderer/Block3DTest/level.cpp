#include "precomp.h"
#include "level.h"

//
// WL6 Data
//
#pragma pack(push,1)

struct LevelData
{
    uint32_t Layer1Offset;
    uint32_t Layer2Offset;
    uint32_t Layer3Offset;
    uint16_t Layer1SizeBytes;
    uint16_t Layer2SizeBytes;
    uint16_t Layer3SizeBytes;
    uint16_t LayerWidth;
    uint16_t LayerHeight;
    char Name[16];
    char Signature[4];
};

struct MapFileHeader
{
    static const uint16_t MagicNumber = 0xABCD;
    uint16_t Marker;
};

#pragma pack(pop)

static void LoadFile(_In_z_ const wchar_t* filename, _Inout_ std::unique_ptr<uint8_t[]>* buffer, _Out_ size_t* size);
static void UncompressCarmack(_In_ const uint16_t* compressed, _Out_writes_(decompressedSize / 2) uint16_t* decompressed, uint16_t decompressedSize);
static void ExpandRLEW(_In_ const uint16_t* compressed, _Out_writes_(expandedSize / 2) uint16_t* expanded, uint16_t expandedSize);

_Use_decl_annotations_
Level::Level(const wchar_t* contentRoot)
{
    size_t size;
    std::unique_ptr<uint8_t[]> buffer;
    wchar_t filename[1024];
    swprintf_s(filename, L"%s\\maphead.wl6", contentRoot);
    LoadFile(filename, &buffer, &size);

    MapFileHeader* header = (MapFileHeader*)buffer.get();
    if (header->Marker != MapFileHeader::MagicNumber)
    {
        throw std::exception();
    }

    uint32_t* offsets = (uint32_t*)(buffer.get() + sizeof(MapFileHeader));

    std::unique_ptr<uint8_t[]> mapData;
    swprintf_s(filename, L"%s\\gamemaps.wl6", contentRoot);
    LoadFile(filename, &mapData, &size);

    while(*offsets != 0)
    {
        LevelData* levelData = (LevelData*)(mapData.get() + *offsets);
        assert(levelData->LayerWidth == 64);
        assert(levelData->LayerHeight == 64);

        LevelInfo level;
        strcpy_s(level.Name, levelData->Name);
        level.Data.resize(64 * 64);

        uint16_t* compressed = (uint16_t*)(mapData.get() + levelData->Layer1Offset);
        uint16_t decompressedSize = *compressed++;

        std::unique_ptr<uint16_t[]> decompressed(new uint16_t[decompressedSize / 2]);
        UncompressCarmack(compressed, decompressed.get(), decompressedSize);

        uint16_t expandedSize = decompressed[0];
        assert(level.Data.capacity() * 2 == expandedSize);
        std::unique_ptr<uint16_t[]> expanded(new uint16_t[expandedSize/ 2]);
        ExpandRLEW(decompressed.get() + 1, expanded.get(), expandedSize);

        const uint16_t* p = (const uint16_t*)expanded.get();
        for (uint32_t i = 0; i < 64 * 64; ++i, ++p)
        {
            int16_t id = (int16_t)(*p - 1) * 2;
            if (id >= 0)
            {
                level.Data[i] = id;
            }
            else
            {
                level.Data[i] = -1;
            }
        }

        _levels.push_back(level);
        ++offsets;
    }
}

uint32_t Level::GetNumLevels() const
{
    return (uint32_t)_levels.size();
}

const int16_t* Level::GetLevel(uint32_t levelIndex) const
{
    assert(levelIndex < _levels.size());
    return _levels[levelIndex].Data.data();
}

_Use_decl_annotations_
void LoadFile(const wchar_t* filename, std::unique_ptr<uint8_t[]>* buffer, size_t* size)
{
    assert(buffer);
    assert(size);

    *size = 0;
    buffer->reset();

    FileHandle file(CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
    if (!file.IsValid()) throw std::exception();

    DWORD fileSize = GetFileSize(file.Get(), nullptr);
    std::unique_ptr<uint8_t[]> data(new uint8_t[fileSize]);

    DWORD read;
    if (!ReadFile(file.Get(), data.get(), fileSize, &read, nullptr))
    {
        throw std::exception();
    }

    *size = (size_t)fileSize;
    buffer->swap(data);
}

_Use_decl_annotations_
void UncompressCarmack(const uint16_t* compressed, uint16_t* decompressed, uint16_t decompressedSize)
{
    static const uint8_t NearTag = 0xA7;
    static const uint8_t FarTag = 0xA8;

    uint16_t word;
    uint8_t high;
    uint16_t count, offset;

    uint16_t* base = decompressed;
    uint16_t* end = decompressed + decompressedSize / 2;
    uint16_t* copy = nullptr;

    while (decompressed < end)
    {
        word = *compressed++;
        high = word >> 8;
        count = word & 0xff;

        if (high == NearTag || high == FarTag)
        {
            if (count == 0)
            {
                // special case, not really near tag (just a word of that same value)
                const uint8_t* p = (const uint8_t*)compressed;
                word |= *p;
                ++p;
                compressed = (const uint16_t*)p;
                *decompressed++ = word;
            }
            else if (high == NearTag)
            {
                const uint8_t* p = (const uint8_t*)compressed;
                offset = *p;
                ++p;
                compressed = (const uint16_t*)p;

                copy = decompressed - offset;
                while (count--)
                {
                    *decompressed++ = *copy++;
                }
            }
            else
            {
                offset = *compressed++;
                copy = base + offset;
                while (count--)
                {
                    *decompressed++ = *copy++;
                }
            }
        }
        else
        {
            *decompressed++ = word;
        }
    }
}

_Use_decl_annotations_
void ExpandRLEW(const uint16_t* compressed, uint16_t* expanded, uint16_t expandedSize)
{
    uint16_t* end = nullptr;
    uint16_t value;
    uint16_t count;
    uint32_t i;

    end = expanded + expandedSize / 2;

    while (expanded < end)
    {
        value = *compressed++;
        if(value != MapFileHeader::MagicNumber)
        {
            // uncompressed
            *expanded++ = value;
        }
        else 
        {
            // compressed
            count = *compressed++;
            value = *compressed++;
            for(i = 0; i < count; ++i)
            {
                *expanded++ = value;
            }
        }
    };
}

