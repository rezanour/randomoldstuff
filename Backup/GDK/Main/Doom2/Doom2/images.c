#include "common.h"
#include "images.h"
#include "graphicsp.h"
#include "mapp.h"

//
// Image related methods
//
// Ex: loading sprites/flats, drawing them, managing them, etc...
//

#pragma pack(push, 1)

typedef struct
{
    int NumPatches;
    char Patches[1][NAME_SIZE];
} patchnames_t;


typedef struct
{
    short xOrigin;
    short yOrigin;
    short Patch;
    short StepDir;
    short Colormap;
} mappatch_t;


typedef struct
{
    char Name[NAME_SIZE];
    int Masked;
    short Width;
    short Height;
    int _unused;
    short NumPatches;
    mappatch_t Patches[1]; // size == NumPatches
} maptexture_t;


typedef struct
{
    int NumTextures;
    int Offsets[1];
} texturetable_t;


#pragma pack(pop)


static patchnames_t* g_patchNames = NULL;
static texturetable_t* g_mapTextures = NULL;


// private Images helper methods
maptexture_t* FindTexture(const char* name);
const char* GetPatchName(int patch);


//
// public GraphicsImages methods
//

BOOL ImagesInitialize()
{
    // only initialize if not already initialized
    if (!g_patchNames)
    {
        long size;

        if (!WadGetLump("PNAMES", (void**)&g_patchNames, &size))
        {
            DebugOut("ImagesInitialize: Failed to load patch names lump");
            return FALSE;
        }

        if (!WadGetLump("TEXTURE1", (void**)&g_mapTextures, &size))
        {
            DebugOut("ImagesInitialize: Failed to load TEXTURE1 lump");
            return FALSE;
        }

        {
            maptexture_t* t = FindTexture("SWCOM1");
            if (t)
            {
                DebugOut(GetPatchName(t->Patches[0].Patch));
            }
            t = t;
        }
    }

    return TRUE;
}

void ImagesUninitialize()
{
    if (g_mapTextures)
    {
        MemoryFree(g_mapTextures);
        g_mapTextures = NULL;
    }

    if (g_patchNames)
    {
        MemoryFree(g_patchNames);
        g_patchNames = NULL;
    }
}

sprite_t* ImagesLoadSprite(const char* name)
{
    long size;

    assert(name);

    return (sprite_t*)MemoryGetRefCountedLump(name, &size);
}

void ImagesFreeSprite(sprite_t* sprite)
{
    assert(sprite);

    MemoryReleaseRefCounted(sprite);
}

byte_t* ImagesLoadFlat(const char* name)
{
    long size;

    assert(name);

    return (byte_t*)MemoryGetRefCountedLump(name, &size);
}

void ImagesFreeFlat(byte_t* flat)
{
    assert(flat);

    MemoryReleaseRefCounted(flat);
}

texture_t* ImagesLoadTexture(const char* name)
{
    texture_t* texture = NULL;
    maptexture_t* maptexture;

    assert(name);

    maptexture = FindTexture(name);
    if (maptexture)
    {
        texture = (texture_t*)MemoryAlloc("texture", sizeof(texture_t) + maptexture->NumPatches * sizeof(texturesprite_t));
        if (texture)
        {
            int i;

            WadCopyDoomString(texture->Name, maptexture->Name);
            texture->Width = maptexture->Width;
            texture->Height = maptexture->Height;
            texture->NumSprites = maptexture->NumPatches;

            for (i = 0; i < texture->NumSprites; ++i)
            {
                const char* patchName = GetPatchName(maptexture->Patches[i].Patch);

                texture->Sprites[i].xOffset = maptexture->Patches[i].xOrigin;
                texture->Sprites[i].yOffset = maptexture->Patches[i].yOrigin;
                texture->Sprites[i].Sprite = MapGetSprite(patchName);
            }
        }
        else
        {
            DebugOut("ImagesLoadTexture: Failed to allocate memory for texture: %s", name);
        }
    }

    return texture;
}

void ImagesFreeTexture(texture_t* texture)
{
    assert(texture);

    MemoryFree(texture);
}


//
// Private helper methods
//

maptexture_t* FindTexture(const char* name)
{
    int i;

    assert(name);
    assert(g_mapTextures);

    for (i = 0; i < g_mapTextures->NumTextures; ++i)
    {
        maptexture_t* t = (maptexture_t*)&(((byte_t*)g_mapTextures)[g_mapTextures->Offsets[i]]);
        if (WadCompareDoomString(t->Name, name))
        {
            return t;
        }
    }

    return NULL;
}

const char* GetPatchName(int patch)
{
    assert(g_patchNames);
    assert(patch < g_patchNames->NumPatches);

    return g_patchNames->Patches[patch];
}

void GraphicsDrawSprite(int x, int y, resource_t resource)
{
    sprite_t* sprite = (sprite_t*)ResourceManagerDataFromHandle(resource_type_sprite, resource);

    if (sprite)
    {
        int i, j;

        for (i = 0; i < sprite->Width; ++i)
        {
            int offset = sprite->ColumnOffsets[i];
            byte_t* p = &((byte_t*)sprite)[offset];

            byte_t rowstart = 0;
            while (rowstart != 255)
            {
                rowstart = *p;
                ++p;

                if (rowstart != 255)
                {
                    byte_t pixelcount = *p;
                    ++p;
                    ++p; // dummy value

                    for (j = 0; j < pixelcount; ++j)
                    {
                        byte_t color = *p;
                        ++p;

                        RasterizePixel(x - sprite->Left + i, y - sprite->Top + rowstart + j, color);
                    }

                    ++p; // dummy value
                }
            }
        }
    }
}

