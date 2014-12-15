#ifndef _WAD_H_
#define _WAD_H_

#define CONTENT_ROOT        "..\\..\\Doom2Content\\"

BOOL WadOpen(const char* path);
void WadClose();
BOOL WadGetLump(const char* lump, void** lumpMemory, long* lumpMemorySize);
BOOL WadGetSubLump(const char* rootLump, const char* lump, void** lumpMemory, long* lumpMemorySize);

BOOL WadCompareDoomString(const char* name1, const char* name2);
void WadCopyDoomString(char* dest, const char* source);

#endif