#include "common.h"
#include <windows.h>
#include "w3dtypes.h"

unsigned short MAPHEAD_FILE_MAGICNUMBER = 0xABCD;
unsigned short CARMACK_EXPAND_NEARTAG   = 0xA7;
unsigned short CARMACK_EXPAND_FARTAG    = 0xA8;
unsigned short EXPANDED_MAP_SIZE        = 64 * 64;

unsigned int g_mapDataOffsets[60];

void CarmackExpand(unsigned short* source, unsigned short* dest, unsigned short length);
void RLEWExpand(unsigned short* source, unsigned short* dest, long length, unsigned short rlewtag);
void HuffExpand(unsigned char *source, unsigned char *dest, long length, long c_len, huffnode32 *hufftable);

void SaveBitmap(char* fileName, long width, long height, unsigned char* pData, long dataSize, BITMAPFILEHEADER** ppbitmap);

#define FIRST_FONT_CHARACTER 32

w3dFontCharacter w3dFont[] =
{
    //      L    T   R    B
    {' ', {0, 0, 8, 13}},
    {'!', {16, 0, 22, 13}},
    {'"', {32, 0, 40, 13}},
    {'#', {48, 0, 58, 13}},
    {'$', {64, 0, 74, 13}},
    {'%', {80, 0, 90, 13}},
    {'&', {96, 0, 108, 13}},
    {'\'', {112, 0, 118, 13}},
    {'(', {128, 0, 135, 13}},
    {')', {144, 0, 150, 13}},
    {'*', {160, 0, 173, 13}},
    {'+', {176, 0, 192, 13}},
    {',', {192, 0, 195, 13}},
    {'-', {208, 0, 218, 13}},
    {'.', {224, 0, 228, 13}},
    {'/', {240, 0, 252, 13}},
    {'0', {0, 13, 10, 26}},
    {'1', {16, 13, 22, 26}},
    {'2', {32, 13, 42, 26}},
    {'3', {48, 13, 58, 26}},
    {'4', {64, 13, 74, 26}},
    {'5', {80, 13, 90, 26}},
    {'6', {96, 13, 106, 26}},
    {'7', {112, 13, 122, 26}},
    {'8', {128, 13, 138, 26}},
    {'9', {144, 13, 154, 26}},
    {':', {160, 13, 164, 26}},
    {';', {176, 13, 180, 26}},
    {'<', {192, 13, 202, 26}},
    {'=', {208, 13, 218, 26}},
    {'>', {224, 13, 234, 26}},
    {'?', {240, 13, 250, 26}},
    {'@', {0, 26, 10, 39}},
    {'A', {16, 26, 26, 39}},
    {'B', {32, 26, 42, 39}},
    {'C', {48, 26, 58, 39}},
    {'D', {64, 26, 74, 39}},
    {'E', {80, 26, 90, 39}},
    {'F', {96, 26, 106, 39}},
    {'G', {112, 26, 122, 39}},
    {'H', {128, 26, 138, 39}},
    {'I', {144, 26, 148, 39}},
    {'J', {160, 26, 170, 39}},
    {'K', {176, 26, 187, 39}},
    {'L', {192, 26, 202, 39}},
    {'M', {208, 26, 220, 39}},
    {'N', {224, 26, 234, 39}},
    {'O', {240, 26, 250, 39}},
    {'P', {0, 39, 10, 52}},
    {'Q', {16, 39, 26, 52}},
    {'R', {32, 39, 42, 52}},
    {'S', {48, 39, 58, 52}},
    {'T', {64, 39, 74, 52}},
    {'U', {80, 39, 90, 52}},
    {'V', {96, 39, 106, 52}},
    {'W', {112, 39, 124, 52}},
    {'X', {128, 39, 138, 52}},
    {'Y', {144, 39, 154, 52}},
    {'Z', {160, 39, 170, 52}},
    {'[', {176, 39, 182, 52}},
    {'\\', {192, 39, 204, 52}},
    {']', {208, 39, 214, 52}},
    {'^', {224, 39, 236, 52}},
    {'_', {240, 39, 256, 52}},
    {'`', {0, 52, 5, 65}},
    {'a', {16, 52, 26, 65}},
    {'b', {32, 52, 42, 65}},
    {'c', {48, 52, 58, 65}},
    {'d', {64, 52, 74, 65}},
    {'e', {80, 52, 90, 65}},
    {'f', {96, 52, 104, 65}},
    {'g', {112, 52, 122, 65}},
    {'h', {128, 52, 138, 65}},
    {'i', {144, 52, 149, 65}},
    {'j', {160, 52, 166, 65}},
    {'k', {176, 52, 186, 65}},
    {'l', {192, 52, 196, 65}},
    {'m', {208, 52, 220, 65}},
    {'n', {224, 52, 234, 65}},
    {'o', {240, 52, 250, 65}},
    {'p', {0, 65, 10, 78}},
    {'q', {16, 65, 26, 78}},
    {'r', {32, 65, 42, 78}},
    {'s', {48, 65, 58, 78}},
    {'t', {64, 65, 74, 78}},
    {'u', {80, 65, 90, 78}},
    {'v', {96, 65, 106, 78}},
    {'w', {112, 65, 124, 78}},
    {'x', {128, 65, 138, 78}},
    {'y', {144, 65, 154, 78}},
    {'z', {160, 65, 170, 78}},
    {'{', {176, 65, 184, 78}},
    {'|', {192, 65, 196, 78}},
    {'}', {208, 65, 214, 78}},
    {'~', {224, 65, 240, 78}},

};

