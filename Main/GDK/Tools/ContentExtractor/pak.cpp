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

HRESULT SavePakChunkAsFile(_In_ const wchar_t* pakFilePath, _In_ const char* name, _In_ const wchar_t* filePath)
{
    if (!pakFilePath || !name || !filePath)
    {
        return E_INVALIDARG;
    }

    std::vector<byte_t> pakData;
    HRESULT hr = S_OK;
    ComPtr<IStream> outputFile;
    ULONG bytesWritten = 0;

    CHECKHR(GetPakChunk(pakFilePath, name, pakData), L"Error reading PAK data");
    CHECKHR(SHCreateStreamOnFile(filePath, STGM_CREATE|STGM_WRITE, &outputFile), L"Error opening output file");
    CHECKHR(outputFile->Write(pakData.data(), (ULONG)pakData.size(), &bytesWritten), L"Error reading entire PAK data entry to file");

Exit:

    return hr;
}

HRESULT SaveFilesFromPak(_In_ const wchar_t* pakFilePath, _In_ const wchar_t* fileExtension, _In_ const wchar_t* rootDir)
{
    HRESULT hr = S_OK;
    ComPtr<IStream> pakFile;
    PAK_HEADER header;
    size_t numEntries = 0;
    ULONG bytesRead = 0;
    std::vector<byte_t> data;

    std::vector<PAK_DIRECTORY_ENTRY> entries;
    
    CHECKHR(SHCreateStreamOnFile(pakFilePath, STGM_READ, &pakFile), L"Error opening PAK file");
    CHECKHR(pakFile->Read(&header, sizeof(header), &bytesRead), L"Error reading PAK file header");
    numEntries = (size_t)header.dirLength / sizeof(PAK_DIRECTORY_ENTRY);
    
    CHECKHR(SeekTo(pakFile.Get(), STREAM_SEEK_SET, header.dirOffset), L"Error seeking to PAK file header");
    for (size_t entryIndex = 0; entryIndex < numEntries; entryIndex++)
    {
        PAK_DIRECTORY_ENTRY entry;
        CHECKHR(pakFile->Read(&entry, sizeof(entry), &bytesRead), L"Error reading PAK file entry");

        std::string pakEntryFilenameA = entry.filename;
        std::wstring pakEntryFilename = AnsiToWide(pakEntryFilenameA);

        if (pakEntryFilename.rfind(fileExtension) != std::wstring::npos)
        {
            for (int i = 0; i < ARRAYSIZE(entry.filename); i++)
            {
                if (entry.filename[i] == '/')
                {
                    entry.filename[i] = '\\';
                }
            }

            entries.push_back(entry);
        }
    }

    for (size_t entryIndex = 0; entryIndex < entries.size(); entryIndex++)
    {
        std::string pakEntryFilenameA = entries[entryIndex].filename;
        std::wstring pakEntryFilename = AnsiToWide(pakEntryFilenameA);

        if (pakEntryFilename.rfind(fileExtension) != std::wstring::npos)
        {
            std::wstring filePath = rootDir;
            filePath.append(L"\\");
            filePath.append(pakEntryFilename);

            SHCreateDirectory(nullptr, DirectoryRootFromPath(filePath).c_str());

            ComPtr<IStream> outputFile;
            ULONG bytesWritten = 0;

            data.resize(entries[entryIndex].length);
            CHECKHR(SeekTo(pakFile.Get(), STREAM_SEEK_SET, entries[entryIndex].offset), L"Error seeking to PAK file entry");
            CHECKHR(pakFile->Read(data.data(), entries[entryIndex].length, &bytesRead), L"Error reading PAK file entry data");

            CHECKHR(SHCreateStreamOnFile(filePath.c_str(), STGM_CREATE|STGM_WRITE, &outputFile), L"Error opening output file");
            CHECKHR(outputFile->Write(data.data(), (ULONG)data.size(), &bytesWritten), L"Error reading entire PAK data entry to file");

            wprintf(L"Saving %ws ... \n", pakEntryFilename.c_str());

            data.clear();
        }
    }

Exit:

    return hr;
}

