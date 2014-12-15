#include "common.h"

FILE* g_wadFile = NULL;

#pragma pack(push,1)
typedef struct
{
    char Id[4]; // Should be "IWAD" or "PWAD"
    int  LumpCount;
    int  InfoTableOffests;
} wadinfo_t;

wadinfo_t g_wadInfo;

typedef struct
{
    int  FilePos;
    int  Size;
    char Name[8];
} filelump_t;

typedef struct
{
    char Name[8];
    int  Handle;
    int  Position;
    int  Size;
} lumpinfo_t;

#pragma pack(pop)

char* MakeLumpName(char* dest, char* src)
{
    ZeroMemory(dest, 9);
    memcpy(dest, src, 8);
    return dest;
}

BOOL WadOpen(const char* path)
{
    BOOL success = TRUE;

    if (!path)
    {
        DebugOut("Failed to open WAD file because a NULL WAD file path as used");
        return FALSE;
    }

    if (success)
    {
        if (g_wadFile)
        {
            DebugOut("WAD file is already open, the existing opened WAD will be used");
            return TRUE;
        }
    }

    if (success)
    {
        if (fopen_s(&g_wadFile, path, "rb") != 0)
        {
            success = FALSE;
            DebugOut("Failed to open WAD file at location: %s", path);
        }
    }

    if (success)
    {
        if (fread_s(&g_wadInfo, sizeof(g_wadInfo), sizeof(g_wadInfo), 1, g_wadFile) <= 0)
        {
            success = FALSE;
            DebugOut("Failed to read WAD file header information for WAD file: %s", path);
        }
    }

    if (success)
    {
        DebugOut("WAD File: %s", path);
        DebugOut("wadinfo_t.Id               = %c%c%c%c", g_wadInfo.Id[0],g_wadInfo.Id[1],g_wadInfo.Id[2],g_wadInfo.Id[3]);
        DebugOut("wadinfo_t.LumpCount        = %d", g_wadInfo.LumpCount);
        DebugOut("wadinfo_t.InfoTableOffests = %d", g_wadInfo.InfoTableOffests);
        DebugOut("-------------------------------------");
    }

    return success;
}

void WadClose()
{
    if (g_wadFile != NULL)
    {
        fclose(g_wadFile);
        g_wadFile = NULL;
    }
}

BOOL IsWadOpen()
{
    return (g_wadFile != NULL);
}

BOOL WadGetLump(const char* lump, void** lumpMemory, long* lumpMemorySize)
{
    return WadGetSubLump(NULL, lump, lumpMemory, lumpMemorySize);
}

BOOL WadGetSubLump(const char* rootLump, const char* lump, void** lumpMemory, long* lumpMemorySize)
{
    int lumpIndexRoot = -1, lumpIndex = 0;
    long totalLumpsDataSize = g_wadInfo.LumpCount * sizeof(filelump_t);
    filelump_t* lumps = NULL;
    char lumpName[9];
    int lumpFoundIndex = -1;

    if (!IsWadOpen())
    {
        DebugOut("Failed to read WAD lump because WAD file has not been opened");
        return FALSE;
    }

    if (!lump)
    {
        DebugOut("Failed to read WAD lump because a NULL lump id was specified");
        return FALSE;
    }

    if (!lumpMemory)
    {
        DebugOut("Failed to read WAD lump because a NULL lump data pointer was specified");
        return FALSE;
    }

    if (!lumpMemorySize)
    {
        DebugOut("Failed to read WAD lump because a NULL lump data size pointer was specified");
        return FALSE;
    }

    // Ensure out parameters are initialized
    *lumpMemory = NULL;
    *lumpMemorySize = 0;

    lumps = (filelump_t*)MemoryAlloc("WAD File Lumps", totalLumpsDataSize);
    if (!lumps)
    {
        DebugOut("Failed to allocate memory for lump data");
        return FALSE;
    }

    if (fseek(g_wadFile, g_wadInfo.InfoTableOffests, SEEK_SET) == 0)
    {
        if (fread_s(lumps, totalLumpsDataSize, totalLumpsDataSize, 1, g_wadFile) > 0)
        {
            for (lumpIndex = 0; lumpIndex < g_wadInfo.LumpCount; lumpIndex++)
            {
                if (rootLump != NULL && lumpIndexRoot < 0)
                {
                    if (WadCompareDoomString(rootLump, MakeLumpName(lumpName, lumps[lumpIndex].Name)))
                    {
                        DebugOut("Found requested lump: %s", rootLump);
                        lumpIndexRoot = lumpIndex;
                    }
                }
                else
                {
                    if (WadCompareDoomString(lump, MakeLumpName(lumpName, lumps[lumpIndex].Name)))
                    {
                        DebugOut("Found requested lump: %s", lump);
                        lumpFoundIndex = lumpIndex;
                        break;
                    }
                }
            }
        }
        else
        {
            DebugOut("Failed to read WAD File Lumps: lump table data size = %d",totalLumpsDataSize);
        }
    }
    else
    {
        DebugOut("Failed to seek to beginning of WAD File Lump offsets table: table offset = %d",g_wadInfo.InfoTableOffests);
    }

    // We have the lump, seek and read
    if (lumpFoundIndex != -1)
    {
        *lumpMemory = MemoryAlloc(lump,lumps[lumpFoundIndex].Size);
        if (*lumpMemory)
        {
            *lumpMemorySize = lumps[lumpFoundIndex].Size;
            if (fseek(g_wadFile, lumps[lumpFoundIndex].FilePos, SEEK_SET) == 0)
            {
                if (fread_s(*lumpMemory, *lumpMemorySize, *lumpMemorySize, 1, g_wadFile) <= 0)
                {
                    DebugOut("Failed to read %s lump data", lump);
                    lumpFoundIndex = -1; // force FALSE return and allow cleanup of other memory
                }
            }
            else
            {
                DebugOut("Failed to seek to %s lump data", lump);
                lumpFoundIndex = -1; // force FALSE return and allow cleanup of other memory
            }
        }
        else
        {
            DebugOut("Failed to allocate memory for %s lump data", lump);
            lumpFoundIndex = -1; // force FALSE return and allow cleanup of other memory
        }
    }

    if (lumps)
    {
        MemoryFree(lumps);
    }

    // If a failure occurs, ensure we free any allocated memory for the outgoing lump data
    if (lumpFoundIndex == -1)
    {
        DebugOut("Failed to find requested lump: %s", lump);
        if (*lumpMemory)
        {
            MemoryFree(*lumpMemory);
        }
    }

    return (lumpFoundIndex != -1);
}