unsigned long vgaCeilingWL6[] =
{
// Floors
//    1      2      3      4      5      6      7      8      9      10 
    0x1d1d,0x1d1d,0x1d1d,0x1d1d,0x1d1d,0x1d1d,0x1d1d,0x1d1d,0x1d1d,0xbfbf, // episode 1
    0x4e4e,0x4e4e,0x4e4e,0x1d1d,0x8d8d,0x4e4e,0x1d1d,0x2d2d,0x1d1d,0x8d8d, // episode 2
    0x1d1d,0x1d1d,0x1d1d,0x1d1d,0x1d1d,0x2d2d,0xdddd,0x1d1d,0x1d1d,0x9898, // episode 3

    0x1d1d,0x9d9d,0x2d2d,0xdddd,0xdddd,0x9d9d,0x2d2d,0x4d4d,0x1d1d,0xdddd, // episode 4
    0x7d7d,0x1d1d,0x2d2d,0x2d2d,0xdddd,0xd7d7,0x1d1d,0x1d1d,0x1d1d,0x2d2d, // episode 5
    0x1d1d,0x1d1d,0x1d1d,0x1d1d,0xdddd,0xdddd,0x7d7d,0xdddd,0xdddd,0xdddd  // episode 6
};

// 256 palette for Wolf3D graphics
w3dRGB Wl6Pal[256] =
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0xA8, 0x00, 0xA8, 0x00, 0x00, 0xA8, 0xA8, 0xA8, 0x00, 0x00, 0xA8,
    0x00, 0xA8, 0xA8, 0x54, 0x00, 0xA8, 0xA8, 0xA8, 0x54, 0x54, 0x54, 0x54, 0x54, 0xFC, 0x54, 0xFC,
    0x54, 0x54, 0xFC, 0xFC, 0xFC, 0x54, 0x54, 0xFC, 0x54, 0xFC, 0xFC, 0xFC, 0x54, 0xFC, 0xFC, 0xFC,
    0xEC, 0xEC, 0xEC, 0xDC, 0xDC, 0xDC, 0xD0, 0xD0, 0xD0, 0xC0, 0xC0, 0xC0, 0xB4, 0xB4, 0xB4, 0xA8,
    0xA8, 0xA8, 0x98, 0x98, 0x98, 0x8C, 0x8C, 0x8C, 0x7C, 0x7C, 0x7C, 0x70, 0x70, 0x70, 0x64, 0x64,
    0x64, 0x54, 0x54, 0x54, 0x48, 0x48, 0x48, 0x38, 0x38, 0x38, 0x2C, 0x2C, 0x2C, 0x20, 0x20, 0x20,
    0xFC, 0x00, 0x00, 0xEC, 0x00, 0x00, 0xE0, 0x00, 0x00, 0xD4, 0x00, 0x00, 0xC8, 0x00, 0x00, 0xBC,
    0x00, 0x00, 0xB0, 0x00, 0x00, 0xA4, 0x00, 0x00, 0x98, 0x00, 0x00, 0x88, 0x00, 0x00, 0x7C, 0x00,
    0x00, 0x70, 0x00, 0x00, 0x64, 0x00, 0x00, 0x58, 0x00, 0x00, 0x4C, 0x00, 0x00, 0x40, 0x00, 0x00,
    0xFC, 0xD8, 0xD8, 0xFC, 0xB8, 0xB8, 0xFC, 0x9C, 0x9C, 0xFC, 0x7C, 0x7C, 0xFC, 0x5C, 0x5C, 0xFC,
    0x40, 0x40, 0xFC, 0x20, 0x20, 0xFC, 0x00, 0x00, 0xFC, 0xA8, 0x5C, 0xFC, 0x98, 0x40, 0xFC, 0x88,
    0x20, 0xFC, 0x78, 0x00, 0xE4, 0x6C, 0x00, 0xCC, 0x60, 0x00, 0xB4, 0x54, 0x00, 0x9C, 0x4C, 0x00,
    0xFC, 0xFC, 0xD8, 0xFC, 0xFC, 0xB8, 0xFC, 0xFC, 0x9C, 0xFC, 0xFC, 0x7C, 0xFC, 0xF8, 0x5C, 0xFC,
    0xF4, 0x40, 0xFC, 0xF4, 0x20, 0xFC, 0xF4, 0x00, 0xE4, 0xD8, 0x00, 0xCC, 0xC4, 0x00, 0xB4, 0xAC,
    0x00, 0x9C, 0x9C, 0x00, 0x84, 0x84, 0x00, 0x70, 0x6C, 0x00, 0x58, 0x54, 0x00, 0x40, 0x40, 0x00,
    0xD0, 0xFC, 0x5C, 0xC4, 0xFC, 0x40, 0xB4, 0xFC, 0x20, 0xA0, 0xFC, 0x00, 0x90, 0xE4, 0x00, 0x80,
    0xCC, 0x00, 0x74, 0xB4, 0x00, 0x60, 0x9C, 0x00, 0xD8, 0xFC, 0xD8, 0xBC, 0xFC, 0xB8, 0x9C, 0xFC,
    0x9C, 0x80, 0xFC, 0x7C, 0x60, 0xFC, 0x5C, 0x40, 0xFC, 0x40, 0x20, 0xFC, 0x20, 0x00, 0xFC, 0x00,
    0x00, 0xFC, 0x00, 0x00, 0xEC, 0x00, 0x00, 0xE0, 0x00, 0x00, 0xD4, 0x00, 0x04, 0xC8, 0x00, 0x04,
    0xBC, 0x00, 0x04, 0xB0, 0x00, 0x04, 0xA4, 0x00, 0x04, 0x98, 0x00, 0x04, 0x88, 0x00, 0x04, 0x7C,
    0x00, 0x04, 0x70, 0x00, 0x04, 0x64, 0x00, 0x04, 0x58, 0x00, 0x04, 0x4C, 0x00, 0x04, 0x40, 0x00,
    0xD8, 0xFC, 0xFC, 0xB8, 0xFC, 0xFC, 0x9C, 0xFC, 0xFC, 0x7C, 0xFC, 0xF8, 0x5C, 0xFC, 0xFC, 0x40,
    0xFC, 0xFC, 0x20, 0xFC, 0xFC, 0x00, 0xFC, 0xFC, 0x00, 0xE4, 0xE4, 0x00, 0xCC, 0xCC, 0x00, 0xB4,
    0xB4, 0x00, 0x9C, 0x9C, 0x00, 0x84, 0x84, 0x00, 0x70, 0x70, 0x00, 0x58, 0x58, 0x00, 0x40, 0x40,
    0x5C, 0xBC, 0xFC, 0x40, 0xB0, 0xFC, 0x20, 0xA8, 0xFC, 0x00, 0x9C, 0xFC, 0x00, 0x8C, 0xE4, 0x00,
    0x7C, 0xCC, 0x00, 0x6C, 0xB4, 0x00, 0x5C, 0x9C, 0xD8, 0xD8, 0xFC, 0xB8, 0xBC, 0xFC, 0x9C, 0x9C,
    0xFC, 0x7C, 0x80, 0xFC, 0x5C, 0x60, 0xFC, 0x40, 0x40, 0xFC, 0x20, 0x24, 0xFC, 0x00, 0x04, 0xFC,
    0x00, 0x00, 0xFC, 0x00, 0x00, 0xEC, 0x00, 0x00, 0xE0, 0x00, 0x00, 0xD4, 0x00, 0x00, 0xC8, 0x00,
    0x00, 0xBC, 0x00, 0x00, 0xB0, 0x00, 0x00, 0xA4, 0x00, 0x00, 0x98, 0x00, 0x00, 0x88, 0x00, 0x00,
    0x7C, 0x00, 0x00, 0x70, 0x00, 0x00, 0x64, 0x00, 0x00, 0x58, 0x00, 0x00, 0x4C, 0x00, 0x00, 0x40,
    0x28, 0x28, 0x28, 0xFC, 0xE0, 0x34, 0xFC, 0xD4, 0x24, 0xFC, 0xCC, 0x18, 0xFC, 0xC0, 0x08, 0xFC,
    0xB4, 0x00, 0xB4, 0x20, 0xFC, 0xA8, 0x00, 0xFC, 0x98, 0x00, 0xE4, 0x80, 0x00, 0xCC, 0x74, 0x00,
    0xB4, 0x60, 0x00, 0x9C, 0x50, 0x00, 0x84, 0x44, 0x00, 0x70, 0x34, 0x00, 0x58, 0x28, 0x00, 0x40,
    0xFC, 0xD8, 0xFC, 0xFC, 0xB8, 0xFC, 0xFC, 0x9C, 0xFC, 0xFC, 0x7C, 0xFC, 0xFC, 0x5C, 0xFC, 0xFC,
    0x40, 0xFC, 0xFC, 0x20, 0xFC, 0xFC, 0x00, 0xFC, 0xE0, 0x00, 0xE4, 0xC8, 0x00, 0xCC, 0xB4, 0x00,
    0xB4, 0x9C, 0x00, 0x9C, 0x84, 0x00, 0x84, 0x6C, 0x00, 0x70, 0x58, 0x00, 0x58, 0x40, 0x00, 0x40,
    0xFC, 0xE8, 0xDC, 0xFC, 0xE0, 0xD0, 0xFC, 0xD8, 0xC4, 0xFC, 0xD4, 0xBC, 0xFC, 0xCC, 0xB0, 0xFC,
    0xC4, 0xA4, 0xFC, 0xBC, 0x9C, 0xFC, 0xB8, 0x90, 0xFC, 0xB0, 0x80, 0xFC, 0xA4, 0x70, 0xFC, 0x9C,
    0x60, 0xF0, 0x94, 0x5C, 0xE8, 0x8C, 0x58, 0xDC, 0x88, 0x54, 0xD0, 0x80, 0x50, 0xC8, 0x7C, 0x4C,
    0xBC, 0x78, 0x48, 0xB4, 0x70, 0x44, 0xA8, 0x68, 0x40, 0xA0, 0x64, 0x3C, 0x9C, 0x60, 0x38, 0x90,
    0x5C, 0x34, 0x88, 0x58, 0x30, 0x80, 0x50, 0x2C, 0x74, 0x4C, 0x28, 0x6C, 0x48, 0x24, 0x5C, 0x40,
    0x20, 0x54, 0x3C, 0x1C, 0x48, 0x38, 0x18, 0x40, 0x30, 0x18, 0x38, 0x2C, 0x14, 0x28, 0x20, 0x0C,
    0x60, 0x00, 0x64, 0x00, 0x64, 0x64, 0x00, 0x60, 0x60, 0x00, 0x00, 0x1C, 0x00, 0x00, 0x2C, 0x30,
    0x24, 0x10, 0x48, 0x00, 0x48, 0x50, 0x00, 0x50, 0x00, 0x00, 0x34, 0x1C, 0x1C, 0x1C, 0x4C, 0x4C,
    0x4C, 0x5C, 0x5C, 0x5C, 0x40, 0x40, 0x40, 0x30, 0x30, 0x30, 0x34, 0x34, 0x34, 0xD8, 0xF4, 0xF4,
    0xB8, 0xE8, 0xE8, 0x9C, 0xDC, 0xDC, 0x74, 0xC8, 0xC8, 0x48, 0xC0, 0xC0, 0x20, 0xB4, 0xB4, 0x20,
    0xB0, 0xB0, 0x00, 0xA4, 0xA4, 0x00, 0x98, 0x98, 0x00, 0x8C, 0x8C, 0x00, 0x84, 0x84, 0x00, 0x7C,
    0x7C, 0x00, 0x78, 0x78, 0x00, 0x74, 0x74, 0x00, 0x70, 0x70, 0x00, 0x6C, 0x6C, 0x98, 0x00, 0x88
};

