#include "common.h"
#include "mapp.h"
#include "graphicsp.h"
#include "images.h"
#include "rendererp.h"

#ifdef USE_RENDEREZA

//
// TODO: Currently rendereza doesn't support different sized viewports....
// 

#define SCREENX         320
#define SCREENY         168 // HUD makes up the rest
#define CENTERX         160
#define CENTERY         84

#define MAX_RENDERSEGS  256
#define MAX_CLIPPOSTS   32

#define PI              3.14156f
#define TWOPI           2.0f * PI

// view position and orientation
static float g_height;
static vector2_t g_position;
static vector2_t g_direction;
static vector2_t g_right;
static float g_angle;

// field of view
static float g_fov;
static float g_halfFov;

// distance to view plane based on FoV
static float g_zPlane;

// a single, renderable seg. This gets built up and
// passed around through the pipeline to render a seg
typedef struct
{
    // basic segment info
    seg_t* seg;
    int subsector;
    vector2_t start, end;
    vector2_t negNorm;
    float dist;

    // projected x values on screen
    float xStart, xEnd;

    // height and sector info
    float dhTop, dhBottom;

    // texture info
    texture_t* topTex;
    texture_t* midTex;
    texture_t* botTex;
    int uOffset, vOffset;
} renderseg_t;

static int g_numRenderSegs = 0;
static renderseg_t g_renderSegs[MAX_RENDERSEGS] = {0};


// clip posts fill in the screen as fragments are rendered,
// and provide an easy way to clip incoming fragments
typedef struct
{
    byte_t top;
    byte_t bottom; 
    float z;
} clippost_t;

// screen column contains all the clipping info for a particular column of the screen
typedef struct
{
    byte_t numPixels;
    short numPosts;
    clippost_t posts[MAX_CLIPPOSTS];
} screencolumn_t;

static screencolumn_t g_screenColumns[SCREENX] = {0};


//
// Private Methods
//

// BSP support

// TRUE return means keep processing tree, FALSE means we're done
static BOOL ProcessNode(node_t* node);
static BOOL ProcessChild(ushort_t child);
static BOOL IsLeafNode(ushort_t child);
static ushort_t GetIndexFromChild(ushort_t child);
static BOOL IsInFront(node_t* node);


//
// To make the pipeline more easily debuggable, we use function pointers
// which can be replaced with debug versions.
//

// Process a subsector encountered in the BSP. This method will process and add any rendersegs in the subsector that are in view to the list.
typedef void (*pfnProcessSubsector)(int subsectorIndex);

// Raycast across the screen and render
typedef void (*pfnRenderSegs)();

// Raycast and rasterize a single column of a renderseg
typedef void (*pfnRasterizeSegColumn)(renderseg_t* seg, vector2_t* ray, int x);

static pfnProcessSubsector          g_pfnProcessSubsector;
static pfnRenderSegs                g_pfnRenderSegs;
static pfnRasterizeSegColumn        g_pfnRasterizeSegColumn;

//
// Implementations of the various stages of the pipeline
//

static void ProcessSubsector(int subsectorIndex);
static void RenderSegs();
static void RasterizeSegColumn(renderseg_t* seg, vector2_t* ray, int x);


//
// Public Renderer Methods
//

BOOL RendererInitialize()
{
    g_pfnProcessSubsector           = ProcessSubsector;
    g_pfnRenderSegs                 = RenderSegs;
    g_pfnRasterizeSegColumn         = RasterizeSegColumn;

    g_fov = TORADIANS(90);
    g_halfFov = 0.5f * g_fov;

    g_zPlane = (float)(CENTERX / tan(g_halfFov));

    return TRUE;
}

void RendererUninitialize()
{
}

void Render3DView()
{
    int i;

    g_position = PlayerGetPosition();
    g_direction = PlayerGetDirection();
    g_angle = (float)atan2(g_direction.Y, g_direction.X);
    g_right = Vector2Perp(g_direction);

    // DEBUG: eventually player will track their height correctly themselves
    {
        int playerSubsector = BSPFindSubsector(PlayerGetPosition());
        subsector_t* subsec = &g_renderer.subsectors[playerSubsector];
        seg_t* seg = &g_renderer.segs[subsec->FirstSeg];
        sector_t* sector = seg->side->sector;
        g_height = sector->floorHeight + 41;
    }

    for (i = 0; i < SCREENX; ++i)
    {
        g_screenColumns[i].numPixels = 0;
        g_screenColumns[i].numPosts = 0;
    }

    // walk the BSP tree and build render seg list
    g_numRenderSegs = 0;
    ProcessNode(&g_map.Nodes[g_map.NumNodes - 1]);

    g_pfnRenderSegs();
}


//
// private helpers
//