HRESULT SavePCXPicsFilesAsTextureContentFromPak(_In_ const wchar_t* pakFilePath, _In_ const wchar_t* rootDir)
{
    HRESULT hr = S_OK;
    ComPtr<IStream> pakFile;
    PAK_HEADER header;
    size_t numEntries = 0;
    ULONG bytesRead = 0;
    std::vector<byte_t> data;

    std::vector<PAK_DIRECTORY_ENTRY> entries;
    
    CHECKHR(SHCreateStreamOnFile(pakFilePath, STGM_READ, &pakFile), L"Error opening PAK file");
    CHECKHR(pakFile->Read(&header, sizeof(header), &bytesRead), L"Error reading PAK file header");
    numEntries = (size_t)header.dirLength / sizeof(PAK_DIRECTORY_ENTRY);
    
    CHECKHR(SeekTo(pakFile.Get(), STREAM_SEEK_SET, header.dirOffset), L"Error seeking to PAK file header");
    for (size_t entryIndex = 0; entryIndex < numEntries; entryIndex++)
    {
        PAK_DIRECTORY_ENTRY entry;
        CHECKHR(pakFile->Read(&entry, sizeof(entry), &bytesRead), L"Error reading PAK file entry");

        std::string pakEntryFilenameA = entry.filename;
        std::wstring pakEntryFilename = AnsiToWide(pakEntryFilenameA);

        if (pakEntryFilename.rfind(L".pcx") != std::wstring::npos && pakEntryFilename.find(L"pics/") != std::wstring::npos)
        {
            for (int i = 0; i < ARRAYSIZE(entry.filename); i++)
            {
                if (entry.filename[i] == '/')
                {
                    entry.filename[i] = '\\';
                }
            }

            entries.push_back(entry);
        }
    }

    for (size_t entryIndex = 0; entryIndex < entries.size(); entryIndex++)
    {
        std::string pakEntryFilenameA = entries[entryIndex].filename;
        std::wstring pakEntryFilename = AnsiToWide(pakEntryFilenameA);

        if (pakEntryFilename.rfind(L".pcx") != std::wstring::npos)
        {
            std::wstring filePath = rootDir;
            filePath.append(L"\\");
            filePath.append(pakEntryFilename);

            SHCreateDirectory(nullptr, DirectoryRootFromPath(filePath).c_str());

            data.resize(entries[entryIndex].length);
            CHECKHR(SeekTo(pakFile.Get(), STREAM_SEEK_SET, entries[entryIndex].offset), L"Error seeking to PAK file entry");
            CHECKHR(pakFile->Read(data.data(), entries[entryIndex].length, &bytesRead), L"Error reading PAK file entry data");

            // Convert .pcx to .texture
            filePath.append(L".texture");
            SavePCXAsTextureContent(filePath.c_str(), "", data.data(), (uint32_t)data.size());

            wprintf(L"Saving %ws ... \n", pakEntryFilename.c_str());

            data.clear();
        }
    }

Exit:

    return hr;
}

HRESULT SaveWALFilesAsTextureContentFromPak(_In_ const wchar_t* pakFilePath, _In_ const wchar_t* rootDir, _In_ std::vector<RGBQUAD> bmpal)
{
    HRESULT hr = S_OK;
    ComPtr<IStream> pakFile;
    PAK_HEADER header;
    size_t numEntries = 0;
    ULONG bytesRead = 0;
    std::vector<byte_t> data;

    std::vector<PAK_DIRECTORY_ENTRY> entries;
    
    CHECKHR(SHCreateStreamOnFile(pakFilePath, STGM_READ, &pakFile), L"Error opening PAK file");
    CHECKHR(pakFile->Read(&header, sizeof(header), &bytesRead), L"Error reading PAK file header");
    numEntries = (size_t)header.dirLength / sizeof(PAK_DIRECTORY_ENTRY);
    
    CHECKHR(SeekTo(pakFile.Get(), STREAM_SEEK_SET, header.dirOffset), L"Error seeking to PAK file header");
    for (size_t entryIndex = 0; entryIndex < numEntries; entryIndex++)
    {
        PAK_DIRECTORY_ENTRY entry;
        CHECKHR(pakFile->Read(&entry, sizeof(entry), &bytesRead), L"Error reading PAK file entry");

        std::string pakEntryFilenameA = entry.filename;
        std::wstring pakEntryFilename = AnsiToWide(pakEntryFilenameA);

        if (pakEntryFilename.rfind(L".wal") != std::wstring::npos)
        {
            for (int i = 0; i < ARRAYSIZE(entry.filename); i++)
            {
                if (entry.filename[i] == '/')
                {
                    entry.filename[i] = '\\';
                }
            }

            entries.push_back(entry);
        }
    }

    for (size_t entryIndex = 0; entryIndex < entries.size(); entryIndex++)
    {
        std::string pakEntryFilenameA = entries[entryIndex].filename;
        std::wstring pakEntryFilename = AnsiToWide(pakEntryFilenameA);

        if (pakEntryFilename.rfind(L".wal") != std::wstring::npos)
        {
            std::wstring filePath = rootDir;
            filePath.append(L"\\");
            filePath.append(pakEntryFilename);

            SHCreateDirectory(nullptr, DirectoryRootFromPath(filePath).c_str());

            data.resize(entries[entryIndex].length);
            CHECKHR(SeekTo(pakFile.Get(), STREAM_SEEK_SET, entries[entryIndex].offset), L"Error seeking to PAK file entry");
            CHECKHR(pakFile->Read(data.data(), entries[entryIndex].length, &bytesRead), L"Error reading PAK file entry data");

            // Convert .wal to .texture
            filePath.append(L".texture");
            SaveWALAsTextureContent(filePath.c_str(), "", data.data(), (uint32_t)data.size(), bmpal);

            wprintf(L"Saving %ws ... \n", pakEntryFilename.c_str());

            data.clear();
        }
    }

Exit:

    return hr;
}

