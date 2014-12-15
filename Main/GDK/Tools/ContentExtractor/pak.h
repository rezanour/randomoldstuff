#pragma once

#pragma pack(push,1)

struct PAK_HEADER
{
    char signature[4]; 
    int dirOffset;
    int dirLength;
};

struct PAK_DIRECTORY_ENTRY
{
    char filename[56];
    int offset;
    int length;
};

#pragma pack(pop)

HRESULT GetPakChunk(_In_ const wchar_t* pakFilePath, _In_ const char* name, _Out_ std::vector<byte_t>& data);
HRESULT SavePakChunkAsFile(_In_ const wchar_t* pakFilePath, _In_ const char* name, _In_ const wchar_t* filePath);
HRESULT SaveFilesFromPak(_In_ const wchar_t* pakFilePath, _In_ const wchar_t* fileExtension, _In_ const wchar_t* rootDir);
HRESULT SaveWALFilesAsTextureContentFromPak(_In_ const wchar_t* pakFilePath, _In_ const wchar_t* rootDir, _In_ std::vector<RGBQUAD> bmpal);
HRESULT SavePCXPicsFilesAsTextureContentFromPak(_In_ const wchar_t* pakFilePath, _In_ const wchar_t* rootDir);
HRESULT SaveBSPLevelsAsObjectsFromPak(_In_z_ const wchar_t* pakFilePath, _In_z_ const wchar_t* rootDir);