VOID RectFromCharacter(char ch, RECT* rect)
{
    INT i = 0;
    RECT r = {0,0,1,1};
    
    *rect = r;

    for (i = 0; i < ARRAYSIZE(w3dFont);i++)
    {
        if (w3dFont[i].ch == ch)
        {
            *rect = w3dFont[i].rect;
            return;
        }
    }
}

VOID GetCeilingColor(BYTE levelNumber, BYTE* ceiling, BYTE* floor)
{
    *ceiling = 29; // 56,56,56
    *floor = 26; // 112,112,112
}

VOID GetPalette(W3DCOLOR palette[])
{
    USHORT i = 0;
    for (; i < ARRAYSIZE(Wl6Pal); ++i)
    {
        palette[i].r = Wl6Pal[i].r;
        palette[i].g = Wl6Pal[i].g;
        palette[i].b = Wl6Pal[i].b;
    }
}

void CarmackExpand(unsigned short* source, unsigned short* dest, unsigned short length)
{
    unsigned short ch       = 0;
    unsigned short chhigh   = 0;
    unsigned short count    = 0;
    unsigned short offset   = 0;
    unsigned short* copyptr = NULL;
    unsigned short* inptr   = NULL;
    unsigned short* outptr  = NULL;

    length /= 2; 
    inptr  = source; 
    outptr = dest;

    while(length)
    {
        ch = *inptr++;
        chhigh = ch >> 8;
        if(chhigh == CARMACK_EXPAND_NEARTAG)
        {
            count = ch & 0xff;
            if(!count)
            {
                // have to insert a word containing the tag byte
                unsigned char* p = (unsigned char *)inptr;
                ch |= *p;
                p++;
                inptr = (unsigned short*)p;
                //ch |= *((unsigned char *)inptr)++;
                *outptr++ = ch;
                length--;
            }
            else
            {
                unsigned char* p = (unsigned char *)inptr;
                offset = *p;
                p++;
                inptr = (unsigned short*)p;

                //offset= *((unsigned char *)inptr)++;
                copyptr = outptr - offset;
                length -= count;
                while(count--)
                {
                    *outptr++ = *copyptr++;
                }
            }
        }
        else if(chhigh == CARMACK_EXPAND_FARTAG)
        {
            count = ch & 0xff;
            if (!count)
            {
                // have to insert a word containing the tag byte
                unsigned char* p = (unsigned char *)inptr;
                ch |= *p;
                p++;
                inptr = (unsigned short*)p;
                //ch |= *((unsigned char *)inptr)++;
                *outptr++ = ch;
                length--;
            }
            else
            {
                offset = *inptr++;
                copyptr = dest + offset;
                length -= count;
                while(count--)
                {
                    *outptr++ = *copyptr++;
                }
            }
        }
        else
        {
            *outptr++ = ch;
            length--;
        }
    }
}