void ProcessSubsector(int subsectorIndex)
{
    int i;

    subsector_t* subsector = &g_renderer.subsectors[subsectorIndex];

    for (i = 0; i < subsector->NumSegs && g_numRenderSegs < MAX_RENDERSEGS; ++i)
    {
        seg_t* seg = &g_renderer.segs[subsector->FirstSeg + i];
        vector2_t toStart = Vector2Subtract(seg->start, g_position);
        vector2_t toEnd = Vector2Subtract(seg->end, g_position);

        float angStart = (float)atan2(toStart.Y, toStart.X);
        float angEnd = (float)atan2(toEnd.Y, toEnd.X);

        angStart = g_angle - angStart;
        angEnd = g_angle - angEnd;

        // ensure the seg is in view
        //if (angEnd >= -g_halfFov && angStart <= g_halfFov)
        {
            // ensure the seg is facing us
            {
                renderseg_t* renderseg = &g_renderSegs[g_numRenderSegs++];

                renderseg->seg = seg;
                renderseg->subsector = subsectorIndex;
                renderseg->start = seg->start;
                renderseg->end = seg->end;
                renderseg->negNorm = Vector2Perp(Vector2Normalize(Vector2Subtract(seg->start, seg->end)));
                renderseg->dist = Vector2Dot(toStart, renderseg->negNorm);

                if (renderseg->dist < 0)
                {
                    // facing away from us
                    g_numRenderSegs--;
                    continue;
                }

                renderseg->xStart = (float)tan(angStart) * g_zPlane;
                renderseg->xEnd = (float)tan(angEnd) * g_zPlane;

                renderseg->dhTop = seg->side->sector->ceilingHeight - g_height;
                renderseg->dhBottom = seg->side->sector->floorHeight - g_height;

                // TODO: compute texture info
            }
        }
    }
}


void RenderSegs()
{
    int i, x, skipsubsector;
    vector2_t ray;
    vector2_t forward = Vector2Scale(g_direction, g_zPlane);

    for (x = -CENTERX; x < CENTERX; ++x)
    {
        ray = Vector2Normalize(Vector2Add(forward, Vector2Scale(g_right, (float)x)));

        skipsubsector = -1;
        for (i = 0; i < g_numRenderSegs; ++i)
        {
            renderseg_t* seg = &g_renderSegs[i];
            if (seg->subsector != skipsubsector && x >= seg->xStart && x <= seg->xEnd)
            {
                g_pfnRasterizeSegColumn(seg, &ray, x);

                // there can only be a single seg from each subsector visible at each column ray
                //skipsubsector = seg->subsector;
            }
        }
    }
}

void RasterizeSegColumn(renderseg_t* seg, vector2_t* ray, int x)
{
    float r, dot, z, u;
    vector2_t scaledRay, pt;
    int y, yStart, yEnd;

    if (seg->seg->line->leftSide > 0)
    {
        return;
    }

    dot = Vector2Dot(*ray, seg->negNorm);
    if (dot > 0)
    {
        r = seg->dist / dot;
        scaledRay = Vector2Scale(*ray, r);
        z = Vector2Dot(scaledRay, g_direction);
        pt = Vector2Add(g_position, scaledRay);
        u = Vector2Length(Vector2Subtract(pt, seg->start));

        yStart = CENTERY - (int)(seg->dhTop * g_zPlane / z);
        yEnd = CENTERY - (int)(seg->dhBottom * g_zPlane / z);

        // clip to screen bounds
        yStart = min(max(yStart, 0), SCREENY);
        yEnd = min(max(yEnd, 0), SCREENY);

        for (y = yStart; y < yEnd; ++y)
        {
            RasterizePixel(CENTERX + x, y, 200);
        }
    }
}

//
// BSP support
//

BOOL ProcessNode(node_t* node)
{
    if (IsInFront(node))
    {
        if (ProcessChild(node->RightChild))
        {
            return ProcessChild(node->LeftChild);
        }
    }
    else
    {
        if (ProcessChild(node->LeftChild))
        {
            return ProcessChild(node->RightChild);
        }
    }

    return FALSE;
}

BOOL ProcessChild(ushort_t child)
{
    if (IsLeafNode(child))
    {
        g_pfnProcessSubsector(GetIndexFromChild(child));
        return (g_numRenderSegs < MAX_RENDERSEGS);
    }
    else
    {
        return ProcessNode(&g_map.Nodes[GetIndexFromChild(child)]);
    }
}

BOOL IsLeafNode(ushort_t child)
{
    return (child & 0x8000) != 0;
}

ushort_t GetIndexFromChild(ushort_t child)
{
    return child & 0x7FFF;
}

BOOL IsInFront(node_t* node)
{
    vector2_t norm = Vector2Perp(Vector2(node->xDir, node->yDir));
    vector2_t dir = Vector2Subtract(g_position, Vector2(node->xStart, node->yStart));
    return Vector2Dot(norm, dir) >= 0;
}

#endif