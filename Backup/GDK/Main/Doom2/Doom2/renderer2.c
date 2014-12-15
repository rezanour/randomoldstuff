#include "common.h"
#include "renderer.h"
#include "rendererp.h"
#include "graphics.h"
#include "rasterizer.h"
#include "images.h"
#include "map.h"
#include "mapp.h"

#if 1

//
// Renderer Globals
//

static const float g_fov = TORADIANS(90);
static float g_halfFov = 0.0f;
static float g_zPlane = 0.0f;

//
// Renderer Data Private Structures and Method Declarations
//

// A clipped portion of a seg that's visible and needs drawing
typedef struct
{
    seg_t* seg;

    // x & y ranges are in screen pixels
    short xStart;
    short xEnd;
    short yStart;
    short yEnd;

    // scale at start and stop, and scale step
    float scaleStart;
    float scaleEnd;
    float scaleStep;

    // dist along seg from start (used for u coordinate of texture)
    float segStart;
    float segEnd;
    float segDistStep;

    // texture stuff
    float midTextureVStart;
    float topTextureVStart;
    float bottomTextureVStart;
} renderseg_t;

#define MAX_RENDERSEGS 256
static renderseg_t g_renderSegs[MAX_RENDERSEGS] = {0};
static renderseg_t* g_nextRenderSeg = NULL;

static void ClearRenderSegs();

// remaining portion of the column that hasn't been drawn to yet. (in screen pixels)
typedef struct
{
    short start;
    short end;
} columnbounds_t;

#define MAX_COLUMNS 320
static columnbounds_t g_columnBounds[MAX_COLUMNS] = {0};

static void ClearColumnBounds();

// when solid portions of walls are drawn, nothing further in those columns 
// should be drawn. We mask off the entire range of columns using clipranges
typedef struct
{
    short start;
    short end;
} cliprange_t;

#define MAX_RANGES 32
static cliprange_t g_clipRanges[MAX_RANGES] = {0};
static cliprange_t* g_nextClipRange = NULL;

static void ClearClipRanges();

// Rendering

#define NODE_LEAF 0x8000
#define NODE_INDEX 0x7FFF

static vector2_t g_position;
static vector2_t g_view;
static float g_viewAngle;
static float g_viewZ;

// temp data kept global during processing
static sector_t* g_frontsector;
static sector_t* g_backsector;
static resource_t g_midTexture;
static resource_t g_topTexture;
static resource_t g_bottomTexture;

static float NormalizeAngle(float angle);
static short PointOnSide(vector2_t* position, node_t* node);
static float GetZFromScreenPosition(seg_t* seg, short x);
static void RenderNode(ushort_t index);
static void RenderSubsector(ushort_t index);
static void RenderSeg(seg_t* seg);
static void ClipAndRenderSolidSeg(seg_t* seg, short xStart, short xEnd);
static void ClipAndRenderTwoSidedSeg(seg_t* seg, short xStart, short xEnd);
static void AddClippedSegPiece(seg_t* seg, short xStart, short xEnd);
static void RenderSegToDisplay(renderseg_t* renderseg);
static byte_t* GetColumnFromTexture(texture_t* texture);
static void RenderColumn(short x, short yStart, short yEnd, byte_t* source);

//
// Public Renderer Methods
//

BOOL RendererInitialize()
{
    float width;

    g_halfFov = 0.5f * g_fov;
    width = (float)GraphicsGetScreenWidth();
    g_zPlane = width / (2.0f * tanf(g_halfFov));

    return TRUE;
}

void RendererUninitialize()
{
}

void Render3DView()
{
    // 1. Traverse BSP and build up render seg list, building and using clip list along the way
    // 2. Traverse render seg list, rendering segments to the screen, and building up vis planes (flats).
    //      Use upper/lower bounds per column to correctly order two sided segs
    // 3. Traverse transparent list back to front to draw transparent segs, leveraging bounds data from above
    //
    // TODO: As this algorithm starts to come together, see if we can combine some of these steps. For instance,
    //   if we build the bounds data as we traverse, we can likely find occlusions (and therefore skip) more segments.
    //   This will save time in the later passes. BUT, we should pay attention to cache coherance. It's desirable
    //   to keep pixel drawing focused on pixel drawing, and bsp/data touching focused on that to keep cache coherence high

    ClearColumnBounds();
    ClearRenderSegs();
    ClearClipRanges();

    g_position = PlayerGetPosition();
    g_view = PlayerGetDirection();
    g_viewAngle = atan2f(g_view.Y, g_view.X);
    g_viewZ = 32;

    RenderNode(g_map.NumNodes - 1);
}