void RLEWExpand(unsigned short* source, unsigned short* dest, long length, unsigned short rlewtag)
{
    unsigned short* end = NULL;
    unsigned short value = 0;
    unsigned short count = 0;
    int n = 0;

    end = dest + (length) / 2;
    do
    {
        value = *source++;
        if(value != rlewtag)
        {
            // uncompressed
            *dest++ = value;
        }
        else 
        {
            // compressed
            count = *source++;
            value = *source++;
            for(n=1; n <= count; n++)
            {
                *dest++ = value;
            }
        }
    } while(dest < end);

}

void HuffExpand(unsigned char *source, unsigned char *dest, long length, long c_len, huffnode32 *hufftable)
{
    huffnode32* headptr = NULL;
    huffnode32* node = NULL;
    unsigned char* end = NULL;
    unsigned char ch = 0;
    unsigned char cl = 0;

    headptr = &hufftable[254];
    end = dest + length;
    node = headptr;
    ch = *source;
    source++;
    c_len--;
    cl = 1;

    do
    {
        if(ch & cl)
        {
            node = (huffnode32*)node->bit1;
        }
        else
        {
            node = (huffnode32*)node->bit0;
        }

        if((int)node<256)
        {
            *dest = (unsigned char)((int)node);
            dest++;
            node = headptr;
        }

        cl <<= 1;

        if(cl == 0)
        {
            c_len--;
            if(c_len < 0)
            {
                return;
            }

            ch = *source;
            source++;
            cl = 1;
        }
    } while(dest != end);
}

