#include "stdafx.h"

struct QUAKE_CONTENT
{
    char* name;
    char* md2Model;
    char* skin1;
    char* skin2;
    wchar_t* gameContentPath;
};

QUAKE_CONTENT quake_content[] = 
{
    {"soldier","models/monsters/soldier/tris.md2", "models/monsters/soldier/skin.pcx", "models/monsters/soldier/pain.pcx", L"\\enemies\\soldier.object\\"},
    /*
    {"soldier","models/monsters/soldier/tris.md2", "models/monsters/soldier/skin.pcx", "models/monsters/soldier/pain.pcx", L"..\\..\\..\\Quake2\\Content\\enemies\\soldier.object\\"},
    {"medkit_medium","models/items/healing/medium/tris.md2", "models/items/healing/medium/skin.pcx", nullptr, L"..\\..\\..\\Quake2\\Content\\pickups\\medkit_medium.object\\"},
    {"medkit_large","models/items/healing/large/tris.md2", "models/items/healing/large/skin.pcx", nullptr, L"..\\..\\..\\Quake2\\Content\\pickups\\medkit_large.object\\"},
    {"medkit_stimpack","models/items/healing/stimpack/tris.md2", "models/items/healing/stimpack/skin.pcx", nullptr, L"..\\..\\..\\Quake2\\Content\\pickups\\medkit_stimpack.object\\"},
    {"bfg","models/weapons/g_bfg/tris.md2", "models/weapons/g_bfg/skin.pcx", nullptr, L"..\\..\\..\\Quake2\\Content\\weapons\\bfg.object\\"},
    {"brain","models/monsters/brain/tris.md2", "models/monsters/brain/skin.pcx", nullptr, L"..\\..\\..\\Quake2\\Content\\enemies\\brain.object\\"},
    */
};

int _tmain(int argc, _TCHAR* argv[])
{
    HRESULT hr = S_OK;
    std::vector<byte_t> data;
    std::wstring contentRoot;

    if (argc < 3)
    {
        printf("contentextractor.exe <pak file path> <content path>\n");
        return 0;
    }

    contentRoot = argv[2];

    // uncomment to list ALL PAK file chunks
    //CHECKHR(GetPakChunk(argv[1], "", data), L"Error listing all chunks");

    // Extract md2 objects
    for (size_t i = 0; i < ARRAYSIZE(quake_content); i++)
    {
        std::wstring contentDir;

        contentDir = contentRoot;
        contentDir.append(quake_content[i].gameContentPath);
        contentDir.append(L"mesh.geometry");
        CHECKHR(GetPakChunk(argv[1], quake_content[i].md2Model, data), L"Error finding md2 model");
        SaveMD2AsGeometryContent(contentDir.c_str(), quake_content[i].name, data.data(), (uint32_t)data.size());

        data.clear();

        contentDir = contentRoot;
        contentDir.append(quake_content[i].gameContentPath);
        contentDir.append(L"primary.texture");
        CHECKHR(GetPakChunk(argv[1], quake_content[i].skin1, data), L"Error finding md2 primary texture");

        SavePCXAsTextureContent(contentDir.c_str(), quake_content[i].name, data.data(), (uint32_t)data.size());
    }

Exit:

    return 0;
}
