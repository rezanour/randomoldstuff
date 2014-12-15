#ifndef _MAPP_H_
#define _MAPP_H_

#pragma pack(push,1)

//
// VERTEXES - I don't need to explain....
//

typedef struct
{
    short x;
    short y;
} vertex_t;


//
// LINEDEFS - they define the borders of sectors, and can act as tripwires, etc...
//

#define LINE_BLOCKS_ALL         0x0001
#define LINE_BLOCKS_MONSTERS    0x0002
#define LINE_TWO_SIDED          0x0004
#define LINE_UPPERTEX_UNPEGGED  0x0008
#define LINE_LOWERTEX_UNPEGGED  0x0010
#define LINE_SECRET             0x0020
#define LINE_BLOCKS_SOUND       0x0040
#define LINE_NEVER_SHOW_ON_MAP  0x0080
#define LINE_ALWAYS_SHOW_ON_MAP 0x0100

typedef struct
{
    short StartVertex;
    short EndVertex;
    short Flags;
    short Special;
    short Tag;
    short RightSidedef;
    short LeftSidedef;
} maplinedef_t;


//
// SIDEDEFS - these describe more detailed information about lines, and can be shared amongst them
//
typedef struct
{
    short xOffset;
    short yOffset;
    char UpperTexture[NAME_SIZE];
    char LowerTexture[NAME_SIZE];
    char MiddleTexture[NAME_SIZE];
    short Sector;
} mapsidedef_t;


//
// SEGS - these are pieces of linedefs after clipping by BSP. These are what get fed to rendering
//
typedef struct
{
    short StartVertex;
    short EndVertex;
    short Angle;
    short Linedef;
    short Side; // 0 == same normal as linedef, 1 == opposite (backside of linedef)
    short Offset; // distance along linedef to start of seg
} mapseg_t;


//
// SSECTORS - subsectors
//
typedef struct
{
    short NumSegs;
    short FirstSeg;
} mapsubsector_t;


//
// SECTORS - These are enclosed by linedefs and define floor/ceiling height, etc...
//

typedef enum
{
    sector_type_normal = 0,
    sector_type_blinkRandom,
    sector_type_blinkHalfSecond,
    sector_type_blinkOneSecond,
    sector_type_20DmgAndBlinkHalfSecond,
    sector_type_10DmgPerSecond,
    sector_type_5DmgPerSecond,
    sector_type_lightOscillates,
    sector_type_secret,
    sector_type_30SecDoorClose,
    sector_type_CancelGodMode20DmgPerSecThenEndLevelWhenPlayerDies,
    sector_type_blinkHalfSecondSync,
    sector_type_blinkOneSecondSync,
    sector_type_300SecDoorOpen,
    sector_type_20DmgPerSecond,
    sector_type_lightFlickersRandom
} sector_type_t;

typedef struct
{
    short FloorHeight;
    short CeilingHeight;
    char FloorTexture[NAME_SIZE];
    char CeilingTexture[NAME_SIZE];
    short LightLevel;
    short Type;
    short TagNumber;
} mapsector_t;


//
// NODES - the BSP
//
typedef struct
{
    short top;
    short bottom;
    short left;
    short right;
} boundingbox_t;

typedef struct
{
    short xStart;
    short yStart;
    short xDir;
    short yDir;
    boundingbox_t RightBBox;
    boundingbox_t LeftBBox;
    ushort_t RightChild;    // if high bit set, then abs(RightChild) == leaf node (subsector index)
    ushort_t LeftChild;     // if high bit clear, then == sub node index
} node_t;


#pragma pack(pop)


typedef struct
{
    //
    // Map basics
    //

    char Name[NAME_SIZE];
    byte_t Episode;
    byte_t Mission;
    byte_t Difficulty;

    //
    // Resources
    //

    resourcemanager_t ResourceManager;

    //
    // Map BSP
    //

    ushort_t NumNodes;
    node_t* Nodes;
} map_t;

extern map_t g_map;

BOOL MapLoadLumpInto(const char* map, const char* lump, void* buffer, ushort_t sizeOfElement, ushort_t* count);

resource_t MapGetFlat(const char* name);
resource_t MapGetSprite(const char* name);
resource_t MapGetTexture(const char* name);

resource_t MapGetSound(const char* name);

#endif // _MAPP_H_