BOOL GetLayerData(BYTE levelNumber, USHORT** ppLayer)
{
    return GetLayerDataEx(1, levelNumber, ppLayer);
}

BOOL GetLayerDataEx(BYTE layerId, BYTE levelNumber, USHORT** ppLayer)
{
    BOOL success = FALSE;
    FILE* mapHeaderFile = NULL;
    FILE* mapFile = NULL;

    unsigned char i = 0;
    unsigned short fileMarker = 0;
    unsigned int mapOffset = 0;

    w3dLevel level;

    unsigned char* levelRawData = NULL;
    unsigned short levelExpandedDataSize = 0;
    unsigned char* levelExpandedData = NULL;
    unsigned short* levelData = NULL;
    long levelDataSize = 64 * 64 * sizeof(unsigned short);

    BYTE x = 0;
    BYTE y = 0;
    unsigned short cellIndex = 0;
    unsigned short cell = 0;
    unsigned char* cellData = NULL;

    unsigned long layerDataSize = 0;
    unsigned long layerDataOffset = 0;

    mapHeaderFile = fopen(mapheadFilePath, "rb");
    if(!mapHeaderFile)
    {
        printf("failed to open map header file\n");
        goto Exit;
    }

    mapFile = fopen(gamemapsFilePath, "rb");
    if (!mapFile)
    {
        printf("failed to open map file\n");
        goto Exit;
    }

    fread(&fileMarker, sizeof(fileMarker), 1, mapHeaderFile);
    if (fileMarker != MAPHEAD_FILE_MAGICNUMBER)
    {
        printf("invalid map header file\n");
        goto Exit;
    }

    for (i = 0; i < levelNumber; i++)
    {
        mapOffset = 0;
        fread(&mapOffset, sizeof(mapOffset), 1, mapHeaderFile);
    }

    fseek(mapFile, mapOffset, SEEK_SET);
    fread(&level, sizeof(level), 1, mapFile);

    switch(layerId)
    {
    case 1:
        layerDataSize = level.Layer1SizeBytes;
        layerDataOffset = level.Layer1Offset;
        break;
    case 2:
        layerDataSize = level.Layer2SizeBytes;
        layerDataOffset = level.Layer2Offset;
        break;
    case 3:
        layerDataSize = level.Layer3SizeBytes;
        layerDataOffset = level.Layer2Offset;
        break;
    default:
        break;
    }

    levelRawData = (unsigned char*)malloc(layerDataSize);
    if(levelRawData == NULL)
    {
        printf("Failed to allocate raw memory buffer for map data\n");
        goto Exit;
    }

    fseek(mapFile, layerDataOffset, SEEK_SET);

    fread(levelRawData, layerDataSize, 1, mapFile);

    levelExpandedDataSize = *((unsigned short*)levelRawData);
    levelExpandedData = (unsigned char*)malloc(levelExpandedDataSize);
    if (levelExpandedData == NULL)
    {
        printf("Failed to allocate expanded memory buffer for map data\n");
        goto Exit;
    }

    ZeroMemory(levelExpandedData,levelExpandedDataSize);

    // expand raw data into expanded buffer
    CarmackExpand(((unsigned short*)levelRawData) + 1, (unsigned short*)levelExpandedData, levelExpandedDataSize);

    // RLE expanded data into final map buffer
    levelData = (unsigned short*)malloc(levelDataSize);
    if (levelData == NULL)
    {
        printf("Failed to allocate final memory buffer for map data\n");
        goto Exit;
    }

    ZeroMemory(levelData, levelDataSize);
    RLEWExpand(((unsigned short*)levelExpandedData) + 1, levelData, levelDataSize, fileMarker);

    if (layerId == 1)
    {
        // walls

        cellData = (unsigned char*)malloc(64 * 64);

        for(x = 0; x < 64; x++)
        {
            for(y = 0; y < 64; y++)
            {
                //cellData[cellIndex] = (cell - 1) * 2 + 1;
                cell = levelData[cellIndex++];
            
                if (cell == 0)
                {
                    // unknown area
                }

                if (cell < 0x6a)
                {
                    // solid map object
                    if((cell >= 0x5A && cell <= 0x5F) || cell == 0x64 || cell == 0x65 )
                    {
                        // door
                        printf("Door Texture Number = %d\n", (cell - 1) * 2 + 1);

                        //cellData[cellIndex - 1] = (cell - 1) * 2 + 1;
                    }
                    else
                    {
                        // wall
                        printf("Texture Number = %d\n", (cell - 1) * 2 + 1);

                        //cellData[cellIndex - 1] = 10;//(cell - 1) * 2 + 1;

                        if (cell == 0x15)
                        {
                            // elevator
                        }
                    }
                }
                else if (cell == 0x6a)
                {
                    // ambush floor trigger
                }
                else if (cell >= 0x6b && cell < (0x6b + 37 /* num areas */))
                {
                    if (cell == 0x6b)
                    {
                        // secret level
                    }
                }
                else
                {
                    // unknown area
                }
            }
        }
    }
    else if (layerId == 2)
    {
        // objects
        for(x = 0; x < 64; x++)
        {
            for(y = 0; y < 64; y++)
            {
                cell = levelData[cellIndex++];
                printf("[x,y] = 0x%x\n",x,y,cell);
            }
        }
    }

Exit:

    if (cellData)
    {
        free(cellData);
    }

    if (levelData)
    {
        *ppLayer = levelData;
    }

    if (levelExpandedData)
    {
        free(levelExpandedData);
    }

    if (levelRawData)
    {
        free(levelRawData);
    }

    if (mapHeaderFile)
    {
        fclose(mapHeaderFile);
    }

    if (mapFile)
    {
        fclose(mapFile);
    }

    return success;
}

