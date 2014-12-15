#include "stdafx.h"

uint16_t MAX_WALL_SPRITES         = 98;
uint16_t MAPHEAD_FILE_MAGICNUMBER = 0xABCD;
uint16_t CARMACK_EXPAND_NEARTAG   = 0xA7;
uint16_t CARMACK_EXPAND_FARTAG    = 0xA8;
uint16_t EXPANDED_MAP_SIZE        = 64 * 64;

#define WALL_DOOR_NS              90
#define WALL_DOOR_WE              91
#define WALL_DOOR_GOLDKEY_NS      92
#define WALL_DOOR_GOLDKEY_WE      93
#define WALL_DOOR_SILVERKEY_NS    94
#define WALL_DOOR_SILVERKEY_WE    95
#define WALL_DOOR_ELEVATOR_NS    100
#define WALL_DOOR_ELEVATOR_WE    101

// Begin items layer defines

#define PLAYER_START_N      19
#define PLAYER_START_E      20
#define PLAYER_START_S      21
#define PLAYER_START_W      22

#define PATROL_DIR_CHANGE_EAST         90
#define PATROL_DIR_CHANGE_NORTHEAST    91
#define PATROL_DIR_CHANGE_NORTH        92
#define PATROL_DIR_CHANGE_NORTHWEST    93
#define PATROL_DIR_CHANGE_WEST         94
#define PATROL_DIR_CHANGE_SOUTHWEST    95
#define PATROL_DIR_CHANGE_SOUTH        96
#define PATROL_DIR_CHANGE_SOUTHEAST    97

#define PUSHWALL_SECRET                98
#define ENDGAME_TRIGGER                99

#define BOSS_EPISODE_1              214

#define ENEMY_GUARD_DEAD     124

#define EASY_STANDING_GUARD_E       108
#define EASY_STANDING_GUARD_N       109
#define EASY_STANDING_GUARD_W       110
#define EASY_STANDING_GUARD_S       111
#define EASY_PATROL_GUARD_E         112
#define EASY_PATROL_GUARD_N         113
#define EASY_PATROL_GUARD_W         114
#define EASY_PATROL_GUARD_S         115

#define MEDIUM_STANDING_GUARD_E     144
#define MEDIUM_STANDING_GUARD_N     145
#define MEDIUM_STANDING_GUARD_W     146
#define MEDIUM_STANDING_GUARD_S     147
#define MEDIUM_PATROL_GUARD_E       148
#define MEDIUM_PATROL_GUARD_N       149
#define MEDIUM_PATROL_GUARD_W       150
#define MEDIUM_PATROL_GUARD_S       151

#define HARD_STANDING_GUARD_E       180
#define HARD_STANDING_GUARD_N       181
#define HARD_STANDING_GUARD_W       182
#define HARD_STANDING_GUARD_S       183
#define HARD_PATROL_GUARD_E         184
#define HARD_PATROL_GUARD_N         185
#define HARD_PATROL_GUARD_W         186
#define HARD_PATROL_GUARD_S         187

#define EASY_STANDING_DOG_N       206
#define EASY_STANDING_DOG_E       207
#define EASY_STANDING_DOG_S       208
#define EASY_STANDING_DOG_W       209
#define EASY_PATROL_DOG_N         210
#define EASY_PATROL_DOG_E         211
#define EASY_PATROL_DOG_S         212
#define EASY_PATROL_DOG_W         213

#define MEDIUM_STANDING_DOG_N     170
#define MEDIUM_STANDING_DOG_E     171
#define MEDIUM_STANDING_DOG_S     172
#define MEDIUM_STANDING_DOG_W     173
#define MEDIUM_PATROL_DOG_N       174
#define MEDIUM_PATROL_DOG_E       175
#define MEDIUM_PATROL_DOG_S       176
#define MEDIUM_PATROL_DOG_W       177

#define HARD_STANDING_DOG_N       134
#define HARD_STANDING_DOG_E       135
#define HARD_STANDING_DOG_S       136
#define HARD_STANDING_DOG_W       137
#define HARD_PATROL_DOG_N         138
#define HARD_PATROL_DOG_E         139
#define HARD_PATROL_DOG_S         140
#define HARD_PATROL_DOG_W         141

#define EASY_STANDING_SS_N       198
#define EASY_STANDING_SS_E       199
#define EASY_STANDING_SS_S       200
#define EASY_STANDING_SS_W       201
#define EASY_PATROL_SS_N         202
#define EASY_PATROL_SS_E         203
#define EASY_PATROL_SS_S         204
#define EASY_PATROL_SS_W         205

#define MEDIUM_STANDING_SS_N     162
#define MEDIUM_STANDING_SS_E     163
#define MEDIUM_STANDING_SS_S     164
#define MEDIUM_STANDING_SS_W     165
#define MEDIUM_PATROL_SS_N       166
#define MEDIUM_PATROL_SS_E       167
#define MEDIUM_PATROL_SS_S       168
#define MEDIUM_PATROL_SS_W       169

#define HARD_STANDING_SS_N       126
#define HARD_STANDING_SS_E       127
#define HARD_STANDING_SS_S       128
#define HARD_STANDING_SS_W       129
#define HARD_PATROL_SS_N         130
#define HARD_PATROL_SS_E         131
#define HARD_PATROL_SS_S         132
#define HARD_PATROL_SS_W         133

#define EASY_STANDING_ZOMBIE_N       216
#define EASY_STANDING_ZOMBIE_E       217
#define EASY_STANDING_ZOMBIE_S       218
#define EASY_STANDING_ZOMBIE_W       219
#define EASY_PATROL_ZOMBIE_N         220
#define EASY_PATROL_ZOMBIE_E         221
#define EASY_PATROL_ZOMBIE_S         222
#define EASY_PATROL_ZOMBIE_W         223

#define MEDIUM_STANDING_ZOMBIE_N     234
#define MEDIUM_STANDING_ZOMBIE_E     235
#define MEDIUM_STANDING_ZOMBIE_S     236
#define MEDIUM_STANDING_ZOMBIE_W     237
#define MEDIUM_PATROL_ZOMBIE_N       238
#define MEDIUM_PATROL_ZOMBIE_E       239
#define MEDIUM_PATROL_ZOMBIE_S       240
#define MEDIUM_PATROL_ZOMBIE_W       241

#define HARD_STANDING_ZOMBIE_N       252
#define HARD_STANDING_ZOMBIE_E       253
#define HARD_STANDING_ZOMBIE_S       254
#define HARD_STANDING_ZOMBIE_W       255
#define HARD_PATROL_ZOMBIE_N         256
#define HARD_PATROL_ZOMBIE_E         257
#define HARD_PATROL_ZOMBIE_S         258
#define HARD_PATROL_ZOMBIE_W         259

#define EASY_STANDING_OFFICER_N       116
#define EASY_STANDING_OFFICER_E       117
#define EASY_STANDING_OFFICER_S       118
#define EASY_STANDING_OFFICER_W       119
#define EASY_PATROL_OFFICER_N         120
#define EASY_PATROL_OFFICER_E         121
#define EASY_PATROL_OFFICER_S         122
#define EASY_PATROL_OFFICER_W         123

#define MEDIUM_STANDING_OFFICER_N     152
#define MEDIUM_STANDING_OFFICER_E     153
#define MEDIUM_STANDING_OFFICER_S     154
#define MEDIUM_STANDING_OFFICER_W     155
#define MEDIUM_PATROL_OFFICER_N       156
#define MEDIUM_PATROL_OFFICER_E       157
#define MEDIUM_PATROL_OFFICER_S       158
#define MEDIUM_PATROL_OFFICER_W       159

#define HARD_STANDING_OFFICER_N       188
#define HARD_STANDING_OFFICER_E       189
#define HARD_STANDING_OFFICER_S       190
#define HARD_STANDING_OFFICER_W       191
#define HARD_PATROL_OFFICER_N         192
#define HARD_PATROL_OFFICER_E         193
#define HARD_PATROL_OFFICER_S         194
#define HARD_PATROL_OFFICER_W         195

#define OBJECT_NONE             0
#define OBJECT_DEMO             21
#define OBJECT_DEATHCAM         22
#define OBJECT_WATER            23
#define OBJECT_GREENBARREL      24
#define OBJECT_TABLE_AND_CHAIRS 25
#define OBJECT_GREENLAMP        26
#define OBJECT_CHANDALIER       27
#define OBJECT_HANGING_SKELETON 28
#define OBJECT_DOGFOOD          29
#define OBJECT_PILLER           30
#define OBJECT_TREEPLANT        31
#define OBJECT_FLOORSKELETON    32
#define OBJECT_SINK             33
#define OBJECT_POTTEDPLANT      34
#define OBJECT_BLUEVACE         35
#define OBJECT_TABLE            36
#define OBJECT_GREENCEILINGLIGHT 37
#define OBJECT_POTSANDPANS      38
#define OBJECT_KNIGHT           39
#define OBJECT_CAGEEMPTY        40
#define OBJECT_CAGESKELETON     41
#define OBJECT_BONEPILES        42
#define OBJECT_GOLDKEY          43
#define OBJECT_SILVERKEY        44
#define OBJECT_BED              45
#define OBJECT_DOGFOOD2         46
#define OBJECT_TVDINNER         47
#define OBJECT_HEALTHKIT        48
#define OBJECT_AMMO             49
#define OBJECT_MACHINEGUN       50
#define OBJECT_CHAINGUN         51
#define OBJECT_CROSS            52
#define OBJECT_CUP              53
#define OBJECT_CHEST            54
#define OBJECT_CROWN            55
#define OBJECT_LIFEPOWERUP      56
#define OBJECT_BLOODYBONES      57
#define OBJECT_BROWNBARREL      58
#define OBJECT_WELLFULL         59
#define OBJECT_WELLEMPTY        60
#define OBJECT_BLOOD            61
#define OBJECT_FLAG             62
#define OBJECT_CALLARDWARF      63
#define OBJECT_MOREBONES        64
#define OBJECT_EVENMOREBONES    65
#define OBJECT_SOMEBONES        66
#define OBJECT_MOREPOTSANDPANS  67
#define OBJECT_WOODSTOVE        68
#define OBJECT_SPEARS           69
#define OBJECT_VINES            70

// End Items Layer defines

#define OBJECT_DIR_N 0
#define OBJECT_DIR_E 1
#define OBJECT_DIR_S 2
#define OBJECT_DIR_W 3

unsigned int g_mapDataOffsets[60];

void CarmackExpand(uint16_t* source, uint16_t* dest, uint16_t length);
void RLEWExpand(uint16_t* source, uint16_t* dest, long length, uint16_t rlewtag);
void HuffExpand(uint8_t *source, uint8_t *dest, long length, long c_len, w3d_huffnode32_t *hufftable);

bool GetLayerDataEx(uint8_t layerId, uint8_t levelNumber, uint16_t** ppLayer);

// 256 palette for Wolf3D graphics
w3d_paletteentry_t Wl6Pal[256] =
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