//
// Private Renderer Methods
//

void ClearRenderSegs()
{
    g_nextRenderSeg = g_renderSegs;
}

void ClearColumnBounds()
{
    uint_t i;

    for (i = 0; i < MAX_COLUMNS; ++i)
    {
        g_columnBounds[i].start = 0;
        g_columnBounds[i].end = 200;
    }
}

void ClearClipRanges()
{
    // insert two sentinels to simplify code later
    g_clipRanges[0].start = -1000;
    g_clipRanges[0].end = 0;
    g_clipRanges[1].start = 320;
    g_clipRanges[1].end = 1000;
    g_nextClipRange = g_clipRanges + 2;
}


void ClipAndUpdateColumnBounds(short x, short* yStart, short* yEnd)
{
    columnbounds_t* column = &g_columnBounds[x];

    // clip yStart and yEnd, updating column bounds as well
    if (*yStart < column->start)
    {
        *yStart = column->start;
    }
    else if (*yStart > column->start)
    {
        column->start = *yStart;
    }

    if (*yEnd > column->end)
    {
        *yEnd = column->end;
    }
    else if (*yEnd < column->end)
    {
        column->end = *yEnd;
    }
}

short PointOnSide(vector2_t* position, node_t* node)
{
    vector2_t perp = Vector2Perp(Vector2(node->xDir, node->yDir));
    float dot = Vector2Dot(Vector2Subtract(*position, Vector2(node->xStart, node->yStart)), perp);
    return dot >= 0 ? 0 : 1;
}

// normalize the angle into 0 -> 2PI
float NormalizeAngle(float angle)
{
    static const float TwoPi = 2 * 3.14156f;

    while (angle < 0)
    {
        angle += TwoPi;
    }

    while (angle >= TwoPi)
    {
        angle -= TwoPi;
    }

    return angle;
}

float GetZFromScreenPosition(seg_t* seg, short x)
{
    vector2_t dir, toPt, norm, pt;
    float angle, dist, d, z;

    angle = atan2f(x - 160, g_zPlane);
    angle = g_viewAngle - angle;
    dir = Vector2(cosf(angle), sinf(angle));
    toPt = Vector2Subtract(seg->start, g_position);
    norm = Vector2Perp(Vector2Normalize(Vector2Subtract(seg->end, seg->start)));
    d = Vector2Dot(toPt, norm);
    dist = d / Vector2Dot(dir, norm);
    pt = Vector2Scale(dir, dist);
    z = Vector2Dot(pt, g_view);
    return z;
}

void RenderNode(ushort_t index)
{
    if (index & NODE_LEAF)
    {
        RenderSubsector(index & NODE_INDEX);
    }
    else
    {
        node_t* node = &g_map.Nodes[index];
        short side = PointOnSide(&g_position, node);

        // process closer side
        RenderNode(*(&node->RightChild + side));

        // then other child (TODO: clip/check bbox)
        side = side ? 0 : 1;
        RenderNode(*(&node->RightChild + side));
    }
}

void RenderSubsector(ushort_t index)
{
    subsector_t* subsector = &g_renderer.subsectors[index];
    seg_t* seg = &g_renderer.segs[subsector->FirstSeg];
    int count = subsector->NumSegs;

    g_frontsector = seg->side->sector;

    // TODO: Process visplanes for ceiling and floor

    // TODO: Process sprites in the sector

    // Process segs
    while (count--)
    {
        RenderSeg(seg);
        ++seg;
    }
}