void InitializeHuffmanLookupTable(huffnode* inNodes, huffnode32* outNodes)
{
    int i = 0;
    for(i=0; i < 255; i++)
    {
        if(inNodes[i].bit0 >= 256)
        {
            outNodes[i].bit0 = (unsigned)(outNodes + (inNodes[i].bit0 - 256));
        }
        else
        {
            outNodes[i].bit0 = inNodes[i].bit0;
        }

        if(inNodes[i].bit1 >= 256)
        {
            outNodes[i].bit1 = (unsigned)(outNodes + (inNodes[i].bit1 - 256));
        }
        else
        {
            outNodes[i].bit1 = inNodes[i].bit1;
        }
    }
}

ULONG CalculateGraphicsChunkSizeBytes(size_t index, int* vgaheaderOffsets, int totalOffsets)
{
    ULONG chunkSize = 0;

    if (index < totalOffsets)
    {
        int offset = vgaheaderOffsets[index];
        int nextOffset = index + 1;
        while (vgaheaderOffsets[nextOffset] == -1)
        {
            nextOffset++;
            if (nextOffset >= totalOffsets)
            {
                printf("Extended past last valid chunk in graphics offsets!\n");
                return 0;
            }
        }
        chunkSize = vgaheaderOffsets[nextOffset] - offset;
    }

    return chunkSize;
}

BOOL ReadGraphicsData(size_t index, int* vgaheaderOffsets, int totalOffsets, huffnode32 *hufftable, unsigned char** data, ULONG* dataSize)
{
    ULONG cbCompressedChunk = CalculateGraphicsChunkSizeBytes(index, vgaheaderOffsets, totalOffsets);
    unsigned char* compressedData = NULL;
    FILE* graphicsFile = NULL;
    int* cbData = NULL;
    long dataLength = 0;
    *data = NULL;

    if (cbCompressedChunk == 0)
    {
        printf("0 sized chunk calculated for data at index '%d'\n",index);
        return FALSE;
    }

    compressedData = (unsigned char*)malloc(cbCompressedChunk);

    graphicsFile = fopen(vgagraphFilePath, "rb");
    fseek(graphicsFile, vgaheaderOffsets[index], SEEK_SET);
    fread(compressedData, cbCompressedChunk, 1, graphicsFile);

    if (index == 0)
    {
        // compressed data size is not stored in the data, so it is assumed to be
        // num offsets * 4;
        *data = (unsigned char*)malloc(totalOffsets * 4);
        dataLength = (totalOffsets * 4);
    }
    else
    {
        cbData = (int*)compressedData;
        *data = (unsigned char*)malloc(*cbData);
        dataLength = *cbData;
    }

    HuffExpand(compressedData + 4, *data, dataLength, cbCompressedChunk, hufftable);

    *dataSize = dataLength;

    if(graphicsFile)
    {
        fclose(graphicsFile);
    }

    return TRUE;
}

