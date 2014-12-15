#ifndef _W3DTYPES_H_
#define _W3DTYPES_H_

#ifdef WOLF3D_SHAREWARE_VERSION
#define mapheadFilePath  CONTENT_ROOT"raw\\maphead.wl1"
#define gamemapsFilePath CONTENT_ROOT"raw\\gamemaps.wl1"
#define vgaheadFilePath  CONTENT_ROOT"raw\\vgahead.wl1"
#define vgadictFilePath  CONTENT_ROOT"raw\\vgadict.wl1"
#define vgagraphFilePath CONTENT_ROOT"raw\\vgagraph.wl1"
#define vswapFilePath    CONTENT_ROOT"raw\\vswap.wl1"
#else
#define mapheadFilePath  CONTENT_ROOT"raw\\maphead.wl6"
#define gamemapsFilePath CONTENT_ROOT"raw\\gamemaps.wl6"
#define vgaheadFilePath  CONTENT_ROOT"raw\\vgahead.wl6"
#define vgadictFilePath  CONTENT_ROOT"raw\\vgadict.wl6"
#define vgagraphFilePath CONTENT_ROOT"raw\\vgagraph.wl6"
#define vswapFilePath    CONTENT_ROOT"raw\\vswap.wl6"
#endif
#define fontFilePath     CONTENT_ROOT"raw\\menufont.wl1"

#pragma pack(push,1)

typedef struct _w3dFontCharacter
{
    char ch;
    RECT rect;
}w3dFontCharacter;

typedef struct _w3dLevel
{
    unsigned int   Layer1Offset;
    unsigned int   Layer2Offset;
    unsigned int   Layer3Offset;
    unsigned short Layer1SizeBytes;
    unsigned short Layer2SizeBytes;
    unsigned short Layer3SizeBytes;
    unsigned short LayerWidth;
    unsigned short LayerHeight;
    char           Name[16];
    char           Signature[4];
}w3dLevel;

typedef struct _w3dSpriteData
{
    unsigned short leftpix;
    unsigned short rightpix;
    unsigned short offsets[64];
}w3dSpriteData;

typedef struct _w3dVSwapHeader
{
    unsigned short NumChunks;
    unsigned short SpritesOffset;
    unsigned short SoundsOffset;
}w3dVSwapHeader;

typedef struct _w3dVSwapChunk
{
    unsigned long offset; // offset of chunk in file
    unsigned short length; // chunk data byte size
}w3dVSwapChunk;

typedef struct _w3dLayer
{
    unsigned int     Offset;
    unsigned short   SizeBytes;
    unsigned short   Width;
    unsigned short   Height;
    unsigned short*  Data; // memory was allocated using malloc()
}w3dLayer;

typedef struct _w3dLevelData
{
    unsigned short* layer1; // walls (memory was allocated using malloc())
    unsigned short* layer2; // objects (memory was allocated using malloc())
    unsigned short* layer3; // other (memory was allocated using malloc())
}w3dLevelData;

typedef struct _w3dPicture
{
    unsigned short width;
    unsigned short height;
}w3dPicture;

typedef struct _huffnode
{
    unsigned short bit0;
    unsigned short bit1;
}huffnode;

typedef struct _huffnode32
{
    unsigned int bit0;
    unsigned int bit1;
}huffnode32;

typedef struct _w3dRGB
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
}w3dRGB;

#pragma pack(pop)

#endif