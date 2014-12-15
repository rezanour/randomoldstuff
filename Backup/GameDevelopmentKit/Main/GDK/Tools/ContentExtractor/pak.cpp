#include "stdafx.h"
using namespace Microsoft::WRL;

HRESULT GetPakChunk(_In_ const wchar_t* pakFilePath, _In_ const char* name, _Out_ std::vector<byte_t>& data)
{
    if (!pakFilePath ||!name)
    {
        return E_INVALIDARG;
    }

    HRESULT hr = S_OK;
    ComPtr<IStream> pakFile;
    PAK_HEADER header;
    size_t numEntries = 0;
    ULONG bytesRead = 0;

    CHECKHR(SHCreateStreamOnFile(pakFilePath, STGM_READ, &pakFile), L"Error opening PAK file");
    CHECKHR(pakFile->Read(&header, sizeof(header), &bytesRead), L"Error reading PAK file header");
    numEntries = (size_t)header.dirLength / sizeof(PAK_DIRECTORY_ENTRY);
    
    CHECKHR(SeekTo(pakFile.Get(), STREAM_SEEK_SET, header.dirOffset), L"Error seeking to PAK file header");
    for (size_t entryIndex = 0; entryIndex < numEntries; entryIndex++)
    {
        PAK_DIRECTORY_ENTRY entry;
        CHECKHR(pakFile->Read(&entry, sizeof(entry), &bytesRead), L"Error reading PAK file entry");

        // If an empty name is passed, make lots of noise by echoing out all entries
        if (strlen(name) == 0)
        {
            // output PAK entries
            printf("entry:  %s\n"
                   "length: %d\n"
                   "offset: %d\n", entry.filename, entry.length, entry.offset);
            printf("===========================================\n");
        }

        if (_strcmpi(name, entry.filename) == 0)
        {
            data.resize(entry.length);
            CHECKHR(SeekTo(pakFile.Get(), STREAM_SEEK_SET, entry.offset), L"Error seeking to PAK file entry");
            CHECKHR(pakFile->Read(data.data(), entry.length, &bytesRead), L"Error reading PAK file entry data");
            break;
        }
    }

Exit:

    return hr;
}