BOOL WadCompareDoomString(const char* name1, const char* name2)
{
    int i;

    assert(name1 && name2);

    for (i = 0; i < NAME_SIZE; ++i)
    {
        char a = name1[i];
        char b = name2[i];

        if (a >= 'a' && a <= 'z')
        {
            a += 'A' - 'a';
        }
        if (b >= 'a' && b <= 'z')
        {
            b += 'A' - 'a';
        }

        if (a != b)
        {
            return FALSE;
        }

        if (a == 0)
        {
            break;
        }
    }

    return TRUE;
}

void WadCopyDoomString(char* dest, const char* source)
{
    int i;

    assert(dest && source);

    for (i = 0; i < NAME_SIZE; ++i)
    {
        const char a = source[i];

        dest[i] = a;

        if (a == 0)
        {
            break;
        }
    }
}

void DebugWadDumpLumps(BOOL headerLumpsOnly)
{
    int lumpIndex = 0;
    long totalLumpsDataSize = g_wadInfo.LumpCount * sizeof(filelump_t);
    filelump_t* lumps = NULL;
    char lumpName[9];
    BOOL outputLump = TRUE;

    if (!IsWadOpen())
    {
        DebugOut("Failed to dump lumps because WAD file has not been opened");
        return;
    }

    lumps = (filelump_t*)MemoryAlloc("WAD File Lumps", totalLumpsDataSize);
    if (!lumps)
    {
        DebugOut("Failed to allocate memory for lump data");
        return;
    }

    if (fseek(g_wadFile, g_wadInfo.InfoTableOffests, SEEK_SET) == 0)
    {
        if (fread_s(lumps, totalLumpsDataSize, totalLumpsDataSize, 1, g_wadFile) > 0)
        {
            for (lumpIndex = 0; lumpIndex < g_wadInfo.LumpCount; lumpIndex++)
            {
                outputLump = TRUE;
                if (headerLumpsOnly && lumps[lumpIndex].Size != 0)
                {
                    outputLump = FALSE;
                }
                if (outputLump)
                {
                    DebugOut("Lump: %d of %d", (lumpIndex + 1), g_wadInfo.LumpCount);
                    DebugOut("  Name    = %s", MakeLumpName(lumpName, lumps[lumpIndex].Name));
                    DebugOut("  Size    = %d", lumps[lumpIndex].Size);
                    DebugOut("  FilePos = %d", lumps[lumpIndex].FilePos);
                    DebugOut("--------------------------");
                }
            }
        }
        else
        {
            DebugOut("Failed to read WAD File Lumps: lump table data size = %d",totalLumpsDataSize);
        }
    }
    else
    {
        DebugOut("Failed to seek to beginning of WAD File Lump offsets table: table offset = %d",g_wadInfo.InfoTableOffests);
    }

    if (lumps)
    {
        MemoryFree(lumps);
    }
}

void DebugWadSaveLumpToFile(const char* name, const char* filePath)
{
    void* lumpMemory = NULL;
    long lumpMemorySize = 0;
    FILE* lumpFile = NULL;

    if (WadGetLump(name, &lumpMemory, &lumpMemorySize))
    {
        if (fopen_s(&lumpFile, filePath, "wb") == 0)
        {
            if (lumpFile)
            {
                fwrite(lumpMemory, lumpMemorySize, 1, lumpFile);
                fclose(lumpFile);
            }
        }

        MemoryFree(lumpMemory);
    }
}