HRESULT SaveBSPLevelsAsObjectsFromPak(_In_z_ const wchar_t* pakFilePath, _In_z_ const wchar_t* rootDir)
{
    if (!pakFilePath || !rootDir)
    {
        return E_INVALIDARG;
    }

    HRESULT hr = S_OK;
    ComPtr<IStream> pakFile;
    PAK_HEADER header;
    size_t numEntries = 0;
    ULONG bytesRead = 0;
    std::vector<byte_t> data;
    std::vector<PAK_DIRECTORY_ENTRY> entries;
    
    CHECKHR(SHCreateStreamOnFile(pakFilePath, STGM_READ, &pakFile), L"Error opening PAK file");
    CHECKHR(pakFile->Read(&header, sizeof(header), &bytesRead), L"Error reading PAK file header");
    numEntries = (size_t)header.dirLength / sizeof(PAK_DIRECTORY_ENTRY);
    
    CHECKHR(SeekTo(pakFile.Get(), STREAM_SEEK_SET, header.dirOffset), L"Error seeking to PAK file header");
    for (size_t entryIndex = 0; entryIndex < numEntries; entryIndex++)
    {
        PAK_DIRECTORY_ENTRY entry;
        CHECKHR(pakFile->Read(&entry, sizeof(entry), &bytesRead), L"Error reading PAK file entry");

        std::string pakEntryFilenameA = entry.filename;
        std::wstring pakEntryFilename = AnsiToWide(pakEntryFilenameA);

        if (pakEntryFilename.rfind(L".bsp") != std::wstring::npos)
        {
            for (int i = 0; i < ARRAYSIZE(entry.filename); i++)
            {
                if (entry.filename[i] == '/')
                {
                    entry.filename[i] = '\\';
                }
            }

            entries.push_back(entry);
        }
    }

    for (size_t entryIndex = 0; entryIndex < entries.size(); entryIndex++)
    {
        std::string pakEntryFilenameA = entries[entryIndex].filename;
        std::wstring pakEntryFilename = AnsiToWide(pakEntryFilenameA);

        if (pakEntryFilename.rfind(L".bsp") != std::wstring::npos)
        {
            std::wstring filePath(rootDir);
            filePath.append(L"\\");
            filePath.append(pakEntryFilename.substr(0, pakEntryFilename.rfind(L".bsp")));

            SHCreateDirectory(nullptr, DirectoryRootFromPath(filePath).c_str());

            data.resize(entries[entryIndex].length);
            CHECKHR(SeekTo(pakFile.Get(), STREAM_SEEK_SET, entries[entryIndex].offset), L"Error seeking to PAK file entry");
            CHECKHR(pakFile->Read(data.data(), entries[entryIndex].length, &bytesRead), L"Error reading PAK file entry data");

            // Convert .bsp to a folder called .world
            filePath.append(L".world");
            CHECKHR(SaveBspAsObjectsUnderRoot(pakFilePath, data.data(), static_cast<uint32_t>(data.size()), filePath.c_str()), L"Error saving .bsp PAK file entry");

            wprintf(L"Saving %ws ... \n", pakEntryFilename.c_str());

            data.clear();
        }
    }
Exit:

    return hr;
}