// Clips the segment against clip ranges and adds 
// rendersegs to the list for the pieces that are visible
void RenderSeg(seg_t* seg)
{
    vector2_t toStart;
    vector2_t toEnd;
    vector2_t norm;
    float startAngle;
    float endAngle;
    float startSide, endSide;
    short xStart;
    short xEnd;
    short dx;

    // Vectors from current position to the start & end of the seg
    toStart = Vector2Subtract(seg->start, g_position);
    toEnd = Vector2Subtract(seg->end, g_position);

    norm = Vector2Perp(Vector2Normalize(Vector2Subtract(seg->end, seg->start)));
    if (Vector2Dot(norm, g_position) <= Vector2Dot(seg->start, norm))
    {
        // behind the wall (back facing)
        return;
    }

    startAngle = acosf(Vector2Dot(g_view, Vector2Normalize(toStart)));
    startSide = Vector2Dot(Vector2Perp(g_view), toStart);
    endAngle = acosf(Vector2Dot(g_view, Vector2Normalize(toEnd)));
    endSide = Vector2Dot(Vector2Perp(g_view), toEnd);

    if (startAngle > g_halfFov)
    {
        // if we're off the screen to the right, cull
        if (startSide > 0)
        {
            return;
        }
        startAngle = g_halfFov;
    }
    if (endAngle > g_halfFov)
    {
        if (endSide < 0)
        {
            return;
        }
        endAngle = g_halfFov;
    }

    startAngle = startSide > 0 ? -startAngle : startAngle;
    endAngle = endSide > 0 ? -endAngle : endAngle;

    dx = (short)(tanf(startAngle) * g_zPlane);
    xStart = 160 - dx;
    dx = (short)(tanf(endAngle) * g_zPlane);
    xEnd = 160 - dx;

    if (xStart >= xEnd)
    {
        // doesn't cover any pixels
        return;
    }

    if (seg->line->leftSide == seg->side)
    {
        g_backsector = seg->line->rightSide ? seg->line->rightSide->sector : NULL;
    }
    else
    {
        g_backsector = seg->line->leftSide ? seg->line->leftSide->sector : NULL;
    }

    if (!g_backsector)
    {
        // single sided line (solid seg)
        ClipAndRenderSolidSeg(seg, xStart, xEnd);
    }
    else if (g_backsector->ceilingHeight <= g_frontsector->floorHeight ||
        g_backsector->floorHeight >= g_frontsector->ceilingHeight)
    {
        // closed door, treat as solid seg
        ClipAndRenderSolidSeg(seg, xStart, xEnd);
    }
    else if (g_backsector->ceilingFlat == g_frontsector->ceilingFlat &&
        g_backsector->floorFlat == g_frontsector->floorFlat &&
        g_backsector->lightLevel == g_frontsector->lightLevel &&
        seg->side->midTexture == INVALID_RESOURCE)
    {
        // no-op seg. maybe a trigger, etc... or just a partition line for bsp
        return;
    }
    else
    {
        // two sided segment
        ClipAndRenderTwoSidedSeg(seg, xStart, xEnd);
    }
}

void ClipAndRenderSolidSeg(seg_t* seg, short xStart, short xEnd)
{
    cliprange_t* next;
    cliprange_t* start;

    start = g_clipRanges;

    // subtract 1 from xEnd to make it inclusive (simplifies stuff below)
    --xEnd;

    // in cases below, we check specifically with 1 offsets
    // to catch ranges that don't overlap, but do touch us.
    // those ranges can be collapsed later

    // skip past all ranges completely before us
    while (start->end < xStart - 1)
    {
        ++start;
    }

    if (xStart < start->start)
    {
        // our starting position is visible, and we might be clipped
        if (xEnd < start->start - 1)
        {
            // entirely in view, no clipping required
            AddClippedSegPiece(seg, xStart, xEnd);

            // insert new clip range
            next = g_nextClipRange++;

            // push the ranges down to make room
            while (next != start)
            {
                *next = *(next - 1);
                --next;
            }

            next->start = xStart;
            next->end = xEnd;
            return;
        }

        // chunk from xStart -> start->start - 1 is visible
        AddClippedSegPiece(seg, xStart, start->start - 1);

        // since we're contiguous with the current cliprange, we 
        // can just extend it out to cover the solid area we cover
        start->start = xStart;
    }

    // if our end is still inside the same cliprange, then the remainder of
    // our seg is covered up, so clip it
    if (xEnd <= start->end)
    {
        return;
    }

    // we extend past the current cliprange, so keep walking until we fully 
    // process our seg
    next = start;

    // because we've inserted sentinel clipranges in the list, we can 
    // use a trick of skipping our current last, and actually looking into 
    // the next node's start to see if we span past this node & into another.
    // if true, that means we have a chunk that's completely bridging these two
    // and needs to be rendered. We can then collapse the clipranges into 1
    while (xEnd >= (next + 1)->start - 1)
    {
        // there's a chunk filling gap from next->end + 1 to (next + 1)->start
        AddClippedSegPiece(seg, next->end + 1, (next + 1)->start - 1);

        // but what if there're more chunks between more ranges? keep doing this
        // until we finally find where our segment sticks out unclipped
        ++next;

        if (xEnd <= next->end)
        {
            // xEnd doesn't extend out any further, and is contained inside next
            start->end = next->end;
            break;
        }
    }

    if (xEnd > next->end)
    {
        // there's a piece sticking out
        AddClippedSegPiece(seg, next->end + 1, xEnd);

        start->end = xEnd;
    }

    if (next != start)
    {
        // Collapse any ranges we skipped over
        while (next++ != g_nextClipRange)
        {
            ++start;
            *start = *next;
        }

        g_nextClipRange = start + 1;
    }
}