BOOL GetUIBitmap(int id, BITMAPFILEHEADER** ppBitmap)
{
    BOOL success = FALSE;
    FILE* vgaDictionaryFile = NULL;
    FILE* vgaHeaderFile = NULL;
    
    huffnode huffnodes[256] = {0};
    huffnode32 huffnodes32[256] = {0};

    unsigned char offsetData[3] = {0};
    int offset = -1;
    int totalOffsets = 0;
    size_t moreOffsets = 0;

    int graphicsOffsets[1024] = {0};

    ULONG cbCompressedChunk = 0;

    ULONG dataSize = 0;
    unsigned char* imageSizeData = NULL;
    unsigned char* imageData = NULL;
    unsigned char* imageBitmapData = NULL;

    int n = 0;

    w3dPicture* pPictureEntry = NULL;

    BYTE pictureCount = 0;

    int hw = 0;
    int quarter = 0;

    size_t i = 0;

    vgaDictionaryFile = fopen(vgadictFilePath, "rb");
    if(!vgaDictionaryFile)
    {
        printf("failed to open vga graphics dictionary file\n");
        goto Exit;
    }

    fread(huffnodes, sizeof(huffnodes), 1, vgaDictionaryFile);

    InitializeHuffmanLookupTable(huffnodes, huffnodes32);

    vgaHeaderFile = fopen(vgaheadFilePath, "rb");
    if(!vgaHeaderFile)
    {
        printf("failed to open vga header file\n");
        goto Exit;
    }

    do
    {
        moreOffsets = fread(offsetData, sizeof(offsetData), 1, vgaHeaderFile);
        if (moreOffsets > 0)
        {
            offset = offsetData[0] + (offsetData[1] << 8) + (offsetData[2] << 16);
            if(offset == 0xFFFFFF)
            {
                offset = -1;
            }

            graphicsOffsets[totalOffsets++] = offset;
        }
    }while(moreOffsets);

    // Read 0 chunk to obtain image sizes
    ReadGraphicsData(0, graphicsOffsets, totalOffsets, huffnodes32, &imageSizeData, &dataSize);

    pPictureEntry = (w3dPicture*)imageSizeData;
    for (i = 0; i < dataSize/4; i ++)//= sizeof(w3dPicture))
    {
        if (id == pictureCount)
        {
            printf("Picture [%d] (%d x %d)\n", id, pPictureEntry->width, pPictureEntry->height);
            break;
        }
        pPictureEntry++;
        pictureCount++;
    }

    // pPictureEntry contains the width and height of my image
    if ((pPictureEntry->width != 0) && (pPictureEntry->width <= 320) && (pPictureEntry->height != 0)  && (pPictureEntry->height <= 200))
    {
        ReadGraphicsData((id + 3), graphicsOffsets, totalOffsets, huffnodes32, &imageData ,&dataSize);
        imageBitmapData = (unsigned char*)malloc(dataSize);
        hw = pPictureEntry->width * pPictureEntry->height;
        quarter = hw / 4;

        for(n = 0; n < hw; n++)
        {
            imageBitmapData[n] = imageData[(n % 4) * quarter + n / 4];
        }

        SaveBitmap(NULL, pPictureEntry->width, pPictureEntry->height, imageBitmapData, dataSize, ppBitmap);

        free(imageBitmapData);
    }

Exit:

    if (imageSizeData)
    {
        free(imageSizeData);
    }

    if (vgaDictionaryFile)
    {
        fclose(vgaDictionaryFile);
    }

    if (vgaHeaderFile)
    {
        fclose(vgaHeaderFile);
    }

    return success;
}

BOOL GetSpriteBitmap(int id, BITMAPFILEHEADER** ppBitmap)
{
    BOOL success = FALSE;

    FILE* vswapFile = NULL;
    w3dVSwapHeader VSwapHeader = {0};
    w3dVSwapChunk* chunks = NULL;
    int chunkIndex = 0;
    unsigned char* spriteData = NULL;
    unsigned char* spriteFinalData = NULL;
    w3dSpriteData* pSprite = NULL;
    int x = 0;
    int y = 0;
    unsigned char tmp[64 * 64];
    unsigned short* pOffsets = NULL;
    unsigned short sx = 0;

    int i = 0;

    short* linecmds = NULL;

    vswapFile = fopen(vswapFilePath, "rb");
    if(!vswapFile)
    {
        printf("failed to open vswap file\n");
        goto Exit;
    }

    fread(&VSwapHeader, sizeof(VSwapHeader), 1, vswapFile);
    chunks = (w3dVSwapChunk*)malloc(VSwapHeader.NumChunks * sizeof(w3dVSwapChunk));

    for(chunkIndex = 0; chunkIndex < VSwapHeader.NumChunks; chunkIndex++)
    {
        fread(&chunks[chunkIndex].offset, sizeof(chunks[chunkIndex].offset), 1, vswapFile);
    }

    for(chunkIndex = 0; chunkIndex < VSwapHeader.NumChunks; chunkIndex++)
    {
        fread(&chunks[chunkIndex].length, sizeof(chunks[chunkIndex].length), 1, vswapFile);
    }

    // Seek to sprite
    fseek(vswapFile, chunks[id].offset, SEEK_SET);
    spriteData = (unsigned char*)malloc(chunks[id].length);
    fread(spriteData, chunks[id].length, 1, vswapFile);

    if (id >= VSwapHeader.SoundsOffset)
    {
        // sound data section
    }
    else if ((id >= VSwapHeader.SpritesOffset) && (id < VSwapHeader.SoundsOffset) )
    {
        // sprite data section
        pSprite = (w3dSpriteData*)spriteData;
        memset(tmp, 255, sizeof(tmp));
                
        // walk the sprite raw data
        pOffsets = pSprite->offsets;
        
        for (sx = pSprite->leftpix; sx <= pSprite->rightpix; sx++)
        {
            linecmds=(short *)(spriteData + *pOffsets++);
            for(; *linecmds; linecmds += 3)
            {
                i=linecmds[2]/2+linecmds[1];
                for(y=linecmds[2]/2; y<linecmds[0]/2; y++, i++)
                {
                    //tmp[y*64+sx]=spriteData[i];
                    tmp[sx*64+y]=spriteData[i];
                }
            }
        }

        spriteFinalData = (unsigned char*)malloc(64 * 64);

        for(x = 0; x < 64; x++)
        {
            for(y = 0; y < 64; y++)
            {
                spriteFinalData[(( y << 6 ) + x)] = tmp[( x << 6 ) + y];
            }
        }

        SaveBitmap(NULL, 64, 64, spriteFinalData, 64 * 64, ppBitmap);
    }
    else
    {
        // wall and other misc section
        spriteFinalData = (unsigned char*)malloc(chunks[id].length);

        for(x = 0; x < 64; x++)
        {
            for(y = 0; y < 64; y++)
            {
                spriteFinalData[(( y << 6 ) + x)] = spriteData[( x << 6 ) + y];
            }
        }

        SaveBitmap(NULL, 64, 64, spriteFinalData, chunks[id].length, ppBitmap); 
    }

Exit:

    if (spriteFinalData)
    {
        free(spriteFinalData);
    }

    if (spriteData)
    {
        free(spriteData);
    }

    if (vswapFile)
    {
        fclose(vswapFile);
    }

    return success;
}