void CarmackExpand(uint16_t* source, uint16_t* dest, uint16_t length)
{
    uint16_t ch       = 0;
    uint16_t chhigh   = 0;
    uint16_t count    = 0;
    uint16_t offset   = 0;
    uint16_t* copyptr = NULL;
    uint16_t* inptr   = NULL;
    uint16_t* outptr  = NULL;

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
                uint8_t* p = (uint8_t *)inptr;
                ch |= *p;
                p++;
                inptr = (uint16_t*)p;
                //ch |= *((uint8_t *)inptr)++;
                *outptr++ = ch;
                length--;
            }
            else
            {
                uint8_t* p = (uint8_t *)inptr;
                offset = *p;
                p++;
                inptr = (uint16_t*)p;

                //offset= *((uint8_t *)inptr)++;
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
                uint8_t* p = (uint8_t *)inptr;
                ch |= *p;
                p++;
                inptr = (uint16_t*)p;
                //ch |= *((uint8_t *)inptr)++;
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

void RLEWExpand(uint16_t* source, uint16_t* dest, long length, uint16_t rlewtag)
{
    uint16_t* end = NULL;
    uint16_t value = 0;
    uint16_t count = 0;
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

void HuffExpand(uint8_t *source, uint8_t *dest, long length, long c_len, w3d_huffnode32_t *hufftable)
{
    w3d_huffnode32_t* headptr = NULL;
    w3d_huffnode32_t* node = NULL;
    uint8_t* end = NULL;
    uint8_t ch = 0;
    uint8_t cl = 0;

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
            node = (w3d_huffnode32_t*)node->bit1;
        }
        else
        {
            node = (w3d_huffnode32_t*)node->bit0;
        }

        if((int)node<256)
        {
            *dest = (uint8_t)((int)node);
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

void InitializeHuffmanLookupTable(w3d_huffnode_t* inNodes, w3d_huffnode32_t* outNodes)
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

uint32_t CalculateGraphicsChunkSizeBytes(size_t index, int* vgaheaderOffsets, int totalOffsets)
{
    uint32_t chunkSize = 0;

    if (index < totalOffsets)
    {
        int offset = vgaheaderOffsets[index];
        int nextOffset = (int)index + 1;
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

bool ReadGraphicsData(size_t index, int* vgaheaderOffsets, int totalOffsets, w3d_huffnode32_t *hufftable, uint8_t** data, uint32_t* dataSize)
{
    uint32_t cbCompressedChunk = CalculateGraphicsChunkSizeBytes(index, vgaheaderOffsets, totalOffsets);
    uint8_t* compressedData = NULL;
    FILE* graphicsFile = NULL;
    int* cbData = NULL;
    long dataLength = 0;
    *data = NULL;

    if (cbCompressedChunk == 0)
    {
        printf("0 sized chunk calculated for data at index '%u'\n", static_cast<uint32_t>(index));
        return false;
    }

    compressedData = (uint8_t*)malloc(cbCompressedChunk);

    fopen_s(&graphicsFile,vgagraphFilePath, "rb");
    if (!graphicsFile)
    {
        printf("failed to open file: %s\n", vgagraphFilePath);
        return false;
    }
    fseek(graphicsFile, vgaheaderOffsets[index], SEEK_SET);
    fread(compressedData, cbCompressedChunk, 1, graphicsFile);

    if (index == 0)
    {
        // compressed data size is not stored in the data, so it is assumed to be
        // num offsets * 4;
        *data = (uint8_t*)malloc(totalOffsets * 4);
        dataLength = (totalOffsets * 4);
    }
    else
    {
        cbData = (int*)compressedData;
        *data = (uint8_t*)malloc(*cbData);
        dataLength = *cbData;
    }

    HuffExpand(compressedData + 4, *data, dataLength, cbCompressedChunk, hufftable);

    *dataSize = dataLength;

    if(graphicsFile)
    {
        fclose(graphicsFile);
    }

    return true;
}

bool GetUIBitmap(int id, bmp_fileheader_t** ppBitmap)
{
    UNREFERENCED_PARAMETER(ppBitmap);

    bool success = false;
    FILE* vgaDictionaryFile = NULL;
    FILE* vgaHeaderFile = NULL;
    
    w3d_huffnode_t huffnodes[256] = {0};
    w3d_huffnode32_t huffnodes32[256] = {0};

    uint8_t offsetData[3] = {0};
    int offset = -1;
    int totalOffsets = 0;
    size_t moreOffsets = 0;

    int graphicsOffsets[1024] = {0};

    uint32_t dataSize = 0;
    uint8_t* imageSizeData = NULL;
    uint8_t* imageData = NULL;
    uint8_t* imageBitmapData = NULL;

    int n = 0;

    w3d_picture_t* pPictureEntry = NULL;

    uint8_t pictureCount = 0;

    int hw = 0;
    int quarter = 0;

    size_t i = 0;

    fopen_s(&vgaDictionaryFile, vgadictFilePath, "rb");
    if(!vgaDictionaryFile)
    {
        printf("failed to open vga graphics dictionary file\n");
        goto Exit;
    }

    fread(huffnodes, sizeof(huffnodes), 1, vgaDictionaryFile);

    InitializeHuffmanLookupTable(huffnodes, huffnodes32);

    fopen_s(&vgaHeaderFile, vgaheadFilePath, "rb");
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

    pPictureEntry = (w3d_picture_t*)imageSizeData;
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
        imageBitmapData = (uint8_t*)malloc(dataSize);
        hw = pPictureEntry->width * pPictureEntry->height;
        quarter = hw / 4;

        for(n = 0; n < hw; n++)
        {
            imageBitmapData[n] = imageData[(n % 4) * quarter + n / 4];
        }

        //SaveBitmap(NULL, pPictureEntry->width, pPictureEntry->height, imageBitmapData, dataSize, ppBitmap);

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

void ReadVSwapChunkList(_In_ std::vector<uint8_t>& vswap, _In_ const w3d_VSwapChunkType_t& chunkType, _Inout_ std::vector<w3d_VSwapChunkInfo_t>& chunks)
{
    w3d_VSwapHeader_t* fileHeader = (w3d_VSwapHeader_t*)vswap.data();
    uint8_t* data = vswap.data();
    std::vector<uint8_t> chunksBuffer;
    chunksBuffer.resize(fileHeader->numChunks * sizeof(w3d_VSwapChunk_t));

    // Skip header
    data += sizeof(w3d_VSwapHeader_t);

    // Read offsets
    for (size_t i = 0; i < fileHeader->numChunks; i++)
    {
        w3d_VSwapChunkInfo_t chunk;
        chunk.chunk.length = 0;
        chunk.chunk.offset = *((uint32_t*)data);
        chunk.id = (i + 1); // Chunk ids are 1-based

        data += sizeof(chunk.chunk.offset);
        if (chunkType == allChunks)
        {
            chunks.push_back(chunk);
        }

        if (chunkType == wallsChunksOnly && i < fileHeader->spritesOffset)
        {
            chunks.push_back(chunk);
        }
        else if (chunkType == spriteChunksOnly && (i >= fileHeader->spritesOffset) && (i < fileHeader->soundsOffset))
        {
            chunks.push_back(chunk);
        }
        else if (chunkType == soundChunksOnly && ( i >= fileHeader->soundsOffset))
        {
            chunks.push_back(chunk);
        }
    }

    // Read lengths
    size_t indexToChunk = 0;
    for (size_t i = 0; i < fileHeader->numChunks; i++)
    {
        if (chunkType == allChunks)
        {
            chunks[indexToChunk++].chunk.length = *((uint16_t*)data);
        }

        if (chunkType == wallsChunksOnly && i < fileHeader->spritesOffset)
        {
            chunks[indexToChunk++].chunk.length = *((uint16_t*)data);
        }
        else if (chunkType == spriteChunksOnly && (i >= fileHeader->spritesOffset) && (i < fileHeader->soundsOffset))
        {
            chunks[indexToChunk++].chunk.length = *((uint16_t*)data);
        }
        else if (chunkType == soundChunksOnly && ( i >= fileHeader->soundsOffset))
        {
            chunks[indexToChunk++].chunk.length = *((uint16_t*)data);
        }

        data += sizeof(chunks[0].chunk.length);
    }

}

void ReadVSwapChunkToBuffer(_In_ std::vector<uint8_t>& vswap, _In_ const w3d_VSwapChunk_t& chunk, _Inout_ std::vector<uint8_t>& buffer)
{
    buffer.resize(chunk.length);
    memcpy(buffer.data(), vswap.data() + chunk.offset, chunk.length);
}

bool ConvertWL6WallTo32bitBMP(_In_ std::vector<uint8_t>& wall, _Inout_ std::vector<uint8_t>& bmp)
{
    std::vector<uint8_t> tempData;
    tempData.resize(wall.size());

    // WL6 Walls are 64x64 8-bit images
    for(uint8_t x = 0; x < 64; x++)
    {
        for(uint8_t y = 0; y < 64; y++)
        {
            tempData.data()[(( y << 6 ) + x)] = wall.data()[( x << 6 ) + y];
        }
    }

    bmp_infoheader_t bmih;
    bmp_fileheader_t bmfh;
    uint8_t* walbits = tempData.data();

    // bitmap file header
    bmfh.size      = sizeof(bmp_fileheader_t);
    bmfh.type      = BitmapFileType;
    bmfh.reserved1 = 0;
    bmfh.reserved2 = 0;
    bmfh.offBits   = bmfh.size + sizeof(bmih);

    // bitmap info header
    bmih.size           = sizeof(bmp_infoheader_t);
    bmih.width          = 64;
    bmih.height         = 64;
    bmih.planes         = 1;
    bmih.bitCount       = 32;
    bmih.compression    = 0;
    bmih.sizeImage      = bmih.width * bmih.height * 4;
    bmih.xPelsPerMeter  = 0;
    bmih.yPelsPerMeter  = 0;
    bmih.clrUsed        = 0;
    bmih.clrImportant   = 0;

    // contruct a full bmp file in memory as the output
    bmp.resize(bmfh.offBits + bmih.sizeImage);

    uint8_t* copyData = bmp.data();
    memset(copyData, 0, bmp.size());
    memcpy(copyData, (void*)&bmfh, sizeof(bmfh)); copyData += sizeof(bmfh);
    memcpy(copyData, (void*)&bmih, sizeof(bmih)); copyData += sizeof(bmih);

    for (uint32_t i = 0; i < (uint32_t)(bmih.width * bmih.height);i++)
    {
        bmp_paletteentry_t palEntry;
        memset(&palEntry, 0, sizeof(palEntry));

        palEntry.red = Wl6Pal[walbits[i]].r;
        palEntry.green = Wl6Pal[walbits[i]].g;
        palEntry.blue = Wl6Pal[walbits[i]].b;
        palEntry.reserved = 255;

        memcpy(copyData, &palEntry, sizeof(palEntry));
        copyData += sizeof(palEntry);
    }

    return true;
}

bool ConvertWL6SpriteTo32bitBMP(_In_ std::vector<uint8_t>& sprite, _Inout_ std::vector<uint8_t>& bmp)
{
    uint8_t tmp[64 * 64];
    w3d_spriteData_t* spriteHeader = (w3d_spriteData_t*)sprite.data();
    uint8_t* spriteData = sprite.data();
    uint16_t* pOffsets = spriteHeader->offsets;
    memset(tmp, 255, sizeof(tmp));

    for (uint16_t sx = spriteHeader->leftpix; sx <= spriteHeader->rightpix; sx++)
    {
        short* linecmds=(short *)(spriteData + *pOffsets++);
        for(; *linecmds; linecmds += 3)
        {
            int i=linecmds[2]/2+linecmds[1];
            for(int y=linecmds[2]/2; y<linecmds[0]/2; y++, i++)
            {
                //tmp[y*64+sx]=spriteData[i];
                tmp[sx*64+y]=spriteData[i];
            }
        }
    }
    
    std::vector<uint8_t> tempData;
    tempData.resize(sizeof(tmp));

    // WL6 Sprites are 64x64 8-bit images
    for(uint8_t x = 0; x < 64; x++)
    {
        for(uint8_t y = 0; y < 64; y++)
        {
            tempData.data()[(( y << 6 ) + x)] = tmp[( x << 6 ) + y];
        }
    }

    bmp_infoheader_t bmih;
    bmp_fileheader_t bmfh;
    uint8_t* walbits = tempData.data();

    // bitmap file header
    bmfh.size      = sizeof(bmp_fileheader_t);
    bmfh.type      = BitmapFileType;
    bmfh.reserved1 = 0;
    bmfh.reserved2 = 0;
    bmfh.offBits   = bmfh.size + sizeof(bmih);

    // bitmap info header
    bmih.size           = sizeof(bmp_infoheader_t);
    bmih.width          = 64;
    bmih.height         = 64;
    bmih.planes         = 1;
    bmih.bitCount       = 32;
    bmih.compression    = 0;
    bmih.sizeImage      = bmih.width * bmih.height * 4;
    bmih.xPelsPerMeter  = 0;
    bmih.yPelsPerMeter  = 0;
    bmih.clrUsed        = 0;
    bmih.clrImportant   = 0;

    // contruct a full bmp file in memory as the output
    bmp.resize(bmfh.offBits + bmih.sizeImage);

    uint8_t* copyData = bmp.data();
    memset(copyData, 0, bmp.size());
    memcpy(copyData, (void*)&bmfh, sizeof(bmfh)); copyData += sizeof(bmfh);
    memcpy(copyData, (void*)&bmih, sizeof(bmih)); copyData += sizeof(bmih);

    for (uint32_t i = 0; i < (uint32_t)(bmih.width * bmih.height);i++)
    {
        bmp_paletteentry_t palEntry;
        memset(&palEntry, 0, sizeof(palEntry));

        palEntry.red   = Wl6Pal[walbits[i]].r;
        palEntry.green = Wl6Pal[walbits[i]].g;
        palEntry.blue  = Wl6Pal[walbits[i]].b;
        palEntry.reserved = 255;

        if (walbits[i] == 255)
        {
            palEntry.red   = 0;
            palEntry.green = 0;
            palEntry.blue  = 0;
            palEntry.reserved = 0;
        }

        memcpy(copyData, &palEntry, sizeof(palEntry));
        copyData += sizeof(palEntry);
    }

    return true;
}

void ReadW3DLevelList(_In_ std::vector<uint8_t>& maphead,  _In_ std::vector<uint8_t>& maps, _In_ std::vector<w3d_level_t>& levels)
{
    w3d_mapfileHeader_t* fileHeader = (w3d_mapfileHeader_t*)maphead.data();
    uint32_t* offsets = (uint32_t*)(maphead.data() + sizeof(w3d_mapfileHeader_t));
    if (fileHeader->fileMarker != MAPHEAD_FILE_MAGICNUMBER)
    {
        return;
    }

    while(*offsets != 0)
    {
        w3d_level_t level;
        uint8_t* levelData = maps.data() + *offsets;
        level = *((w3d_level_t*)levelData);
        levels.push_back(level);
        offsets++;
    }
}

void ReadW3dLevelLayerToBuffer(_In_ std::vector<uint8_t>& maps, _In_ const w3d_level_t level, _In_ const w3d_layerType_t layerType, _In_ std::vector<uint8_t>& buffer)
{
    // Defaults to layer type walls
    uint16_t layerSizeBytes = level.layer1SizeBytes;
    uint32_t layerOffset = level.layer1Offset;

    switch(layerType)
    {
    case wallsLayer:
        layerSizeBytes = level.layer1SizeBytes;
        layerOffset = level.layer1Offset;
        break;
    case itemsLayer:
        layerSizeBytes = level.layer2SizeBytes;
        layerOffset = level.layer2Offset;
        break;
    case miscLayer:
        layerSizeBytes = level.layer3SizeBytes;
        layerOffset = level.layer3Offset;
        break;
    default:
        break;
    }

    // Read layer data
    buffer.resize(layerSizeBytes);
    uint8_t* levelData = maps.data() + layerOffset;
    memcpy(buffer.data(), levelData, buffer.size());
}

uint16_t RawWolfWallValueToId(_In_ const uint16_t i)
{
    return (i - 1) * 2 + 1;
}

bool IsWall(_In_ const uint16_t i)
{
    uint16_t id = RawWolfWallValueToId(i);
    return (id > 0 && id < MAX_WALL_SPRITES);
}

uint16_t GetCell(_In_ int16_t x, _In_ int16_t y)
{
    if (x < 0) x = 0;
    if (x >= 64) x = 64 - 1;

    if (y < 0) y = 0;
    if (y >= 64) y = 64 - 1;

    return x * 64 + y;
}

uint16_t GetCellX(_In_ const uint16_t cellIndex)
{
    return cellIndex / 64;
}

uint16_t GetCellY(_In_ const uint16_t cellIndex)
{
    return cellIndex % 64;
}

struct geometryContentVertex2_t
{
    geometryContentVertex2_t(){};
    geometryContentVertex2_t(Vector3 p, Vector3 n, Vector2 t){position = p; normal = n; texCoord = t;};
    Vector3 position;
    Vector3 normal;
    Vector2 texCoord;
};

struct geometryContentVertex_t
{
    Vector3 position;
    Vector3 normal;
    Vector2 texCoord;
};

void AddWEDoorJambVertices(uint16_t x, uint16_t y, std::vector<geometryContentVertex_t>& cubeVertices, std::vector<uint32_t>& cubeIndices, _In_ float scaleFactor)
{
    geometryContentVertex_t vertices[] = {
        {Vector3(-0.49f, 0.49f, -0.05f),  Vector3(0,0,0), Vector2(0, 0)},
        {Vector3(0.49f, 0.49f, -0.05f),   Vector3(0,0,0), Vector2(1, 0)},
        {Vector3(0.49f, -0.49f, -0.05f),  Vector3(0,0,0), Vector2(1, 1)},
        {Vector3(-0.49f, -0.49f, -0.05f), Vector3(0,0,0), Vector2(0, 1)},
        {Vector3(0.49f, 0.49f, 0.05f),    Vector3(0,0,0), Vector2(0, 0)},
        {Vector3(-0.49f, 0.49f, 0.05f),   Vector3(0,0,0), Vector2(1, 0)},
        {Vector3(-0.49f, -0.49f, 0.05f),  Vector3(0,0,0), Vector2(1, 1)},
        {Vector3(0.49f, -0.49f, 0.05f),   Vector3(0,0,0), Vector2(0, 1)},
        {Vector3(-0.49f, 0.49f, 0.05f),   Vector3(0,0,0), Vector2(0, 0)},
        {Vector3(0.49f, 0.49f, 0.05f),    Vector3(0,0,0), Vector2(1, 0)},
        {Vector3(0.49f, 0.49f, -0.05f),   Vector3(0,0,0), Vector2(1, 1)},
        {Vector3(-0.49f, 0.49f, -0.05f),  Vector3(0,0,0), Vector2(0, 1)},
        {Vector3(-0.49f, -0.49f, -0.05f), Vector3(0,0,0), Vector2(0, 0)},
        {Vector3(0.49f, -0.49f, -0.05f),  Vector3(0,0,0), Vector2(1, 0)},
        {Vector3(0.49f, -0.49f, 0.05f),   Vector3(0,0,0), Vector2(1, 1)},
        {Vector3(-0.49f, -0.49f, 0.05f),  Vector3(0,0,0), Vector2(0, 1)},
        {Vector3(-0.49f, 0.49f, 0.05f),   Vector3(0,0,0), Vector2(0, 0)},
        {Vector3(-0.49f, 0.49f, -0.05f),  Vector3(0,0,0), Vector2(1, 0)},
        {Vector3(-0.49f, -0.49f, -0.05f), Vector3(0,0,0), Vector2(1, 1)},
        {Vector3(-0.49f, -0.49f, 0.05f),  Vector3(0,0,0), Vector2(0, 1)},
        {Vector3(0.49f, 0.49f, -0.05f),   Vector3(0,0,0), Vector2(0, 0)},
        {Vector3(0.49f, 0.49f, 0.05f),    Vector3(0,0,0), Vector2(1, 0)},
        {Vector3(0.49f, -0.49f, 0.05f),   Vector3(0,0,0), Vector2(1, 1)},
        {Vector3(0.49f, -0.49f, -0.05f),  Vector3(0,0,0), Vector2(0, 1)},
    };

    uint32_t indices[] = {  0,  1,  2, 
                          0,  2,  3, 
                          4,  5,  6, 
                          4,  6,  7, 
                          8,  9, 10, 
                          8, 10, 11, 
                         12, 13, 14, 
                         12, 14, 15, 
                         16, 17, 18, 
                         16, 18, 19, 
                         20, 21, 22, 
                         20, 22, 23, 
                         };

    // copy vertices
    size_t numVertices = cubeVertices.size();
    for(size_t i = 0; i < _countof(vertices); i++)
    {
        geometryContentVertex_t grv;
        grv.position.x = ((vertices[i].position.x + x) ) * scaleFactor;
        grv.position.y = (vertices[i].position.y) * scaleFactor;
        grv.position.z = ((vertices[i].position.z + y) - .54f) * scaleFactor;
        grv.texCoord.x = vertices[i].texCoord.x;
        grv.texCoord.y = vertices[i].texCoord.y;
        cubeVertices.push_back(grv);
    }

    // copy indices
    for(size_t i = 0; i < _countof(indices); i++)
    {
        cubeIndices.push_back((uint32_t)(indices[i] + numVertices));
    }

    // copy vertices
    numVertices = cubeVertices.size();
    for(size_t i = 0; i < _countof(vertices); i++)
    {
        geometryContentVertex_t grv;
        grv.position.x = ((vertices[i].position.x + x) ) * scaleFactor;
        grv.position.y = (vertices[i].position.y) * scaleFactor;
        grv.position.z = ((vertices[i].position.z + y) + .54f) * scaleFactor;
        grv.texCoord.x = vertices[i].texCoord.x;
        grv.texCoord.y = vertices[i].texCoord.y;
        cubeVertices.push_back(grv);
    }

    // copy indices
    for(size_t i = 0; i < _countof(indices); i++)
    {
        cubeIndices.push_back((uint32_t)(indices[i] + numVertices));
    }
}

void AddSimpleCubeVertices(uint16_t x, uint16_t y, std::vector<geometryContentVertex_t>& cubeVertices, std::vector<uint32_t>& cubeIndices, _In_ float scaleFactor)
{
    geometryContentVertex_t vertices[] = {
        {Vector3(-0.5f, 0.5f, -0.5f),  Vector3(0,0,0), Vector2(0, 0)},
        {Vector3(0.5f, 0.5f, -0.5f),   Vector3(0,0,0), Vector2(1, 0)},
        {Vector3(0.5f, -0.5f, -0.5f),  Vector3(0,0,0), Vector2(1, 1)},
        {Vector3(-0.5f, -0.5f, -0.5f), Vector3(0,0,0), Vector2(0, 1)},

        {Vector3(0.5f, 0.5f, 0.5f),    Vector3(0,0,0), Vector2(0, 0)},
        {Vector3(-0.5f, 0.5f, 0.5f),   Vector3(0,0,0), Vector2(1, 0)},
        {Vector3(-0.5f, -0.5f, 0.5f),  Vector3(0,0,0), Vector2(1, 1)},
        {Vector3(0.5f, -0.5f, 0.5f),   Vector3(0,0,0), Vector2(0, 1)},
        
        // top
        {Vector3(-0.5f, 0.5f, 0.5f),   Vector3(0,0,0), Vector2(0, 0)},
        {Vector3(0.5f, 0.5f, 0.5f),    Vector3(0,0,0), Vector2(1, 0)},
        {Vector3(0.5f, 0.5f, -0.5f),   Vector3(0,0,0), Vector2(1, 1)},
        {Vector3(-0.5f, 0.5f, -0.5f),  Vector3(0,0,0), Vector2(0, 1)},

        // bottom
        {Vector3(-0.5f, -0.5f, -0.5f), Vector3(0,0,0), Vector2(0, 0)},
        {Vector3(0.5f, -0.5f, -0.5f),  Vector3(0,0,0), Vector2(1, 0)},
        {Vector3(0.5f, -0.5f, 0.5f),   Vector3(0,0,0), Vector2(1, 1)},
        {Vector3(-0.5f, -0.5f, 0.5f),  Vector3(0,0,0), Vector2(0, 1)},
        
        // side
        {Vector3(-0.5f, 0.5f, 0.5f),   Vector3(0,0,0), Vector2(0, 0)},
        {Vector3(-0.5f, 0.5f, -0.5f),  Vector3(0,0,0), Vector2(1, 0)},
        {Vector3(-0.5f, -0.5f, -0.5f), Vector3(0,0,0), Vector2(1, 1)},
        {Vector3(-0.5f, -0.5f, 0.5f),  Vector3(0,0,0), Vector2(0, 1)},
        
        // side
        {Vector3(0.5f, 0.5f, -0.5f),   Vector3(0,0,0), Vector2(0, 0)},
        {Vector3(0.5f, 0.5f, 0.5f),    Vector3(0,0,0), Vector2(1, 0)},
        {Vector3(0.5f, -0.5f, 0.5f),   Vector3(0,0,0), Vector2(1, 1)},
        {Vector3(0.5f, -0.5f, -0.5f),  Vector3(0,0,0), Vector2(0, 1)},
    };

    uint32_t indices[] = {  0,  1,  2, 
                          0,  2,  3, 
                          4,  5,  6, 
                          4,  6,  7, 
                          8,  9, 10, 
                          8, 10, 11, 
                         12, 13, 14, 
                         12, 14, 15, 
                         16, 17, 18, 
                         16, 18, 19, 
                         20, 21, 22, 
                         20, 22, 23, 
                         };

    // copy vertices
    size_t numVertices = cubeVertices.size();
    for(size_t i = 0; i < _countof(vertices); i++)
    {
        geometryContentVertex_t grv;
        grv.position.x = (vertices[i].position.x + x) * scaleFactor;
        grv.position.y = (vertices[i].position.y) * scaleFactor;
        grv.position.z = (vertices[i].position.z + y) * scaleFactor;
        grv.texCoord.x = vertices[i].texCoord.x;
        grv.texCoord.y = vertices[i].texCoord.y;
        cubeVertices.push_back(grv);
    }

    // copy indices
    for(size_t i = 0; i < _countof(indices); i++)
    {
        cubeIndices.push_back((uint32_t)(indices[i] + numVertices));
    }
}

void AddNSDoorVertices(uint16_t x, uint16_t y, std::vector<geometryContentVertex_t>& cubeVertices, std::vector<uint32_t>& cubeIndices, _In_ float scaleFactor)
{
    geometryContentVertex_t vertices[] = {
        {Vector3(-0.5f, 0.5f, -0.05f),  Vector3(0,0,0), Vector2(0, -1)},
        {Vector3(0.5f, 0.5f, -0.05f),   Vector3(0,0,0), Vector2(-1, -1)},
        {Vector3(0.5f, -0.5f, -0.05f),  Vector3(0,0,0), Vector2(-1, 0)},
        {Vector3(-0.5f, -0.5f, -0.05f), Vector3(0,0,0), Vector2(0, 0)},

        {Vector3(0.5f, 0.5f, 0.05f),    Vector3(0,0,0), Vector2(0, 0)},
        {Vector3(-0.5f, 0.5f, 0.05f),   Vector3(0,0,0), Vector2(1, 0)},
        {Vector3(-0.5f, -0.5f, 0.05f),  Vector3(0,0,0), Vector2(1, 1)},
        {Vector3(0.5f, -0.5f, 0.05f),   Vector3(0,0,0), Vector2(0, 1)},
        
        // top
        {Vector3(-0.5f, 0.5f, 0.05f),   Vector3(0,0,0), Vector2(0, 0)},
        {Vector3(0.5f, 0.5f, 0.05f),    Vector3(0,0,0), Vector2(0, 0)},
        {Vector3(0.5f, 0.5f, -0.05f),   Vector3(0,0,0), Vector2(0, 0)},
        {Vector3(-0.5f, 0.5f, -0.05f),  Vector3(0,0,0), Vector2(0, 0)},

        // bottom
        {Vector3(-0.5f, -0.5f, -0.05f), Vector3(0,0,0), Vector2(0, 0)},
        {Vector3(0.5f, -0.5f, -0.05f),  Vector3(0,0,0), Vector2(0, 0)},
        {Vector3(0.5f, -0.5f, 0.05f),   Vector3(0,0,0), Vector2(0, 0)},
        {Vector3(-0.5f, -0.5f, 0.05f),  Vector3(0,0,0), Vector2(0, 0)},
        
        // side
        {Vector3(-0.5f, 0.5f, 0.05f),   Vector3(0,0,0), Vector2(0, 0)},
        {Vector3(-0.5f, 0.5f, -0.05f),  Vector3(0,0,0), Vector2(0, 0)},
        {Vector3(-0.5f, -0.5f, -0.05f), Vector3(0,0,0), Vector2(0, 0)},
        {Vector3(-0.5f, -0.5f, 0.05f),  Vector3(0,0,0), Vector2(0, 0)},
        
        // side
        {Vector3(0.5f, 0.5f, -0.05f),   Vector3(0,0,0), Vector2(0, 0)},
        {Vector3(0.5f, 0.5f, 0.05f),    Vector3(0,0,0), Vector2(0, 0)},
        {Vector3(0.5f, -0.5f, 0.05f),   Vector3(0,0,0), Vector2(0, 0)},
        {Vector3(0.5f, -0.5f, -0.05f),  Vector3(0,0,0), Vector2(0, 0)},
    };

    uint32_t indices[] = {  0,  1,  2, 
                          0,  2,  3, 
                          4,  5,  6, 
                          4,  6,  7, 
                          8,  9, 10, 
                          8, 10, 11, 
                         12, 13, 14, 
                         12, 14, 15, 
                         16, 17, 18, 
                         16, 18, 19, 
                         20, 21, 22, 
                         20, 22, 23, 
                         };

    // copy vertices
    size_t numVertices = cubeVertices.size();
    for(size_t i = 0; i < _countof(vertices); i++)
    {
        geometryContentVertex_t grv;
        grv.position.x = (vertices[i].position.x + x) * scaleFactor;
        grv.position.y = (vertices[i].position.y) * scaleFactor;
        grv.position.z = (vertices[i].position.z + y) * scaleFactor;
        grv.texCoord.x = vertices[i].texCoord.x;
        grv.texCoord.y = vertices[i].texCoord.y;
        cubeVertices.push_back(grv);
    }

    // copy indices
    for(size_t i = 0; i < _countof(indices); i++)
    {
        cubeIndices.push_back((uint32_t)(indices[i] + numVertices));
    }
}

void AddWEDoorVertices(uint16_t x, uint16_t y, std::vector<geometryContentVertex_t>& cubeVertices, std::vector<uint32_t>& cubeIndices, _In_ float scaleFactor)
{
    geometryContentVertex_t vertices[] = {
        // side
        {Vector3(-0.05f, 0.5f, -0.5f),  Vector3(0,0,0), Vector2(0, 0)},
        {Vector3(0.05f, 0.5f, -0.5f),   Vector3(0,0,0), Vector2(0, 0)},
        {Vector3(0.05f, -0.5f, -0.5f),  Vector3(0,0,0), Vector2(0, 0)},
        {Vector3(-0.05f, -0.5f, -0.5f), Vector3(0,0,0), Vector2(0, 0)},

        // side
        {Vector3(0.05f, 0.5f, 0.5f),    Vector3(0,0,0), Vector2(0, 0)},
        {Vector3(-0.05f, 0.5f, 0.5f),   Vector3(0,0,0), Vector2(0, 0)},
        {Vector3(-0.05f, -0.5f, 0.5f),  Vector3(0,0,0), Vector2(0, 0)},
        {Vector3(0.05f, -0.5f, 0.5f),   Vector3(0,0,0), Vector2(0, 0)},

        // top
        {Vector3(-0.05f, 0.5f, 0.5f),   Vector3(0,0,0), Vector2(0, 0)},
        {Vector3(0.05f, 0.5f, 0.5f),    Vector3(0,0,0), Vector2(0, 0)},
        {Vector3(0.05f, 0.5f, -0.5f),   Vector3(0,0,0), Vector2(0, 0)},
        {Vector3(-0.05f, 0.5f, -0.5f),  Vector3(0,0,0), Vector2(0, 0)},

        // bottom
        {Vector3(-0.05f, -0.5f, -0.5f), Vector3(0,0,0), Vector2(0, 0)},
        {Vector3(0.05f, -0.5f, -0.5f),  Vector3(0,0,0), Vector2(0, 0)},
        {Vector3(0.05f, -0.5f, 0.5f),   Vector3(0,0,0), Vector2(0, 0)},
        {Vector3(-0.05f, -0.5f, 0.5f),  Vector3(0,0,0), Vector2(0, 0)},
        
        // back
        {Vector3(-0.05f, 0.5f, 0.5f),   Vector3(0,0,0), Vector2(0, -1)},
        {Vector3(-0.05f, 0.5f, -0.5f),  Vector3(0,0,0), Vector2(-1, -1)},
        {Vector3(-0.05f, -0.5f, -0.5f), Vector3(0,0,0), Vector2(-1, 0)},
        {Vector3(-0.05f, -0.5f, 0.5f),  Vector3(0,0,0), Vector2(0, 0)},
        
        {Vector3(0.05f, 0.5f, -0.5f),   Vector3(0,0,0), Vector2(0, 0)},
        {Vector3(0.05f, 0.5f, 0.5f),    Vector3(0,0,0), Vector2(1, 0)},
        {Vector3(0.05f, -0.5f, 0.5f),   Vector3(0,0,0), Vector2(1, 1)},
        {Vector3(0.05f, -0.5f, -0.5f),  Vector3(0,0,0), Vector2(0, 1)},
    };

    uint32_t indices[] = {  0,  1,  2, 
                          0,  2,  3, 
                          4,  5,  6, 
                          4,  6,  7, 
                          8,  9, 10, 
                          8, 10, 11, 
                         12, 13, 14, 
                         12, 14, 15, 
                         16, 17, 18, 
                         16, 18, 19, 
                         20, 21, 22, 
                         20, 22, 23, 
                         };

    // copy vertices
    size_t numVertices = cubeVertices.size();
    for(size_t i = 0; i < _countof(vertices); i++)
    {
        geometryContentVertex_t grv;
        grv.position.x = (vertices[i].position.x + x) * scaleFactor;
        grv.position.y = (vertices[i].position.y) * scaleFactor;
        grv.position.z = (vertices[i].position.z + y) * scaleFactor;
        grv.texCoord.x = vertices[i].texCoord.x;
        grv.texCoord.y = vertices[i].texCoord.y;
        cubeVertices.push_back(grv);
    }

    // copy indices
    for(size_t i = 0; i < _countof(indices); i++)
    {
        cubeIndices.push_back((uint32_t)(indices[i] + numVertices));
    }
}

void AddNSDoorJambVertices(uint16_t x, uint16_t y, std::vector<geometryContentVertex_t>& cubeVertices, std::vector<uint32_t>& cubeIndices, _In_ float scaleFactor)
{
    geometryContentVertex_t vertices[] = {
        {Vector3(-0.05f, 0.49f, -0.49f),  Vector3(0,0,0), Vector2(0, 0)},
        {Vector3(0.05f, 0.49f, -0.49f),   Vector3(0,0,0), Vector2(1, 0)},
        {Vector3(0.05f, -0.49f, -0.49f),  Vector3(0,0,0), Vector2(1, 1)},
        {Vector3(-0.05f, -0.49f, -0.49f), Vector3(0,0,0), Vector2(0, 1)},
        {Vector3(0.05f, 0.49f, 0.49f),    Vector3(0,0,0), Vector2(0, 0)},
        {Vector3(-0.05f, 0.49f, 0.49f),   Vector3(0,0,0), Vector2(1, 0)},
        {Vector3(-0.05f, -0.49f, 0.49f),  Vector3(0,0,0), Vector2(1, 1)},
        {Vector3(0.05f, -0.49f, 0.49f),   Vector3(0,0,0), Vector2(0, 1)},
        {Vector3(-0.05f, 0.49f, 0.49f),   Vector3(0,0,0), Vector2(0, 0)},
        {Vector3(0.05f, 0.49f, 0.49f),    Vector3(0,0,0), Vector2(1, 0)},
        {Vector3(0.05f, 0.49f, -0.49f),   Vector3(0,0,0), Vector2(1, 1)},
        {Vector3(-0.05f, 0.49f, -0.49f),  Vector3(0,0,0), Vector2(0, 1)},
        {Vector3(-0.05f, -0.49f, -0.49f), Vector3(0,0,0), Vector2(0, 0)},
        {Vector3(0.05f, -0.49f, -0.49f),  Vector3(0,0,0), Vector2(1, 0)},
        {Vector3(0.05f, -0.49f, 0.49f),   Vector3(0,0,0), Vector2(1, 1)},
        {Vector3(-0.05f, -0.49f, 0.49f),  Vector3(0,0,0), Vector2(0, 1)},
        {Vector3(-0.05f, 0.49f, 0.49f),   Vector3(0,0,0), Vector2(0, 0)},
        {Vector3(-0.05f, 0.49f, -0.49f),  Vector3(0,0,0), Vector2(1, 0)},
        {Vector3(-0.05f, -0.49f, -0.49f), Vector3(0,0,0), Vector2(1, 1)},
        {Vector3(-0.05f, -0.49f, 0.49f),  Vector3(0,0,0), Vector2(0, 1)},
        {Vector3(0.05f, 0.49f, -0.49f),   Vector3(0,0,0), Vector2(0, 0)},
        {Vector3(0.05f, 0.49f, 0.49f),    Vector3(0,0,0), Vector2(1, 0)},
        {Vector3(0.05f, -0.49f, 0.49f),   Vector3(0,0,0), Vector2(1, 1)},
        {Vector3(0.05f, -0.49f, -0.49f),  Vector3(0,0,0), Vector2(0, 1)},
    };

    uint32_t indices[] = {  0,  1,  2, 
                          0,  2,  3, 
                          4,  5,  6, 
                          4,  6,  7, 
                          8,  9, 10, 
                          8, 10, 11, 
                         12, 13, 14, 
                         12, 14, 15, 
                         16, 17, 18, 
                         16, 18, 19, 
                         20, 21, 22, 
                         20, 22, 23, 
                         };

    // copy vertices
    size_t numVertices = cubeVertices.size();
    for(size_t i = 0; i < _countof(vertices); i++)
    {
        geometryContentVertex_t grv;
        grv.position.x = ((vertices[i].position.x + x) + .54f) * scaleFactor;
        grv.position.y = (vertices[i].position.y) * scaleFactor;
        grv.position.z = (vertices[i].position.z + y) * scaleFactor;
        grv.texCoord.x = vertices[i].texCoord.x;
        grv.texCoord.y = vertices[i].texCoord.y;
        cubeVertices.push_back(grv);
    }

    // copy indices
    for(size_t i = 0; i < _countof(indices); i++)
    {
        cubeIndices.push_back((uint32_t)(indices[i] + numVertices));
    }

    // copy vertices
    numVertices = cubeVertices.size();
    for(size_t i = 0; i < _countof(vertices); i++)
    {
        geometryContentVertex_t grv;
        grv.position.x = ((vertices[i].position.x + x) - .54f) * scaleFactor;
        grv.position.y = (vertices[i].position.y) * scaleFactor;
        grv.position.z = (vertices[i].position.z + y) * scaleFactor;
        grv.texCoord.x = vertices[i].texCoord.x;
        grv.texCoord.y = vertices[i].texCoord.y;
        cubeVertices.push_back(grv);
    }

    // copy indices
    for(size_t i = 0; i < _countof(indices); i++)
    {
        cubeIndices.push_back((uint32_t)(indices[i] + numVertices));
    }
}

void AddCubeVertices(_In_ std::vector<uint16_t>& layerData, uint32_t cellIndex, std::vector<geometryContentVertex_t>& cubeVertices, std::vector<uint32_t>& cubeIndices, _In_ float scaleFactor)
{
    UNREFERENCED_PARAMETER(layerData);
    UNREFERENCED_PARAMETER(cellIndex);

    uint16_t x = GetCellX((uint16_t)cellIndex);
    uint16_t y = GetCellY((uint16_t)cellIndex);

    std::vector<geometryContentVertex2_t> vertices;
    std::vector<uint32_t> indices;

    uint16_t testCell = GetCell(x, y - 1);

    if (!IsWall(layerData[testCell]))
    {
        // Add wall facing (y - 1)
        vertices.push_back(geometryContentVertex2_t(Vector3(-0.5, 0.5, -0.5),  Vector3(0,0,0), Vector2(0, 0)));
        vertices.push_back(geometryContentVertex2_t(Vector3(0.5, 0.5, -0.5),   Vector3(0,0,0), Vector2(1, 0)));
        vertices.push_back(geometryContentVertex2_t(Vector3(0.5, -0.5, -0.5),  Vector3(0,0,0), Vector2(1, 1)));
        vertices.push_back(geometryContentVertex2_t(Vector3(-0.5, -0.5, -0.5), Vector3(0,0,0), Vector2(0, 1)));
        indices.push_back((uint32_t)(vertices.size() - 4)); indices.push_back((uint32_t)(vertices.size() - 3)); indices.push_back((uint32_t)(vertices.size() - 2));
        indices.push_back((uint32_t)(vertices.size() - 4)); indices.push_back((uint32_t)(vertices.size() - 2)); indices.push_back((uint32_t)(vertices.size() - 1));
    }

    testCell = GetCell(x, y + 1);

    if (!IsWall(layerData[GetCell(x, y + 1)]))
    {
        // Add wall facing (y + 1)
        vertices.push_back(geometryContentVertex2_t(Vector3(0.5, 0.5, 0.5),    Vector3(0,0,0), Vector2(0, 0)));
        vertices.push_back(geometryContentVertex2_t(Vector3(-0.5, 0.5, 0.5),   Vector3(0,0,0), Vector2(1, 0)));
        vertices.push_back(geometryContentVertex2_t(Vector3(-0.5, -0.5, 0.5),  Vector3(0,0,0), Vector2(1, 1)));
        vertices.push_back(geometryContentVertex2_t(Vector3(0.5, -0.5, 0.5),   Vector3(0,0,0), Vector2(0, 1)));
        indices.push_back((uint32_t)(vertices.size() - 4)); indices.push_back((uint32_t)(vertices.size() - 3)); indices.push_back((uint32_t)(vertices.size() - 2));
        indices.push_back((uint32_t)(vertices.size() - 4)); indices.push_back((uint32_t)(vertices.size() - 2)); indices.push_back((uint32_t)(vertices.size() - 1));
    }

    // Do not add top or bottom of vertices of the cube since Wolf3d wall geometry doesn't need it. Keep it here for future use
    // though :)
    /*
    vertices.push_back(geometryContentVertex2_t(Vector3(-0.5, 0.5, 0.5),   Vector3(0,0,0), Vector2(0, 0)));
    vertices.push_back(geometryContentVertex2_t(Vector3(0.5, 0.5, 0.5),    Vector3(0,0,0), Vector2(1, 0)));
    vertices.push_back(geometryContentVertex2_t(Vector3(0.5, 0.5, -0.5),   Vector3(0,0,0), Vector2(1, 1)));
    vertices.push_back(geometryContentVertex2_t(Vector3(-0.5, 0.5, -0.5),  Vector3(0,0,0), Vector2(0, 1)));
    indices.push_back((uint32_t)(vertices.size() - 4)); indices.push_back((uint32_t)(vertices.size() - 3)); indices.push_back((uint32_t)(vertices.size() - 2));
    indices.push_back((uint32_t)(vertices.size() - 4)); indices.push_back((uint32_t)(vertices.size() - 2)); indices.push_back((uint32_t)(vertices.size() - 1));

    vertices.push_back(geometryContentVertex2_t(Vector3(-0.5, -0.5, -0.5), Vector3(0,0,0), Vector2(0, 0)));
    vertices.push_back(geometryContentVertex2_t(Vector3(0.5, -0.5, -0.5),  Vector3(0,0,0), Vector2(1, 0)));
    vertices.push_back(geometryContentVertex2_t(Vector3(0.5, -0.5, 0.5),   Vector3(0,0,0), Vector2(1, 1)));
    vertices.push_back(geometryContentVertex2_t(Vector3(-0.5, -0.5, 0.5),  Vector3(0,0,0), Vector2(0, 1)));
    indices.push_back((uint32_t)(vertices.size() - 4)); indices.push_back((uint32_t)(vertices.size() - 3)); indices.push_back((uint32_t)(vertices.size() - 2));
    indices.push_back((uint32_t)(vertices.size() - 4)); indices.push_back((uint32_t)(vertices.size() - 2)); indices.push_back((uint32_t)(vertices.size() - 1));
    */

    testCell = GetCell(x - 1, y);

    if (!IsWall(layerData[GetCell(x - 1, y)]))
    {
        // Add wall facing (x - 1)
        vertices.push_back(geometryContentVertex2_t(Vector3(-0.5, 0.5, 0.5),   Vector3(0,0,0), Vector2(0, 0)));
        vertices.push_back(geometryContentVertex2_t(Vector3(-0.5, 0.5, -0.5),  Vector3(0,0,0), Vector2(1, 0)));
        vertices.push_back(geometryContentVertex2_t(Vector3(-0.5, -0.5, -0.5), Vector3(0,0,0), Vector2(1, 1)));
        vertices.push_back(geometryContentVertex2_t(Vector3(-0.5, -0.5, 0.5),  Vector3(0,0,0), Vector2(0, 1)));
        indices.push_back((uint32_t)(vertices.size() - 4)); indices.push_back((uint32_t)(vertices.size() - 3)); indices.push_back((uint32_t)(vertices.size() - 2));
        indices.push_back((uint32_t)(vertices.size() - 4)); indices.push_back((uint32_t)(vertices.size() - 2)); indices.push_back((uint32_t)(vertices.size() - 1));
    }

    testCell = GetCell(x + 1, y);

    if (!IsWall(layerData[GetCell(x + 1, y)]))
    {
        // Add wall facing (x + 1)
        vertices.push_back(geometryContentVertex2_t(Vector3(0.5, 0.5, -0.5),   Vector3(0,0,0), Vector2(0, 0)));
        vertices.push_back(geometryContentVertex2_t(Vector3(0.5, 0.5, 0.5),    Vector3(0,0,0), Vector2(1, 0)));
        vertices.push_back(geometryContentVertex2_t(Vector3(0.5, -0.5, 0.5),   Vector3(0,0,0), Vector2(1, 1)));
        vertices.push_back(geometryContentVertex2_t(Vector3(0.5, -0.5, -0.5),  Vector3(0,0,0), Vector2(0, 1)));
        indices.push_back((uint32_t)(vertices.size() - 4)); indices.push_back((uint32_t)(vertices.size() - 3)); indices.push_back((uint32_t)(vertices.size() - 2));
        indices.push_back((uint32_t)(vertices.size() - 4)); indices.push_back((uint32_t)(vertices.size() - 2)); indices.push_back((uint32_t)(vertices.size() - 1));
    }

    // copy vertices
    size_t numVertices = cubeVertices.size();
    for(size_t i = 0; i < vertices.size(); i++)
    {
        geometryContentVertex_t grv;
        grv.position.x = (vertices[i].position.x + x) * scaleFactor;
        grv.position.y = (vertices[i].position.y) * scaleFactor;
        grv.position.z = (vertices[i].position.z + y) * scaleFactor;
        grv.texCoord.x = vertices[i].texCoord.x;
        grv.texCoord.y = vertices[i].texCoord.y;
        cubeVertices.push_back(grv);
    }

    // copy indices
    for(size_t i = 0; i < indices.size(); i++)
    {
        cubeIndices.push_back((uint32_t)(indices[i] + numVertices));
    }
}

void AddFloorVertices(uint16_t x, uint16_t y, std::vector<geometryContentVertex_t>& cubeVertices, std::vector<uint32_t>& cubeIndices, _In_ float scaleFactor)
{
    geometryContentVertex_t vertices[] = {
        {Vector3(-32.0f, 0.05f, 32.0f),   Vector3(0,0,0), Vector2(0, 0)},
        {Vector3(32.0f, 0.05f, 32.0f),    Vector3(0,0,0), Vector2(1, 0)},
        {Vector3(32.0f, 0.05f, -32.0f),   Vector3(0,0,0), Vector2(1, 1)},
        {Vector3(-32.0f, 0.05f, -32.0f),  Vector3(0,0,0), Vector2(0, 1)},
    };

    uint32_t indices[] = {  0,  1,  2, 
                          0,  2,  3
                         };

    // copy vertices
    size_t numVertices = cubeVertices.size();
    for(size_t i = 0; i < _countof(vertices); i++)
    {
        geometryContentVertex_t grv;
        grv.position.x = ((vertices[i].position.x + x) + .5f) * scaleFactor;
        grv.position.y = ((vertices[i].position.y) - .510f) * scaleFactor;
        grv.position.z = ((vertices[i].position.z + y) + .5f) * scaleFactor;
        grv.texCoord.x = vertices[i].texCoord.x;
        grv.texCoord.y = vertices[i].texCoord.y;
        cubeVertices.push_back(grv);
    }

    // copy indices
    for(size_t i = 0; i < _countof(indices); i++)
    {
        cubeIndices.push_back((uint32_t)(indices[i] + numVertices));
    }
}

uint32_t GetMapCellX(_In_ uint32_t cellIndex)
{
    return cellIndex % 64;
}

uint32_t GetMapCellY(_In_ uint32_t cellIndex)
{
    return cellIndex / 64;
}

void GetMapCellXY(_In_ uint32_t cellIndex, _Out_ uint32_t& x, _Out_ uint32_t& y)
{
    x = GetMapCellX(cellIndex);
    y = GetMapCellY(cellIndex);
}

void AddItemEntryToListEx(_Inout_ std::vector<std::string>& itemsList, _In_ const char* name, _In_ uint16_t cellIndex, uint8_t directionFacing, _In_ float scale, _In_ uint16_t textureId )
{
    std::string itemEntry = name;
    uint32_t x,y;
    float rotation = 0.0f;
    GetMapCellXY(cellIndex, x, y);
    Vector3 pos((float)y,0,(float)x);
    pos *= scale;

    // Name
    itemEntry = name;
    itemEntry += " ";

    // location
    itemEntry += std::to_string(pos.x) + " " + std::to_string(pos.y) + " " + std::to_string(pos.z);
    itemEntry += " ";

    // rotation

    switch(directionFacing)
    {
        case EASY_STANDING_GUARD_E:
        case MEDIUM_STANDING_GUARD_E:
        case HARD_STANDING_GUARD_E:
        case EASY_PATROL_GUARD_E:
        case MEDIUM_PATROL_GUARD_E:
        case HARD_PATROL_GUARD_E:
        case PATROL_DIR_CHANGE_EAST:
        case OBJECT_DIR_E:
            rotation = 0.0f;
        break;

        case PATROL_DIR_CHANGE_SOUTHEAST:
            rotation = 0.79f;
        break;

        case EASY_STANDING_GUARD_S:
        case MEDIUM_STANDING_GUARD_S:
        case HARD_STANDING_GUARD_S:
        case EASY_PATROL_GUARD_S:
        case MEDIUM_PATROL_GUARD_S:
        case HARD_PATROL_GUARD_S:
        case PATROL_DIR_CHANGE_SOUTH:
        case OBJECT_DIR_S:
            rotation = 1.57f;
        break;

        case PATROL_DIR_CHANGE_SOUTHWEST:
            rotation = 2.36f;
        break;

        case EASY_STANDING_GUARD_W:
        case MEDIUM_STANDING_GUARD_W:
        case HARD_STANDING_GUARD_W:
        case EASY_PATROL_GUARD_W:
        case MEDIUM_PATROL_GUARD_W:
        case HARD_PATROL_GUARD_W:
        case PATROL_DIR_CHANGE_WEST:
        case OBJECT_DIR_W:
            rotation = 3.14f;
        break;

        case PATROL_DIR_CHANGE_NORTHWEST:
            rotation = 3.93f;
        break;

        case EASY_STANDING_GUARD_N:
        case MEDIUM_STANDING_GUARD_N:
        case HARD_STANDING_GUARD_N:
        case EASY_PATROL_GUARD_N:
        case MEDIUM_PATROL_GUARD_N:
        case HARD_PATROL_GUARD_N:
        case PATROL_DIR_CHANGE_NORTH:
        case OBJECT_DIR_N:
            rotation = 4.71f;
        break;

        case PATROL_DIR_CHANGE_NORTHEAST:
            rotation = 5.50f;
        break;

        default:
            rotation = 0.0f;
        break;
    }

    itemEntry += std::to_string(rotation);

    if (textureId != 0)
    {
        itemEntry += " ";
        itemEntry += std::to_string(textureId);
        itemEntry += ".texture";
    }

    itemsList.push_back(itemEntry);
}

void AddItemEntryToList(_Inout_ std::vector<std::string>& itemsList, _In_ const char* name, _In_ uint16_t cellIndex, uint8_t directionFacing, _In_ float scale)
{
    AddItemEntryToListEx(itemsList, name, cellIndex, directionFacing, scale, 0);
}

void ConvertItemsLayerToItemsList(_In_ std::vector<uint8_t>& layer, _Inout_ std::vector<std::string>& itemsList, const float scaleFactor)
{
    std::vector<uint8_t> expandedData;
    std::vector<uint16_t> layerData;
    layerData.resize(64 * 64);
    uint16_t* srcLayerData = (uint16_t*)layer.data();

    uint16_t expandedDataSize = *srcLayerData;
    expandedData.resize(expandedDataSize);
    srcLayerData++;

    CarmackExpand(srcLayerData, (uint16_t*)expandedData.data(), (uint16_t)expandedData.size());
    RLEWExpand(((uint16_t*)expandedData.data()) + 1, layerData.data(), (long)layerData.size() * sizeof(uint16_t), MAPHEAD_FILE_MAGICNUMBER);

    for(uint32_t i = 0; i < layerData.size(); i++)
    {
        std::string layerDataValue = std::to_string(layerData[i]);

        switch(layerData[i])
        {
        case PLAYER_START_N:
        case PLAYER_START_E:
        case PLAYER_START_S:
        case PLAYER_START_W:
            AddItemEntryToList(itemsList, "start", i, layerData[i] - PLAYER_START_N, scaleFactor);
        break;

        case PATROL_DIR_CHANGE_EAST:
        case PATROL_DIR_CHANGE_NORTHEAST:
        case PATROL_DIR_CHANGE_NORTH:
        case PATROL_DIR_CHANGE_NORTHWEST:
        case PATROL_DIR_CHANGE_WEST:
        case PATROL_DIR_CHANGE_SOUTHWEST:
        case PATROL_DIR_CHANGE_SOUTH:
        case PATROL_DIR_CHANGE_SOUTHEAST:
            AddItemEntryToList(itemsList, "patroldirchange", i, layerData[i], scaleFactor);
        break;

        case PUSHWALL_SECRET:
            //AddItemEntryToList(itemsList, "pushwall", i, OBJECT_DIR_E, scaleFactor);
        break;
        
        case ENDGAME_TRIGGER:
            AddItemEntryToList(itemsList, "endgame", i, OBJECT_DIR_E, scaleFactor);
        break;
        
        case BOSS_EPISODE_1:
            AddItemEntryToList(itemsList, "boss", i, OBJECT_DIR_E, scaleFactor);
        break;
        
        case ENEMY_GUARD_DEAD:
            AddItemEntryToList(itemsList, "deadguard", i, OBJECT_DIR_E, scaleFactor);
        break;

        case EASY_STANDING_GUARD_N:
        case EASY_STANDING_GUARD_E:
        case EASY_STANDING_GUARD_S:
        case EASY_STANDING_GUARD_W:
            AddItemEntryToList(itemsList, "guard", i, layerData[i], scaleFactor);
        break;

        case EASY_PATROL_GUARD_N:
        case EASY_PATROL_GUARD_E:
        case EASY_PATROL_GUARD_S:
        case EASY_PATROL_GUARD_W:
            AddItemEntryToList(itemsList, "pguard", i, layerData[i], scaleFactor);
        break;

        case MEDIUM_STANDING_GUARD_N:
        case MEDIUM_STANDING_GUARD_E:
        case MEDIUM_STANDING_GUARD_S:
        case MEDIUM_STANDING_GUARD_W:
            AddItemEntryToList(itemsList, "guard", i, layerData[i], scaleFactor);
        break;

        case MEDIUM_PATROL_GUARD_N:
        case MEDIUM_PATROL_GUARD_E:
        case MEDIUM_PATROL_GUARD_S:
        case MEDIUM_PATROL_GUARD_W:
            AddItemEntryToList(itemsList, "pguard", i, layerData[i], scaleFactor);
        break;

        case HARD_STANDING_GUARD_N:
        case HARD_STANDING_GUARD_E:
        case HARD_STANDING_GUARD_S:
        case HARD_STANDING_GUARD_W:
            AddItemEntryToList(itemsList, "guard", i, layerData[i], scaleFactor);
        break;

        case HARD_PATROL_GUARD_N:
        case HARD_PATROL_GUARD_E:
        case HARD_PATROL_GUARD_S:
        case HARD_PATROL_GUARD_W:
            AddItemEntryToList(itemsList, "pguard", i, layerData[i], scaleFactor);
        break;

        case EASY_STANDING_DOG_N:
        case EASY_STANDING_DOG_E:
        case EASY_STANDING_DOG_S:
        case EASY_STANDING_DOG_W:
            AddItemEntryToList(itemsList, "dog", i, layerData[i] - EASY_STANDING_DOG_N, scaleFactor);
        break;

        case EASY_PATROL_DOG_N:
        case EASY_PATROL_DOG_E:
        case EASY_PATROL_DOG_S:
        case EASY_PATROL_DOG_W:
            AddItemEntryToList(itemsList, "pdog", i, layerData[i] - EASY_PATROL_DOG_N, scaleFactor);
        break;

        case MEDIUM_STANDING_DOG_N:
        case MEDIUM_STANDING_DOG_E:
        case MEDIUM_STANDING_DOG_S:
        case MEDIUM_STANDING_DOG_W:
            AddItemEntryToList(itemsList, "dog", i, layerData[i] - MEDIUM_STANDING_DOG_N, scaleFactor);
        break;

        case MEDIUM_PATROL_DOG_N:
        case MEDIUM_PATROL_DOG_E:
        case MEDIUM_PATROL_DOG_S:
        case MEDIUM_PATROL_DOG_W:
            AddItemEntryToList(itemsList, "pdog", i, layerData[i] - MEDIUM_PATROL_DOG_N, scaleFactor);
        break;

        case HARD_STANDING_DOG_N:
        case HARD_STANDING_DOG_E:
        case HARD_STANDING_DOG_S:
        case HARD_STANDING_DOG_W:
            AddItemEntryToList(itemsList, "dog", i, layerData[i] - HARD_STANDING_DOG_N, scaleFactor);
        break;

        case HARD_PATROL_DOG_N:
        case HARD_PATROL_DOG_E:
        case HARD_PATROL_DOG_S:
        case HARD_PATROL_DOG_W:
            AddItemEntryToList(itemsList, "pdog", i, layerData[i] - HARD_PATROL_DOG_N, scaleFactor);
        break;

        case EASY_STANDING_SS_N:
        case EASY_STANDING_SS_E:
        case EASY_STANDING_SS_S:
        case EASY_STANDING_SS_W:
            AddItemEntryToList(itemsList, "sssoldier", i, layerData[i] - EASY_STANDING_SS_N, scaleFactor);
        break;

        case EASY_PATROL_SS_N:
        case EASY_PATROL_SS_E:
        case EASY_PATROL_SS_S:
        case EASY_PATROL_SS_W:
            AddItemEntryToList(itemsList, "psssoldier", i, layerData[i] - EASY_PATROL_SS_N, scaleFactor);
        break;

        case MEDIUM_STANDING_SS_N:
        case MEDIUM_STANDING_SS_E:
        case MEDIUM_STANDING_SS_S:
        case MEDIUM_STANDING_SS_W:
            AddItemEntryToList(itemsList, "sssoldier", i, layerData[i] - MEDIUM_STANDING_SS_N, scaleFactor);
        break;

        case MEDIUM_PATROL_SS_N:
        case MEDIUM_PATROL_SS_E:
        case MEDIUM_PATROL_SS_S:
        case MEDIUM_PATROL_SS_W:
            AddItemEntryToList(itemsList, "psssoldier", i, layerData[i] - MEDIUM_PATROL_SS_N, scaleFactor);
        break;

        case HARD_STANDING_SS_N:
        case HARD_STANDING_SS_E:
        case HARD_STANDING_SS_S:
        case HARD_STANDING_SS_W:
            AddItemEntryToList(itemsList, "sssoldier", i, layerData[i] - HARD_STANDING_SS_N, scaleFactor);
        break;

        case HARD_PATROL_SS_N:
        case HARD_PATROL_SS_E:
        case HARD_PATROL_SS_S:
        case HARD_PATROL_SS_W:
            AddItemEntryToList(itemsList, "psssoldier", i, layerData[i] - HARD_PATROL_SS_N, scaleFactor);
        break;

        case EASY_STANDING_ZOMBIE_N:
        case EASY_STANDING_ZOMBIE_E:
        case EASY_STANDING_ZOMBIE_S:
        case EASY_STANDING_ZOMBIE_W:
        break;

        case EASY_PATROL_ZOMBIE_N:
        case EASY_PATROL_ZOMBIE_E:
        case EASY_PATROL_ZOMBIE_S:
        case EASY_PATROL_ZOMBIE_W:
        break;

        case MEDIUM_STANDING_ZOMBIE_N:
        case MEDIUM_STANDING_ZOMBIE_E:
        case MEDIUM_STANDING_ZOMBIE_S:
        case MEDIUM_STANDING_ZOMBIE_W:
        break;

        case MEDIUM_PATROL_ZOMBIE_N:
        case MEDIUM_PATROL_ZOMBIE_E:
        case MEDIUM_PATROL_ZOMBIE_S:
        case MEDIUM_PATROL_ZOMBIE_W:
        break;

        case HARD_STANDING_ZOMBIE_N:
        case HARD_STANDING_ZOMBIE_E:
        case HARD_STANDING_ZOMBIE_S:
        case HARD_STANDING_ZOMBIE_W:
        break;

        case HARD_PATROL_ZOMBIE_N:
        case HARD_PATROL_ZOMBIE_E:
        case HARD_PATROL_ZOMBIE_S:
        case HARD_PATROL_ZOMBIE_W:
        break;

        case EASY_STANDING_OFFICER_N:
        case EASY_STANDING_OFFICER_E:
        case EASY_STANDING_OFFICER_S:
        case EASY_STANDING_OFFICER_W:
        break;

        case EASY_PATROL_OFFICER_N:
        case EASY_PATROL_OFFICER_E:
        case EASY_PATROL_OFFICER_S:
        case EASY_PATROL_OFFICER_W:
        break;

        case MEDIUM_STANDING_OFFICER_N:
        case MEDIUM_STANDING_OFFICER_E:
        case MEDIUM_STANDING_OFFICER_S:
        case MEDIUM_STANDING_OFFICER_W:
        break;

        case MEDIUM_PATROL_OFFICER_N:
        case MEDIUM_PATROL_OFFICER_E:
        case MEDIUM_PATROL_OFFICER_S:
        case MEDIUM_PATROL_OFFICER_W:
        break;

        case HARD_STANDING_OFFICER_N:
        case HARD_STANDING_OFFICER_E:
        case HARD_STANDING_OFFICER_S:
        case HARD_STANDING_OFFICER_W:
        break;

        case HARD_PATROL_OFFICER_N:
        case HARD_PATROL_OFFICER_E:
        case HARD_PATROL_OFFICER_S:
        case HARD_PATROL_OFFICER_W:
        break;

        case OBJECT_NONE:
        break;

        case OBJECT_WATER:
            AddItemEntryToList(itemsList, "waterpool", i, OBJECT_DIR_E, scaleFactor);
        break;
        case OBJECT_GREENBARREL:
            AddItemEntryToList(itemsList, "greenbarrel", i, OBJECT_DIR_E, scaleFactor);
        break;
        case OBJECT_TABLE_AND_CHAIRS:
            AddItemEntryToList(itemsList, "tableandchairs", i, OBJECT_DIR_E, scaleFactor);
        break;
        case OBJECT_GREENLAMP:
            AddItemEntryToList(itemsList, "greenlamp", i, OBJECT_DIR_E, scaleFactor);
        break;
        case OBJECT_CHANDALIER:
            AddItemEntryToList(itemsList, "chandilier", i, OBJECT_DIR_E, scaleFactor);
        break;
        case OBJECT_HANGING_SKELETON:
            AddItemEntryToList(itemsList, "hangingskeleton", i, OBJECT_DIR_E, scaleFactor);
        break;
        case OBJECT_DOGFOOD:
            AddItemEntryToList(itemsList, "dogfood", i, OBJECT_DIR_E, scaleFactor);
        break;
        case OBJECT_PILLER:
            AddItemEntryToList(itemsList, "pillar", i, OBJECT_DIR_E, scaleFactor);
        break;
        case OBJECT_TREEPLANT:
            AddItemEntryToList(itemsList, "treeplant", i, OBJECT_DIR_E, scaleFactor);
        break;
        case OBJECT_FLOORSKELETON:
            AddItemEntryToList(itemsList, "floorskeleton", i, OBJECT_DIR_E, scaleFactor);
        break;
        case OBJECT_SINK:
            AddItemEntryToList(itemsList, "sink", i, OBJECT_DIR_E, scaleFactor);
        break;
        case OBJECT_POTTEDPLANT:
            AddItemEntryToList(itemsList, "pottedplant", i, OBJECT_DIR_E, scaleFactor);
        break;
        case OBJECT_BLUEVACE:
            AddItemEntryToList(itemsList, "bluevace", i, OBJECT_DIR_E, scaleFactor);
        break;
        case OBJECT_TABLE:
            AddItemEntryToList(itemsList, "table", i, OBJECT_DIR_E, scaleFactor);
        break;
        case OBJECT_GREENCEILINGLIGHT:
            AddItemEntryToList(itemsList, "greenceilinglight", i, OBJECT_DIR_E, scaleFactor);
        break;
        case OBJECT_POTSANDPANS:
            AddItemEntryToList(itemsList, "potsandpans", i, OBJECT_DIR_E, scaleFactor);
        break;
        case OBJECT_KNIGHT:
            AddItemEntryToList(itemsList, "knight", i, OBJECT_DIR_E, scaleFactor);
        break;
        case OBJECT_CAGEEMPTY:
            AddItemEntryToList(itemsList, "emotycage", i, OBJECT_DIR_E, scaleFactor);
        break;
        case OBJECT_CAGESKELETON:
            AddItemEntryToList(itemsList, "skeletoncage", i, OBJECT_DIR_E, scaleFactor);
        break;
        case OBJECT_BONEPILES:
            AddItemEntryToList(itemsList, "bonepiles", i, OBJECT_DIR_E, scaleFactor);
        break;
        case OBJECT_GOLDKEY:
            AddItemEntryToList(itemsList, "goldkey", i, OBJECT_DIR_E, scaleFactor);
        break;
        case OBJECT_SILVERKEY:
            AddItemEntryToList(itemsList, "silverkey", i, OBJECT_DIR_E, scaleFactor);
        break;
        case OBJECT_BED:
            AddItemEntryToList(itemsList, "bed", i, OBJECT_DIR_E, scaleFactor);
        break;
        case OBJECT_DOGFOOD2:
            AddItemEntryToList(itemsList, "dogfood", i, OBJECT_DIR_E, scaleFactor);
        break;
        case OBJECT_TVDINNER:
            AddItemEntryToList(itemsList, "tvdinner", i, OBJECT_DIR_E, scaleFactor);
        break;
        case OBJECT_HEALTHKIT:
            AddItemEntryToList(itemsList, "healthkit", i, OBJECT_DIR_E, scaleFactor);
        break;
        case OBJECT_AMMO:
            AddItemEntryToList(itemsList, "ammo", i, OBJECT_DIR_E, scaleFactor);
        break;
        case OBJECT_MACHINEGUN:
            AddItemEntryToList(itemsList, "machinegun", i, OBJECT_DIR_E, scaleFactor);
        break;
        case OBJECT_CHAINGUN:
            AddItemEntryToList(itemsList, "chaingun", i, OBJECT_DIR_E, scaleFactor);
        break;
        case OBJECT_CROSS:
            AddItemEntryToList(itemsList, "goldcross", i, OBJECT_DIR_E, scaleFactor);
        break;
        case OBJECT_CUP:
            AddItemEntryToList(itemsList, "goldcup", i, OBJECT_DIR_E, scaleFactor);
        break;
        case OBJECT_CHEST:
            AddItemEntryToList(itemsList, "goldchest", i, OBJECT_DIR_E, scaleFactor);
        break;
        case OBJECT_CROWN:
            AddItemEntryToList(itemsList, "goldcrown", i, OBJECT_DIR_E, scaleFactor);
        break;
        case OBJECT_LIFEPOWERUP:
            AddItemEntryToList(itemsList, "lifepowerup", i, OBJECT_DIR_E, scaleFactor);
        break;
        case OBJECT_BLOODYBONES:
            AddItemEntryToList(itemsList, "bloodybones", i, OBJECT_DIR_E, scaleFactor);
        break;
        case OBJECT_BROWNBARREL:
            AddItemEntryToList(itemsList, "brownbarrel", i, OBJECT_DIR_E, scaleFactor);
        break;
        case OBJECT_WELLFULL:
            AddItemEntryToList(itemsList, "fullwell", i, OBJECT_DIR_E, scaleFactor);
        break;
        case OBJECT_WELLEMPTY:
            AddItemEntryToList(itemsList, "emptywell", i, OBJECT_DIR_E, scaleFactor);
        break;
        case OBJECT_BLOOD:
            AddItemEntryToList(itemsList, "blood", i, OBJECT_DIR_E, scaleFactor);
        break;
        case OBJECT_FLAG:
            AddItemEntryToList(itemsList, "flag", i, OBJECT_DIR_E, scaleFactor);
        break;
        case OBJECT_CALLARDWARF:
            AddItemEntryToList(itemsList, "callardwarf", i, OBJECT_DIR_E, scaleFactor);
        break;
        case OBJECT_MOREBONES:
            AddItemEntryToList(itemsList, "bones", i, OBJECT_DIR_E, scaleFactor);
        break;
        case OBJECT_EVENMOREBONES:
            AddItemEntryToList(itemsList, "bones", i, OBJECT_DIR_E, scaleFactor);
        break;
        case OBJECT_SOMEBONES:
            AddItemEntryToList(itemsList, "bones", i, OBJECT_DIR_E, scaleFactor);
        break;
        case OBJECT_MOREPOTSANDPANS:
            AddItemEntryToList(itemsList, "potsandpans", i, OBJECT_DIR_E, scaleFactor);
        break;
        case OBJECT_WOODSTOVE:
            AddItemEntryToList(itemsList, "woodstove", i, OBJECT_DIR_E, scaleFactor);
        break;
        case OBJECT_SPEARS:
            AddItemEntryToList(itemsList, "spears", i, OBJECT_DIR_E, scaleFactor);
        break;
        case OBJECT_VINES:
            AddItemEntryToList(itemsList, "vines", i, OBJECT_DIR_E, scaleFactor);
        break;
        default:
        break;

        }
    }
}

void ConvertWallsLayerToGeometryContentList(_In_ std::vector<uint8_t>& itemsLayer, _In_ std::vector<uint8_t>& layer, _Inout_ std::vector<std::wstring>& textureNameList, _Inout_ std::vector<std::shared_ptr<GDK::GeometryContent>>& contentList, _Inout_ std::vector<std::string>& itemsList, _In_ const float scaleFactor)
{
    std::vector<uint8_t> expandedData;
    std::vector<uint16_t> layerData;
    layerData.resize(64 * 64);
    uint16_t* srcLayerData = (uint16_t*)layer.data();

    uint16_t expandedDataSize = *srcLayerData;
    expandedData.resize(expandedDataSize);
    srcLayerData++;

    CarmackExpand(srcLayerData, (uint16_t*)expandedData.data(), (uint16_t)expandedData.size());
    RLEWExpand(((uint16_t*)expandedData.data()) + 1, layerData.data(), (long)layerData.size() * sizeof(uint16_t), MAPHEAD_FILE_MAGICNUMBER);

    // Expand items layer for cross parsing against walls
    std::vector<uint8_t> expandedData2;
    std::vector<uint16_t> layerData2;
    layerData2.resize(64 * 64);
    uint16_t* srcLayerData2 = (uint16_t*)itemsLayer.data();

    uint16_t expandedDataSize2 = *srcLayerData2;
    expandedData2.resize(expandedDataSize2);
    srcLayerData2++;

    CarmackExpand(srcLayerData2, (uint16_t*)expandedData2.data(), (uint16_t)expandedData2.size());
    RLEWExpand(((uint16_t*)expandedData2.data()) + 1, layerData2.data(), (long)layerData2.size() * sizeof(uint16_t), MAPHEAD_FILE_MAGICNUMBER);

    struct w3d_geometrycontentraw_t
    {
        std::wstring textureName;
        std::vector<geometryContentVertex_t> vertices;
        std::vector<uint32_t> indices;
    };

    std::map<uint16_t, w3d_geometrycontentraw_t> geometryMap;

    // For secret push walls to be properly handled, they need to be removed from the original source
    // data.  This will cause the cubes creation code to properly add the surrounding walls instead of
    // leaving holes where the closed secret door covered.
    // Secret door locations are already captured in the items list, so only the texture used to cover the walls
    // needs to be captured.  This can be done by just recording the Wall value Id.
    std::map<uint32_t, uint16_t> secretPushWallMap;

    // Clear secret walls and record their locations + texture ids
    for(uint32_t i = 0; i < layerData.size(); i++)
    {
        if (layerData2[i] == PUSHWALL_SECRET)
        {
            secretPushWallMap[i] = RawWolfWallValueToId(layerData[i]);
            layerData[i] = 0; // clear the push wall

            AddItemEntryToListEx(itemsList, "pushwall", i, OBJECT_DIR_E, scaleFactor, secretPushWallMap[i]);
        }
    }

    for(uint32_t i = 0; i < layerData.size(); i++)
    {
        uint16_t id = RawWolfWallValueToId(layerData[i]);
        if (id > 0 && id < MAX_WALL_SPRITES)
        {
            if (!geometryMap.count(id))
            {
                w3d_geometrycontentraw_t rawGeometry;
                rawGeometry.textureName = L"w3d_";
                wchar_t wallId[4] = {0};
                _itow_s((int)id, wallId, _countof(wallId), 10);
                rawGeometry.textureName.append(wallId);

                geometryMap[id] = rawGeometry;
            }

            AddCubeVertices(layerData, i, geometryMap[id].vertices, geometryMap[id].indices, scaleFactor);
        }

        // Create door jambs
        if((layerData[i] >= WALL_DOOR_NS && layerData[i] <= WALL_DOOR_SILVERKEY_WE) || layerData[i] == WALL_DOOR_ELEVATOR_NS || layerData[i] == WALL_DOOR_ELEVATOR_WE )
        {
            uint16_t doorJambTextureId = 101;

            switch(layerData[i])
            {
                case WALL_DOOR_NS:
                case WALL_DOOR_ELEVATOR_NS:
                case WALL_DOOR_GOLDKEY_NS:
                case WALL_DOOR_SILVERKEY_NS:
                    {
                        if (!geometryMap.count(doorJambTextureId)) // door jamb texture id
                        {
                            w3d_geometrycontentraw_t rawGeometry;
                            rawGeometry.textureName = L"w3d_";
                            wchar_t wallId[4] = {0};
                            _itow_s((int)doorJambTextureId, wallId, _countof(wallId), 10);
                            rawGeometry.textureName.append(wallId);

                            geometryMap[doorJambTextureId] = rawGeometry;
                        }

                        AddNSDoorJambVertices(GetCellX((uint16_t)i), GetCellY((uint16_t)i), geometryMap[doorJambTextureId].vertices, geometryMap[doorJambTextureId].indices, scaleFactor);
                    }
                    AddItemEntryToList(itemsList, "door", i, OBJECT_DIR_W, scaleFactor);
                    break;
                case WALL_DOOR_WE:
                case WALL_DOOR_ELEVATOR_WE:
                case WALL_DOOR_GOLDKEY_WE:
                case WALL_DOOR_SILVERKEY_WE:
                    {
                        if (!geometryMap.count(doorJambTextureId)) // door jamb texture id
                        {
                            w3d_geometrycontentraw_t rawGeometry;
                            rawGeometry.textureName = L"w3d_";
                            wchar_t wallId[4] = {0};
                            _itow_s((int)doorJambTextureId, wallId, _countof(wallId), 10);
                            rawGeometry.textureName.append(wallId);

                            geometryMap[doorJambTextureId] = rawGeometry;
                        }

                        AddWEDoorJambVertices(GetCellX((uint16_t)i), GetCellY((uint16_t)i), geometryMap[doorJambTextureId].vertices, geometryMap[doorJambTextureId].indices, scaleFactor);
                    }
                    AddItemEntryToList(itemsList, "door", i, OBJECT_DIR_S, scaleFactor);
                    break;
                default:
                    break;
            }
        }
    }

    // Add floor vertices
    uint16_t floorTextureId = 1000;
    if (!geometryMap.count(floorTextureId)) // door jamb texture id
    {
        w3d_geometrycontentraw_t rawGeometry;
        rawGeometry.textureName = L"w3d_";
        wchar_t wallId[5] = {0};
        _itow_s((int)floorTextureId, wallId, _countof(wallId), 10);
        rawGeometry.textureName.append(wallId);

        geometryMap[floorTextureId] = rawGeometry;
    }
    AddFloorVertices(31,31,geometryMap[floorTextureId].vertices, geometryMap[floorTextureId].indices, scaleFactor);

    for (auto geo : geometryMap)
    {
        if (geo.second.vertices.size())
        {
        std::shared_ptr<GDK::GeometryContent> geometryContent;
        std::unique_ptr<uint8_t[]> vertexData;
        std::unique_ptr<uint32_t[]> indexData;
        std::unique_ptr<GDK::GeometryContent::Animation[]> animationData;

        uint32_t attributeCount = 3;
        std::unique_ptr<GDK::GeometryContent::AttributeDesc[]> attributes(new GDK::GeometryContent::AttributeDesc[3]);
        attributes.get()[0] = GDK::GeometryContent::AttributeDesc(0, GDK::GeometryContent::AttributeType::Float3, GDK::GeometryContent::AttributeName::Position, 0);
        attributes.get()[1] = GDK::GeometryContent::AttributeDesc(12, GDK::GeometryContent::AttributeType::Float3, GDK::GeometryContent::AttributeName::Normal, 0);
        attributes.get()[2] = GDK::GeometryContent::AttributeDesc(24, GDK::GeometryContent::AttributeType::Float2, GDK::GeometryContent::AttributeName::TexCoord, 0);

        // Convert the vertices and indices into buffers appropriate for GDK
        vertexData.reset(new uint8_t[sizeof(geometryContentVertex_t) * geo.second.vertices.size()]);
        indexData.reset(new uint32_t[geo.second.indices.size()]);

        // Done processing vertices and indices, time to generate normals for all the vertices
        GenerateNormals(reinterpret_cast<const uint8_t*>(&geo.second.vertices[0].position), sizeof(geometryContentVertex_t), geo.second.vertices.size(), geo.second.indices.data(), geo.second.indices.size(), reinterpret_cast<uint8_t*>(&geo.second.vertices[0].normal));

        memcpy(vertexData.get(), geo.second.vertices.data(), sizeof(geometryContentVertex_t) * geo.second.vertices.size());
        memcpy(indexData.get(), geo.second.indices.data(), sizeof(uint32_t) * geo.second.indices.size());

        animationData.reset(new GDK::GeometryContent::Animation[1]);
        animationData.get()[0].StartFrame = 0;
        animationData.get()[0].EndFrame = 1;
        strcpy_s(animationData.get()[0].Name, "noname");

        geometryContent = GDK::GeometryContent::Create(attributeCount, attributes, (uint32_t)sizeof(geometryContentVertex_t), (uint32_t)geo.second.vertices.size(), vertexData, (uint32_t)geo.second.indices.size(), indexData, 1, 1, animationData);
        contentList.push_back(geometryContent);
        textureNameList.push_back(geo.second.textureName);
        }
    }
}

std::string GetW3DLevelName(_In_ const w3d_level_t& level)
{
    std::string levelName;
    for (uint8_t i = 0; i < _countof(level.name); i++)
    {
        if (level.name[i] == 0)
        {
            break;
        }
        levelName += level.name[i];
    }

    return levelName;
}

void Create1x1Texture(_In_ const uint8_t red, _In_ const uint8_t green, _In_ const uint8_t blue, _In_ const uint8_t alpha, _Inout_ std::shared_ptr<GDK::TextureContent>& content)
{
    std::unique_ptr<uint8_t[]> pixels;
    std::unique_ptr<GDK::RectangleF[]> frames(new GDK::RectangleF[1]);
    frames.get()[0] = GDK::RectangleF(0.0f, 0.0f, 1.0f, 1.0f);

    pixels.reset(new uint8_t[4]);
    pixels.get()[0] = (uint8_t)red;
    pixels.get()[1] = (uint8_t)green;
    pixels.get()[2] = (uint8_t)blue;
    pixels.get()[3] = (uint8_t)alpha;

    content = GDK::TextureContent::Create(1, 1, GDK::TextureFormat::R8G8B8A8, pixels, 1, frames);
}

void CreateDoorModel(_In_ float scaleFactor, _Inout_ std::shared_ptr<GDK::GeometryContent>& content)
{
    struct w3d_geometrycontentraw_t
    {
        std::wstring textureName;
        std::vector<geometryContentVertex_t> vertices;
        std::vector<uint32_t> indices;
    };

    w3d_geometrycontentraw_t rawGeometry;

    AddNSDoorVertices(0,0,rawGeometry.vertices, rawGeometry.indices, scaleFactor);

    std::shared_ptr<GDK::GeometryContent> geometryContent;
    std::unique_ptr<uint8_t[]> vertexData;
    std::unique_ptr<uint32_t[]> indexData;
    std::unique_ptr<GDK::GeometryContent::Animation[]> animationData;

    uint32_t attributeCount = 3;
    std::unique_ptr<GDK::GeometryContent::AttributeDesc[]> attributes(new GDK::GeometryContent::AttributeDesc[3]);
    attributes.get()[0] = GDK::GeometryContent::AttributeDesc(0, GDK::GeometryContent::AttributeType::Float3, GDK::GeometryContent::AttributeName::Position, 0);
    attributes.get()[1] = GDK::GeometryContent::AttributeDesc(12, GDK::GeometryContent::AttributeType::Float3, GDK::GeometryContent::AttributeName::Normal, 0);
    attributes.get()[2] = GDK::GeometryContent::AttributeDesc(24, GDK::GeometryContent::AttributeType::Float2, GDK::GeometryContent::AttributeName::TexCoord, 0);

    // Convert the vertices and indices into buffers appropriate for GDK
    vertexData.reset(new uint8_t[sizeof(geometryContentVertex_t) * rawGeometry.vertices.size()]);
    indexData.reset(new uint32_t[rawGeometry.indices.size()]);

    // Done processing vertices and indices, time to generate normals for all the vertices
    GenerateNormals(reinterpret_cast<const uint8_t*>(&rawGeometry.vertices[0].position), sizeof(geometryContentVertex_t), rawGeometry.vertices.size(), rawGeometry.indices.data(), rawGeometry.indices.size(), reinterpret_cast<uint8_t*>(&rawGeometry.vertices[0].normal));

    memcpy(vertexData.get(), rawGeometry.vertices.data(), sizeof(geometryContentVertex_t) * rawGeometry.vertices.size());
    memcpy(indexData.get(), rawGeometry.indices.data(), sizeof(uint32_t) * rawGeometry.indices.size());

    animationData.reset(new GDK::GeometryContent::Animation[1]);
    animationData.get()[0].StartFrame = 0;
    animationData.get()[0].EndFrame = 1;
    strcpy_s(animationData.get()[0].Name, "noname");

    content = GDK::GeometryContent::Create(attributeCount, attributes, (uint32_t)sizeof(geometryContentVertex_t), (uint32_t)rawGeometry.vertices.size(), vertexData, (uint32_t)rawGeometry.indices.size(), indexData, 1, 1, animationData);
}

void CreatePushWallModel(_In_ float scaleFactor, _Inout_ std::shared_ptr<GDK::GeometryContent>& content)
{
    struct w3d_geometrycontentraw_t
    {
        std::wstring textureName;
        std::vector<geometryContentVertex_t> vertices;
        std::vector<uint32_t> indices;
    };

    w3d_geometrycontentraw_t rawGeometry;

    AddSimpleCubeVertices(0,0,rawGeometry.vertices, rawGeometry.indices, scaleFactor);

    std::shared_ptr<GDK::GeometryContent> geometryContent;
    std::unique_ptr<uint8_t[]> vertexData;
    std::unique_ptr<uint32_t[]> indexData;
    std::unique_ptr<GDK::GeometryContent::Animation[]> animationData;

    uint32_t attributeCount = 3;
    std::unique_ptr<GDK::GeometryContent::AttributeDesc[]> attributes(new GDK::GeometryContent::AttributeDesc[3]);
    attributes.get()[0] = GDK::GeometryContent::AttributeDesc(0, GDK::GeometryContent::AttributeType::Float3, GDK::GeometryContent::AttributeName::Position, 0);
    attributes.get()[1] = GDK::GeometryContent::AttributeDesc(12, GDK::GeometryContent::AttributeType::Float3, GDK::GeometryContent::AttributeName::Normal, 0);
    attributes.get()[2] = GDK::GeometryContent::AttributeDesc(24, GDK::GeometryContent::AttributeType::Float2, GDK::GeometryContent::AttributeName::TexCoord, 0);

    // Convert the vertices and indices into buffers appropriate for GDK
    vertexData.reset(new uint8_t[sizeof(geometryContentVertex_t) * rawGeometry.vertices.size()]);
    indexData.reset(new uint32_t[rawGeometry.indices.size()]);

    // Done processing vertices and indices, time to generate normals for all the vertices
    GenerateNormals(reinterpret_cast<const uint8_t*>(&rawGeometry.vertices[0].position), sizeof(geometryContentVertex_t), rawGeometry.vertices.size(), rawGeometry.indices.data(), rawGeometry.indices.size(), reinterpret_cast<uint8_t*>(&rawGeometry.vertices[0].normal));

    memcpy(vertexData.get(), rawGeometry.vertices.data(), sizeof(geometryContentVertex_t) * rawGeometry.vertices.size());
    memcpy(indexData.get(), rawGeometry.indices.data(), sizeof(uint32_t) * rawGeometry.indices.size());

    animationData.reset(new GDK::GeometryContent::Animation[1]);
    animationData.get()[0].StartFrame = 0;
    animationData.get()[0].EndFrame = 1;
    strcpy_s(animationData.get()[0].Name, "noname");

    content = GDK::GeometryContent::Create(attributeCount, attributes, (uint32_t)sizeof(geometryContentVertex_t), (uint32_t)rawGeometry.vertices.size(), vertexData, (uint32_t)rawGeometry.indices.size(), indexData, 1, 1, animationData);
}