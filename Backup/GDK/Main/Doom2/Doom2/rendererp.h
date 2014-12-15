#ifndef _RENDERERP_H_
#define _RENDERERP_H_

//
// contains all internal structures and methods used between renderer modules
//

struct sidedef_s;
struct sector_s;

// a post of pixels written to the screen, used for clipping (replacement for pure z buffer)
typedef struct
{
    // start and end pixel row (y value)
    byte_t start;
    byte_t end;
    float z;
} screenpost_t;

typedef struct
{
    short width;
    short height;
    short left;
    short top;
    int offsets[1];
} patch_t;

typedef struct
{
    vector2_t start;
    vector2_t end;
    short flags;
    short special;
    short tag;
    struct sidedef_s* rightSide;
    struct sidedef_s* leftSide;
} linedef_t;

typedef struct sidedef_s
{
    short xOffset;
    short yOffset;
    resource_t upperTexture;
    resource_t midTexture;
    resource_t lowerTexture;
    struct sector_s* sector;
} sidedef_t;

typedef struct
{
    vector2_t start;
    vector2_t end;
    linedef_t* line;
    sidedef_t* side;
} seg_t;

typedef struct
{
    short NumSegs;
    short FirstSeg;
} subsector_t;

typedef struct sector_s
{
    float floorHeight;
    float ceilingHeight;
    resource_t floorFlat;
    resource_t ceilingFlat;
    short lightLevel;
    short type;
    short tagNumber;
} sector_t;

typedef struct
{
    linedef_t* linedefs;
    sidedef_t* sidedefs;
    seg_t* segs;
    subsector_t* subsectors;
    sector_t* sectors;

    short numLinedefs;
    short numSidedefs;
    short numSegs;
    short numSubsectors;
    short numSectors;
} rendererp_t;

extern rendererp_t g_renderer;

//
// Methods
//

BOOL RendererpLoadMap(const char* lumpName);
void RendererpDestroy();

#endif // _RENDERERP_H_