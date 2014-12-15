#pragma once

class Pak
{
public:
    Pak(_In_z_ const wchar_t* filename);

    std::unique_ptr<uint8_t[]> GetChunk(_In_z_ const char* name, _Out_ uint32_t* size);

private:

#pragma pack(push, 1)
    struct PAK_DIRECTORY_ENTRY
    {
        char filename[56];
        int offset;
        int length;
    };
#pragma pack(pop)

private:
    Pak(const Pak&);
    Pak& operator= (const Pak&);

private:
    FileHandle _pakFile;
    std::unique_ptr<PAK_DIRECTORY_ENTRY[]> _directory;
    uint32_t _directoryLength;
    uint32_t _chunkStartOffset;
};