void ClipAndRenderTwoSidedSeg(seg_t* seg, short xStart, short xEnd)
{
    cliprange_t* start;

    start = g_clipRanges;

    // subtract 1 from xEnd to make it inclusive (simplifies stuff below)
    --xEnd;

    // in cases below, we check specifically with 1 offsets
    // to catch ranges that don't overlap, but do touch us.

    // skip past all ranges completely before us
    while (start->end < xStart - 1)
    {
        ++start;
    }

    if (xStart < start->start)
    {
        // our starting position is visible, and we might be clipped
        if (xEnd < start->start - 1)
        {
            // entirely in view, no clipping required
            AddClippedSegPiece(seg, xStart, xEnd);
            return;
        }

        // chunk from xStart -> start->start - 1 is visible
        AddClippedSegPiece(seg, xStart, start->start - 1);
    }

    // if our end is still inside the same cliprange, then the remainder of
    // our seg is covered up, so clip it
    if (xEnd <= start->end)
    {
        return;
    }

    while (xEnd >= (start + 1)->start - 1)
    {
        // there's a chunk filling gap from next->end + 1 to (next + 1)->start
        AddClippedSegPiece(seg, start->end + 1, (start + 1)->start - 1);
        ++start;

        if (xEnd <= start->end)
        {
            return;
        }
    }

    // there's a piece sticking out
    AddClippedSegPiece(seg, start->end + 1, xEnd);
}

