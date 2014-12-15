#include "stdafx.h"
#include "q2pak.h"

#pragma pack(push,1)

struct pak_fileheader_t
{
    char signature[4];
    int dirOffset;
    int dirLength;
};

struct pak_entry_t
{
    char filename[56];
    int offset;
    int length;
};

#pragma pack(pop)

static HANDLE g_pakFile = INVALID_HANDLE_VALUE;
static pak_entry_t* g_entries = nullptr;
static unsigned int g_numEntries = 0;

void
PakInit(
    PCWSTR filename
    )
{
    if (g_pakFile != INVALID_HANDLE_VALUE)
    {
        RIP(L"Pak file already initialized.\n");
    }

    g_pakFile = CreateFile(filename, GENERIC_READ, 0, nullptr,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (g_pakFile == INVALID_HANDLE_VALUE)
    {
        RIP(L"Error opening pak file: %s. %d\n", filename, GetLastError());
    }

    pak_fileheader_t header = {};
    DWORD bytesRead = 0;
    if (!ReadFile(g_pakFile, &header, sizeof(header), &bytesRead, nullptr))
    {
        RIP(L"Error reading header from pak file %s. %d\n", filename, GetLastError());
    }

    g_numEntries = header.dirLength / sizeof(pak_entry_t);
    g_entries = new pak_entry_t[g_numEntries];

    if (SetFilePointer(g_pakFile, header.dirOffset, nullptr, FILE_BEGIN) == 0)
    {
        RIP(L"Failed to seek to directory in pak file %s. %d\n", filename, GetLastError());
    }

    if (!ReadFile(g_pakFile, g_entries, header.dirLength, &bytesRead, nullptr))
    {
        RIP(L"Error reading directory from pak file %s. %d\n", filename, GetLastError());
    }
}

void
PakShutdown()
{
    if (g_entries != nullptr)
    {
        delete [] g_entries;
        g_entries = nullptr;
        g_numEntries = 0;
    }

    if (g_pakFile != INVALID_HANDLE_VALUE)
    {
        CloseHandle(g_pakFile);
        g_pakFile = INVALID_HANDLE_VALUE;
    }
}

void*
PakLoadFile(
    PCSTR filename
    )
{
    if (g_pakFile == nullptr)
    {
        RIP(L"Pak file not initialized.\n");
    }

    pak_entry_t* entry = g_entries;
    int count = g_numEntries;
    while (count-- > 0)
    {
        if (_stricmp(entry->filename, filename) == 0)
        {
            break;
        }
        ++entry;
    }
    if (count < 0)
    {
        RIP(L"Filename %s not found in pak.\n", filename);
    }

    if (SetFilePointer(g_pakFile, entry->offset, nullptr, FILE_BEGIN) == 0)
    {
        RIP(L"Failed to seek to file %s in pak file. %d\n", filename, GetLastError());
    }

    unsigned char* data = new unsigned char[entry->length];
    DWORD bytesRead = 0;
    if (!ReadFile(g_pakFile, data, entry->length, &bytesRead, nullptr))
    {
        RIP(L"Error reading file %s from pak file. %d\n", filename, GetLastError());
    }

    return data;
}

uint32_t
PakFileSize(
    PCSTR filename
    )
{
    if (g_pakFile == nullptr)
    {
        RIP(L"Pak file not initialized.\n");
    }

    pak_entry_t* entry = g_entries;
    int count = g_numEntries;
    while (count-- > 0)
    {
        if (_stricmp(entry->filename, filename) == 0)
        {
            return entry->length;
        }
        ++entry;
    }
    RIP(L"Filename %s not found in pak.\n", filename);
    return 0;
}
