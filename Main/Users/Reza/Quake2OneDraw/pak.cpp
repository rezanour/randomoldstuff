#include "precomp.h"
#include "pak.h"

#pragma pack(push,1)
struct PAK_HEADER
{
    char signature[4]; 
    int dirOffset;
    int dirLength;
};
#pragma pack(pop)

_Use_decl_annotations_
Pak::Pak(const wchar_t* filename) :
    _chunkStartOffset(0)
{
    _pakFile.Attach(CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
    CHECKGLE(_pakFile.IsValid());

    PAK_HEADER header;
    DWORD read;
    CHECKGLE(ReadFile(_pakFile.Get(), &header, sizeof(header), &read, nullptr));

    _directoryLength = header.dirLength / sizeof(PAK_DIRECTORY_ENTRY);
    _directory.reset(new PAK_DIRECTORY_ENTRY[_directoryLength]);

    SetFilePointer(_pakFile.Get(), header.dirOffset, nullptr, FILE_BEGIN);
    CHECKGLE(ReadFile(_pakFile.Get(), _directory.get(), header.dirLength, &read, nullptr));

    _chunkStartOffset = header.dirOffset = header.dirLength;
}

_Use_decl_annotations_
std::unique_ptr<uint8_t[]> Pak::GetChunk(const char* name, uint32_t* size)
{
    // Find the chunk directory entry
    PAK_DIRECTORY_ENTRY* entry = _directory.get();
    for (uint32_t i = 0; i < _directoryLength; ++i, ++entry)
    {
        if (_stricmp(entry->filename, name) == 0)
        {
            std::unique_ptr<uint8_t[]> buffer(new uint8_t[entry->length]);
            DWORD read;
            SetFilePointer(_pakFile.Get(), entry->offset, nullptr, FILE_BEGIN);
            CHECKGLE(ReadFile(_pakFile.Get(), buffer.get(), entry->length, &read, nullptr));

            *size = entry->length;
            return buffer;
        }
    }

    Error("Name not found in pak: %s", name);
    return nullptr;
}