void AddClippedSegPiece(seg_t* seg, short xStart, short xEnd)
{
    renderseg_t* renderseg;
    float worldTop, worldBottom, worldHigh, worldLow;

    if (g_nextRenderSeg - g_renderSegs == MAX_RENDERSEGS)
    {
        // no room!
        return;
    }

    renderseg = g_nextRenderSeg++;

    renderseg->seg = seg;
    renderseg->xStart = xStart;
    renderseg->xEnd = xEnd;

    // find the two extents that we intersect the segment,
    // and compute the scale & distance data for them
    renderseg->scaleStart = g_zPlane / GetZFromScreenPosition(seg, xStart);

    // and the other side
    GetZFromScreenPosition(seg, xEnd);
    renderseg->scaleEnd = g_zPlane / GetZFromScreenPosition(seg, xEnd);

    // step values
    renderseg->scaleStep = (renderseg->scaleEnd - renderseg->scaleStart) / (xEnd - xStart);

    // texture calculations

    g_midTexture = g_topTexture = g_bottomTexture = INVALID_RESOURCE;

    worldTop = g_frontsector->ceilingHeight - g_viewZ;
    worldBottom = g_frontsector->floorFlat - g_viewZ;

    if (!g_backsector)
    {
        // one sided solid line
        g_midTexture = seg->side->midTexture;
        if (seg->line->flags & LINE_LOWERTEX_UNPEGGED)
        {
            renderseg->midTextureVStart = g_frontsector->floorHeight +
                ((texture_t*)ResourceManagerDataFromHandle(resource_type_texture, g_midTexture))->Height;
            renderseg->midTextureVStart -= g_viewZ;
        }
        else
        {
            renderseg->midTextureVStart = worldTop;
        }
        renderseg->midTextureVStart += seg->side->yOffset;
    }
    else
    {
        // two-sided line
        worldHigh = g_backsector->ceilingHeight - g_viewZ;
        worldLow = g_backsector->floorHeight - g_viewZ;
        if (worldHigh < worldTop)
        {
            // top texture visible
            g_topTexture = seg->side->upperTexture;
            if (g_topTexture != INVALID_RESOURCE)
            {
                if (seg->line->flags & LINE_UPPERTEX_UNPEGGED)
                {
                    renderseg->topTextureVStart = worldTop;
                }
                else
                {
                    renderseg->topTextureVStart = g_backsector->ceilingHeight +
                        ((texture_t*)ResourceManagerDataFromHandle(resource_type_texture, g_topTexture))->Height;
                    renderseg->topTextureVStart -= g_viewZ;
                }
            }
        }
        if (worldLow > worldBottom)
        {
            // bottom texture visible
            g_bottomTexture = seg->side->lowerTexture;
            if (g_bottomTexture != INVALID_RESOURCE)
            {
                if (seg->line->flags & LINE_LOWERTEX_UNPEGGED)
                {
                    renderseg->bottomTextureVStart = worldTop;
                }
                else
                {
                    renderseg->bottomTextureVStart = worldLow;
                }
            }
        }
        renderseg->topTextureVStart += seg->side->yOffset;
        renderseg->bottomTextureVStart += seg->side->yOffset;
    }

    RenderSegToDisplay(renderseg);
}

void RenderSegToDisplay(renderseg_t* renderseg)
{
    short x;
    float scale;

    renderseg->yStart = (short)(g_viewZ - g_frontsector->ceilingHeight);
    renderseg->yEnd = (short)(g_viewZ - g_frontsector->floorHeight);

    x = renderseg->xStart;
    scale = renderseg->scaleStart;
    while (x <= renderseg->xEnd)
    {
        short yStart = (short)(100 + renderseg->yStart * scale);
        short yEnd = (short)(100 + renderseg->yEnd * scale);

        if (yStart < 0)
        {
            // completely off screen above?
            if (yEnd < 0)
            {
                ++x;
                scale += renderseg->scaleStep;
                continue;
            }
            // clip
            yStart = 0;
        }

        if (yEnd >= 200)
        {
            if (yStart >= 200)
            {
                ++x;
                scale += renderseg->scaleStep;
                continue;
            }
            yEnd = 200;
        }

        if (g_midTexture != INVALID_RESOURCE)
        {
            // solid wall
            byte_t* source = GetColumnFromTexture((texture_t*)ResourceManagerDataFromHandle(resource_type_texture, g_midTexture));
            RenderColumn(x, yStart, yEnd, source);
        }
        else
        {
            // two sided
            if (g_topTexture != INVALID_RESOURCE)
            {
                byte_t* source = GetColumnFromTexture((texture_t*)ResourceManagerDataFromHandle(resource_type_texture, g_topTexture));
                RenderColumn(x, yStart, yEnd - ((yEnd - yStart) / 2), source);
            }
            if (g_bottomTexture != INVALID_RESOURCE)
            {
                byte_t* source = GetColumnFromTexture((texture_t*)ResourceManagerDataFromHandle(resource_type_texture, g_bottomTexture));
                RenderColumn(x, yStart + ((yEnd - yStart) / 2), yEnd, source);
            }
        }

        ++x;
        scale += renderseg->scaleStep;
    }
}

byte_t* GetColumnFromTexture(texture_t* texture)
{
    sprite_t* patch = (sprite_t*)ResourceManagerDataFromHandle(resource_type_sprite, texture->Sprites[0].Sprite);
    return (byte_t*)(patch + patch->ColumnOffsets[0]);
}

void RenderColumn(short x, short yStart, short yEnd, byte_t* source)
{
    short y = yStart;
    while (y < yEnd)
    {
        RasterizePixel(x, y, source[0]);
        ++y;
    }
}

#endif