void SaveBitmap(char* fileName, long width, long height, unsigned char* pData, long dataSize, BITMAPFILEHEADER** ppbitmap)
{
    BITMAPFILEHEADER bitmapFileHeader = {0};
    BITMAPINFOHEADER bitmapInfoHeader = {0};
    int p = 0;
    FILE* bmp = NULL;
    long b = 0;
    BYTE* pTemp = NULL;
    BYTE* pBitmapFile = NULL;

    if (fileName)
    {
        bmp = fopen(fileName, "wb");
    }

    // define the bitmap file header 
    bitmapFileHeader.bfSize      = sizeof(BITMAPFILEHEADER);
    bitmapFileHeader.bfType      = 0x4D42;
    bitmapFileHeader.bfReserved1 = 0;
    bitmapFileHeader.bfReserved2 = 0;
    bitmapFileHeader.bfOffBits   = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (ARRAYSIZE(Wl6Pal) * sizeof(RGBQUAD));
 
    if (bmp)
    {
        fwrite(&bitmapFileHeader, sizeof(bitmapFileHeader), 1, bmp);
    }

    // define the bitmap information header 
    bitmapInfoHeader.biSize          = sizeof(BITMAPINFOHEADER);
    bitmapInfoHeader.biPlanes        = 1;
    bitmapInfoHeader.biBitCount      = 8;
    bitmapInfoHeader.biCompression   = BI_RGB;
    bitmapInfoHeader.biSizeImage     = width * height;
    bitmapInfoHeader.biXPelsPerMeter = 0; 
    bitmapInfoHeader.biYPelsPerMeter = 0;
    bitmapInfoHeader.biClrUsed       = 0;
    bitmapInfoHeader.biClrImportant  = 0;
    bitmapInfoHeader.biWidth         = width;
    bitmapInfoHeader.biHeight        = -height;

    if (bmp)
    {
        fwrite(&bitmapInfoHeader, sizeof(bitmapInfoHeader), 1, bmp);
    }

    if (ppbitmap)
    {
        pBitmapFile = (BYTE*)malloc(sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (ARRAYSIZE(Wl6Pal) * sizeof(RGBQUAD)) + dataSize);
        pTemp = pBitmapFile;
        
        memcpy(pTemp, &bitmapFileHeader, sizeof(bitmapFileHeader));
        pTemp+= sizeof(bitmapFileHeader);

        memcpy(pTemp, &bitmapInfoHeader, sizeof(bitmapInfoHeader));
        pTemp+= sizeof(bitmapInfoHeader);
    }

    for(p = 0; p < ARRAYSIZE(Wl6Pal); p++)
    {
        if (bmp)
        {
            // Write RGBQUAD
            fwrite(&Wl6Pal[p].b, sizeof(Wl6Pal[p].b), 1, bmp);     // RGB
            fwrite(&Wl6Pal[p].g, sizeof(Wl6Pal[p].g), 1, bmp);     // RGB
            fwrite(&Wl6Pal[p].r, sizeof(Wl6Pal[p].r), 1, bmp);     // RGB
            fwrite(&Wl6Pal[0].r, sizeof(Wl6Pal[0].r), 1, bmp); // Reserved
        }

        if (pTemp)
        {
            *pTemp = Wl6Pal[p].b; pTemp++;
            *pTemp = Wl6Pal[p].g; pTemp++;
            *pTemp = Wl6Pal[p].r; pTemp++;
            *pTemp = Wl6Pal[p].r; pTemp++;
        }
    }

    if (bmp)
    {
        fwrite(pData, dataSize, 1, bmp);
        fclose(bmp);
    }

    if (pTemp)
    {
        memcpy(pTemp, pData, dataSize);
        *ppbitmap = (BITMAPFILEHEADER*)pBitmapFile;
    }
}