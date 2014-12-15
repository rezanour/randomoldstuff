#include "common.h"
#include "mapp.h"
#include "graphicsp.h"
#include "images.h"
#include "rendererp.h"

#if 0

#ifndef USE_RENDEREZA

#define MAX_RENDERED_SUBSECTORS     256

typedef struct 
{
    // View position and orientation
    vector2_t Position;
    vector2_t Direction;
    vector2_t Right;

    // Direction scaled by zPlane
    vector2_t ScaledForward;

    // left & right edge of FOV
    vector2_t FovLeft;
    vector2_t FovRight;
    vector2_t PerpFovLeft;
    vector2_t PerpFovRight;

    float Fov;
    float HalfFov;
    float zPlane;

    // View Height
    float Height;

    short ScreenWidth;
    short ScreenHeight;
    short ScreenLeft;
    short ScreenTop;
    short ScreenCenterX;
    short ScreenCenterY;

    int Subsectors[MAX_RENDERED_SUBSECTORS];
    int NumSubsectors;
} renderinfo_t;

// A single, renderable seg. This gets built up and
// passed around through the pipeline to render a seg
typedef struct
{
    seg_t* seg;
    sidedef_t* front;
    sidedef_t* back;
    vector2_t start, end;
    vector2_t vStart, vEnd;
    vector2_t vSeg, negNorm;
    float dist, length;
    int uStart;
} renderseg_t;

typedef struct
{
    int screenX;
    float dhMax;
    float z;
} rastercolumn_t;

typedef struct
{
    byte_t* flat;
    float dh;
    BOOL isFloor;
} renderflat_t;

static renderinfo_t g_renderInfo = {0};

//
// Private helper method declarations
//

//
// To make the pipeline more easily debuggable, we use function pointers
// which can be replaced with debug versions.
//

// Render a single subsector
typedef void (*pfnRenderSubsector)(subsector_t* subsector);

// Renders a single seg. This seg is not assumed to be clipped or even visible. pfnRenderSeg will do the culling and clipping
typedef void (*pfnRenderSeg)(seg_t* seg);

// Clips the renderseg in-place, returning TRUE if at least some part of the seg is still visible. FALSE return means the entire thing should be culled.
typedef BOOL (*pfnClipSeg)(renderseg_t* seg);

// Rasterizes a single, fully clipped and horizontally visible renderseg to the frame buffer. As each column gets rasterized, vertical visiblity will be checked.
typedef void (*pfnRasterizeSeg)(renderseg_t* seg);

// Rasterizes a textured column of pixels to the screen, clipping as needed.
typedef void (*pfnRasterizeTexturedColumn)(rastercolumn_t* column, float dhTop, float dhBottom, texture_t* texture, int u, float vStart, BOOL wrap);

// Rasterizes a single column of a single patch. The column is expected to be fully clipped to the screen bounds, but this method will handle wrapping.
typedef void (*pfnRasterizePatchColumn)(rastercolumn_t* column, int yStart, int yEnd, byte_t* patchColumn, float vStart, int textureHeight, int patchHeight, int yOffset, float dyOverDv, BOOL wrap);

// Rasterizes a single column of pixels, sourcing from a flat. The column is expected to be fully clipped to the screen bounds, but this method will handle wrapping.
typedef void (*pfnRasterizeFlatColumn)(rastercolumn_t* column, renderflat_t* flat);

static pfnRenderSubsector           g_pfnRenderSubsector;
static pfnRenderSeg                 g_pfnRenderSeg;
static pfnClipSeg                   g_pfnClipSeg;
static pfnRasterizeSeg              g_pfnRasterizeSeg;
static pfnRasterizeTexturedColumn   g_pfnRasterizeTexturedColumn;
static pfnRasterizePatchColumn      g_pfnRasterizePatchColumn;
static pfnRasterizeFlatColumn       g_pfnRasterizeFlatColumn;

//
// Implementations of the various stages of the pipeline
//

static void RenderSubsector(subsector_t* subsector);
static void RenderSeg(seg_t* seg);
static BOOL ClipSeg(renderseg_t* seg);
static void RasterizeSeg(renderseg_t* seg);
static void RasterizeTexturedColumn(rastercolumn_t* column, float dhTop, float dhBottom, texture_t* texture, int u, float vStart, BOOL wrap);
static void RasterizePatchColumn(rastercolumn_t* column, int yStart, int yEnd, byte_t* patchColumn, float vStart, int textureHeight, int patchHeight, int yOffset, float dyOverDv, BOOL wrap);
static void RasterizeFlatColumn(rastercolumn_t* column, renderflat_t* flat);

static void DebugRasterizeTexturedColumn(rastercolumn_t* column, float dhTop, float dhBottom, texture_t* texture, int u, float vStart, BOOL wrap);
static void DebugRasterizePatchColumn(rastercolumn_t* column, int yStart, int yEnd, byte_t* patchColumn, float vStart, int textureHeight, int patchHeight, int yOffset, float dyOverDv, BOOL wrap);
static void DebugRasterizeFlatColumn(rastercolumn_t* column, renderflat_t* flat);


//
// Public Renderer Methods
//

BOOL RendererInitialize()
{
    g_pfnRenderSubsector            = RenderSubsector;
    g_pfnRenderSeg                  = RenderSeg;
    g_pfnClipSeg                    = ClipSeg;
    g_pfnRasterizeSeg               = RasterizeSeg;

    g_pfnRasterizeTexturedColumn    = RasterizeTexturedColumn;
    g_pfnRasterizePatchColumn       = RasterizePatchColumn;
    g_pfnRasterizeFlatColumn        = RasterizeFlatColumn;

    //g_pfnRasterizeTexturedColumn    = DebugRasterizeTexturedColumn;
    //g_pfnRasterizePatchColumn       = DebugRasterizePatchColumn;
    //g_pfnRasterizeFlatColumn        = DebugRasterizeFlatColumn;

    g_renderInfo.Fov = TORADIANS(90);
    g_renderInfo.HalfFov = 0.5f * g_renderInfo.Fov;

    return TRUE;
}

void RendererUninitialize()
{
}

void Render3DView()
{
    int i;
    RECT viewport;
    GraphicsGetViewport(&viewport);

    g_renderInfo.ScreenLeft = g_viewport.Left;
    g_renderInfo.ScreenTop = g_viewport.Top;

    g_renderInfo.ScreenWidth = g_viewport.Width;
    g_renderInfo.ScreenHeight = g_viewport.Height;

    g_renderInfo.ScreenCenterX = g_renderInfo.ScreenWidth / 2;
    g_renderInfo.ScreenCenterY = g_renderInfo.ScreenHeight / 2;

    g_renderInfo.zPlane = (float)((g_renderInfo.ScreenWidth / 2) / tan(g_renderInfo.Fov / 2));

    g_renderInfo.Position = PlayerGetPosition();
    g_renderInfo.Direction = PlayerGetDirection();
    g_renderInfo.Right = Vector2Perp(g_renderInfo.Direction);
    g_renderInfo.ScaledForward = Vector2Scale(g_renderInfo.Direction, g_renderInfo.zPlane);

    // Compute FoV left and right vectors
    g_renderInfo.FovLeft = Vector2Rotate(g_renderInfo.Direction, g_renderInfo.HalfFov);
    g_renderInfo.PerpFovLeft = Vector2Perp(g_renderInfo.FovLeft);

    g_renderInfo.FovRight = Vector2Rotate(g_renderInfo.Direction, -g_renderInfo.HalfFov);
    g_renderInfo.PerpFovRight = Vector2Perp(g_renderInfo.FovRight);

    // DEBUG:
    {
        int playerSubsector = BSPFindSubsector(PlayerGetPosition());
        subsector_t* subsec = &g_renderer.subsectors[playerSubsector];
        seg_t* seg = &g_renderer.segs[subsec->FirstSeg];
        sector_t* sector = seg->side->sector;
        g_renderInfo.Height = sector->floorHeight + 41;
    }

    assert(g_map.Nodes);

    g_renderInfo.NumSubsectors = 0;

    // traverse the BSP
    BSPTraverseForRendering(PlayerGetPosition(), g_renderInfo.FovLeft, g_renderInfo.FovRight, g_renderInfo.Subsectors, ARRAYSIZE(g_renderInfo.Subsectors), &g_renderInfo.NumSubsectors);

    GraphicsClearDepthBuffer();

    // walk the subsectors from front to back
    //for (i = 0; i < 1; ++i)
    for (i = 0; i < g_renderInfo.NumSubsectors; ++i)
    {
        g_pfnRenderSubsector(&g_renderer.subsectors[g_renderInfo.Subsectors[i]]);
    }
}


//
// private helpers
//

void RenderSubsector(subsector_t* subsector)
{
    int i, segIndex = subsector->FirstSeg;

    for (i = 0; i < subsector->NumSegs; ++i)
    {
        g_pfnRenderSeg(&g_renderer.segs[segIndex++]);
    }
}

void RenderSeg(seg_t* seg)
{
    // start building out a renderseg
    renderseg_t renderSeg = {0};

    renderSeg.seg = seg;
    renderSeg.start = seg->start;
    renderSeg.end = seg->end;

    // get reverse normal (pointing away, we'll need this for clipping too).
    renderSeg.negNorm = Vector2Normalize(Vector2Perp(Vector2Subtract(renderSeg.start, renderSeg.end)));

    // find distance along that normal to position. This tells us how far from the seg we are.
    // if we are behind the seg (negative distance along negNorm) than we can't possibly
    // see the seg
    renderSeg.vStart = Vector2Subtract(renderSeg.start, g_renderInfo.Position);
    renderSeg.dist = Vector2Dot(renderSeg.vStart, renderSeg.negNorm);

    // we're behind the segment, nothing to draw
    if (renderSeg.dist < 0)
    {
        return;
    }

    // Before we can continue to clipping, we need to compute a few more values
    renderSeg.vEnd = Vector2Subtract(renderSeg.end, g_renderInfo.Position);
    renderSeg.vSeg = Vector2Normalize(Vector2Subtract(renderSeg.end, renderSeg.start));
    renderSeg.uStart = 0;

    // segment clipping will return FALSE if the whole seg should be culled
    if (!g_pfnClipSeg(&renderSeg))
    {
        return;
    }

    // we now know we have some portion of the segment visible, and the seg has been clipped
    // into range. Time to rasterize it
    g_pfnRasterizeSeg(&renderSeg);
}

BOOL ClipSeg(renderseg_t* seg)
{
    float dot, r;
    vector2_t pt;

    // does start need to be clipped against left edge of FoV?
    if (Vector2Dot(seg->vStart, g_renderInfo.PerpFovLeft) < 0)
    {
        dot = Vector2Dot(g_renderInfo.FovLeft, seg->negNorm);
        if (dot <= 0)
        {
            // this means the seg is parallel or facing away from the fov ray. Since we've already culled
            // facing away, and we know that start is ouside the fov, parallel means cull entirely
            return FALSE;
        }

        // distance along ray to the intersection with the seg
        r = seg->dist / dot;
        pt = Vector2Add(g_renderInfo.Position, Vector2Scale(g_renderInfo.FovLeft, r));
        seg->uStart += (int)Vector2Dot(Vector2Subtract(pt, seg->start), seg->vSeg);
        seg->start = pt;
        seg->vStart = Vector2Subtract(pt, g_renderInfo.Position);
    }

    // does end need to be clipped against right edge of FoV?
    if (Vector2Dot(seg->vEnd, g_renderInfo.PerpFovRight) > 0)
    {
        dot = Vector2Dot(g_renderInfo.FovRight, seg->negNorm);
        if (dot <= 0)
        {
            // this means the seg is parallel or facing away from the fov ray. Since we've already culled
            // facing away, and we know that end is ouside the fov, parallel means cull entirely
            return FALSE;
        }

        // distance along ray to the intersection with the seg
        r = seg->dist / dot;
        pt = Vector2Add(g_renderInfo.Position, Vector2Scale(g_renderInfo.FovRight, r));
        seg->end = pt;
        seg->vEnd = Vector2Subtract(pt, g_renderInfo.Position);
    }

    // update values
    seg->vSeg = Vector2Subtract(seg->end, seg->start);
    seg->length = Vector2Length(seg->vSeg);
    seg->vSeg = Vector2Normalize(seg->vSeg);

    return TRUE;
}

void RasterizeSeg(renderseg_t* seg)
{
    linedef_t* line;
    sector_t* front;
    sector_t* back = NULL;
    texture_t* topTex = NULL;
    texture_t* midTex = NULL;
    texture_t* bottomTex = NULL;
    renderflat_t ceiling;
    renderflat_t floor;
    rastercolumn_t column;

    BOOL hasAnyTex;

    int x, xStart, xEnd, u;
    float dhTop, dhBottom, dhMidTop, dhMidBottom, dhMidTopPegged;
    float topPeg, midPeg, bottomPeg;
    float dot, r, clampedAcos, distAlongSeg;
    vector2_t pt, ray;

    line = seg->seg->line;

    if (!line->leftSide)
    {
        // one-sided seg, load basic info
        seg->front = line->rightSide;
        front = line->rightSide->sector;
        midTex = (texture_t*)ResourceManagerDataFromHandle(resource_type_texture, seg->front->midTexture);
        assert(midTex);
    }
    else
    {
        // two-sided seg. Load basic + extra info
        seg->front = seg->seg->side;
        seg->back = line->rightSide == seg->seg->side ? line->leftSide : line->rightSide;
        front = seg->front->sector;
        back = seg->back->sector;

        if (front->ceilingHeight > back->ceilingHeight && seg->front->upperTexture != INVALID_RESOURCE)
        {
            topTex = (texture_t*)ResourceManagerDataFromHandle(resource_type_texture, seg->front->upperTexture);
        }
        if (seg->front->midTexture != INVALID_RESOURCE)
        {
            midTex = (texture_t*)ResourceManagerDataFromHandle(resource_type_texture, seg->front->midTexture);
        }
        if (front->floorHeight < back->floorHeight && seg->front->lowerTexture != INVALID_RESOURCE)
        {
            bottomTex = (texture_t*)ResourceManagerDataFromHandle(resource_type_texture, seg->front->lowerTexture);
        }
    }

    hasAnyTex = topTex || midTex || bottomTex;

    seg->uStart += seg->front->xOffset;

    dhTop = front->ceilingHeight - g_renderInfo.Height;
    dhBottom = front->floorHeight - g_renderInfo.Height;
    dhMidTop = topTex ? back->ceilingHeight - g_renderInfo.Height : dhTop;
    dhMidBottom = bottomTex ? back->floorHeight - g_renderInfo.Height : dhBottom;

    ceiling.dh = dhTop;
    ceiling.flat = (byte_t*)ResourceManagerDataFromHandle(resource_type_flat, front->ceilingFlat);
    ceiling.isFloor = FALSE;

    floor.dh = dhBottom;
    floor.flat = (byte_t*)ResourceManagerDataFromHandle(resource_type_flat, front->floorFlat);
    floor.isFloor = TRUE;

    clampedAcos = (float)acos(Vector2Dot(Vector2Normalize(seg->vStart), g_renderInfo.Direction));
    clampedAcos = min(max(clampedAcos, -1), 1);
    xStart = (int)(tan(clampedAcos) * g_renderInfo.zPlane) * (Vector2Dot(g_renderInfo.Right, seg->vStart) < 0 ? -1 : 1);

    clampedAcos = (float)acos(Vector2Dot(Vector2Normalize(seg->vEnd), g_renderInfo.Direction));
    clampedAcos = min(max(clampedAcos, -1), 1);
    xEnd = (int)(tan(clampedAcos) * g_renderInfo.zPlane) * (Vector2Dot(g_renderInfo.Right, seg->vEnd) < 0 ? -1 : 1);

    if (xStart > xEnd)
    {
        return;
    }

    // compute texture 'pegging' offset
    topPeg = 0;
    midPeg = 0;
    bottomPeg = 0;

    // if the line does NOT have the unpegged flag, that means it should align
    // the bottom of it's top texture to the midTop location
    if (topTex && (line->flags & LINE_UPPERTEX_UNPEGGED) == 0)
    {
        topPeg = topTex->Height - (dhTop - dhMidTop);
    }

    // if the line does have the unpegged flag, that means it should align
    // the bottom of it's bottom texture to the floor location
    if (bottomTex && (line->flags & LINE_LOWERTEX_UNPEGGED) != 0)
    {
        bottomPeg = bottomTex->Height - (dhMidBottom - dhBottom);
    }

    dhMidTopPegged = dhMidTop;
    // If a mid texture has a lower unpegged then it should be bottom aligned 
    // very common on door jams and transparent twosided textures
    if (midTex && (line->flags & LINE_LOWERTEX_UNPEGGED) != 0)
    {
        // two sided mid tex is transparent, always lower pegged, and no wrap
        if (seg->back)
        {
            dhMidTopPegged = min(dhMidTop, dhMidBottom + midTex->Height);
        }
        else
        {
            midPeg = midTex->Height - (dhMidTop - dhMidBottom);
            midPeg = midPeg - (int)(midPeg / midTex->Height) * midTex->Height;
            if (midPeg < 0)
            {
                midPeg += midTex->Height;
            }
        }
    }

    for (x = xStart; x <= xEnd; ++x)
    {
        column.screenX = g_renderInfo.ScreenCenterX + x;

        ray = Vector2Normalize(Vector2Add(g_renderInfo.ScaledForward, Vector2Scale(g_renderInfo.Right, (float)x)));
        dot = Vector2Dot(ray, seg->negNorm);
        if (dot < 0.0001f)
        {
            continue;
        }

        // distance along ray to the intersection with the seg
        r = seg->dist / dot;
        pt = Vector2Scale(ray, r);
        column.z = Vector2Dot(pt, g_renderInfo.Direction);
        column.dhMax = g_renderInfo.ScreenCenterY * column.z / g_renderInfo.zPlane;

        if (hasAnyTex)
        {
            pt = Vector2Add(g_renderInfo.Position, pt);
            distAlongSeg = Vector2Dot(Vector2Subtract(pt, seg->start), seg->vSeg);

            if (distAlongSeg < -0.001f || distAlongSeg > seg->length + 0.001f)
            {
                continue;
            }

            distAlongSeg = min(max(distAlongSeg, 0.0f), seg->length);

            u = seg->uStart + (int)distAlongSeg;

            if (topTex)
            {
                g_pfnRasterizeTexturedColumn(&column, dhTop, dhMidTop, topTex, u, topPeg + seg->front->yOffset, TRUE);
            }

            if (bottomTex)
            {
                g_pfnRasterizeTexturedColumn(&column, dhMidBottom, dhBottom, bottomTex, u, bottomPeg + seg->front->yOffset, TRUE);
            }

            if (midTex)
            {
                g_pfnRasterizeTexturedColumn(&column, dhMidTopPegged, dhMidBottom, midTex, u, midPeg + seg->front->yOffset, !seg->back);
            }
        }

        g_pfnRasterizeFlatColumn(&column, &ceiling);
        g_pfnRasterizeFlatColumn(&column, &floor);
    }
}

void RasterizeTexturedColumn(rastercolumn_t* column, float dhTop, float dhBottom, texture_t* texture, int u, float vStart, BOOL wrap)
{
    int y0, y1, i, count = texture->NumSprites;
    float height = dhTop - dhBottom;
    texturesprite_t* texSprite;
    sprite_t* sprite;

    if ((vStart < 0 || vStart > texture->Height) && !wrap)
    {
        return;
    }

    if (dhBottom > column->dhMax || dhTop < -column->dhMax)
    {
        return;
    }

    if (dhTop > column->dhMax)
    {
        vStart += (dhTop - column->dhMax);
        dhTop = column->dhMax + 1;
    }

    if (dhBottom < -column->dhMax)
    {
        dhBottom = -column->dhMax - 1;
    }

    y0 = g_renderInfo.ScreenCenterY - (int)(dhTop * g_renderInfo.zPlane / column->z);
    y1 = g_renderInfo.ScreenCenterY - (int)(dhBottom * g_renderInfo.zPlane / column->z);

    y0 = min(max(y0, 0), g_renderInfo.ScreenHeight);
    y1 = min(max(y1, 0), g_renderInfo.ScreenHeight);

    if (y0 >= y1 || dhTop <= dhBottom)
    {
        return;
    }

    // wrap u
    u = u % texture->Width;
    if (u < 0)
    {
        u += texture->Width;
    }

    // wrap v
    vStart = vStart - (int)(vStart / texture->Height) * texture->Height;
    if (vStart < 0)
    {
        vStart += texture->Height;
    }

    for (i = 0; i < count; ++i)
    {
        texSprite = &texture->Sprites[i];

        // determine if the patch could possibly overlap the u
        if (u >= texSprite->xOffset && vStart + height >= texSprite->yOffset)
        {
            sprite = (sprite_t*)ResourceManagerDataFromHandle(resource_type_sprite, texSprite->Sprite);

            // verify that the patch does in fact overlap
            if (u < texSprite->xOffset + sprite->Width &&
                vStart < texSprite->yOffset + sprite->Height)
            {
                // pull the column of the patch at this u, and rasterize it
                byte_t* patchColumn = &((byte_t*)sprite)[sprite->ColumnOffsets[u - texSprite->xOffset]];

                g_pfnRasterizePatchColumn(column, y0, y1, patchColumn, vStart, texture->Height, sprite->Height, texSprite->yOffset, (y1 - y0) / (dhTop - dhBottom), wrap);
            }
        }
    }
}

void RasterizePatchColumn(rastercolumn_t* column, int yStart, int yEnd, byte_t* patchColumn, float vStart, int textureHeight, int patchHeight, int yOffset, float dyOverDv, BOOL wrap)
{
    int y, sx = g_renderInfo.ScreenLeft + column->screenX, sy;
    byte_t rowStart, numPixels;
    byte_t* startColumn = patchColumn;
    int v;
    float vTotal, dvOverDy = 1.0f / dyOverDv;//(int)((1 << 16) / dyOverDv);

    //vTotal = (vStart << 16);
    vTotal = vStart;

    // find start of the first post of pixels in this patch
    rowStart = *patchColumn;
    if (rowStart == 255)
    {
        // this is an empty patch column, nothing to draw...
        return;
    }
    ++patchColumn;

    // number of pixels in this post
    numPixels = *patchColumn;
    ++patchColumn;
    ++patchColumn; // padding

    for (y = yStart; y < yEnd; ++y)
    {
        sy = g_renderInfo.ScreenTop + y;
        if (column->z <= RasterizeGetDepthAtPixel(sx, sy))
        {
            // wrap into texture space
            //v = (vTotal >> 16);
            float fv = vTotal - (int)vTotal;
            //v = (int)(fv < 0.2f ? vTotal - 1 : (fv > 0.8f ? vTotal + 1 : vTotal));
            v = (int)(vTotal);
            //v = (int)(vStart + (y - yStart) * dvOverDy + 0.5f);

            if (v >= textureHeight && !wrap)
            {
                return;
            }

            v = v % textureHeight;
            if (v < 0)
            {
                v += textureHeight;
            }

            if (v >= yOffset && v < yOffset + patchHeight)
            {
                v -= yOffset;

                // if v is < rowStart, then we haven't reached drawable pixels yet...
                if (v >= rowStart)
                {
                    while (v >= rowStart + numPixels)
                    {
                        // we've passed the current post of pixels in the patch. Advance the patch
                        patchColumn += numPixels + 1; // there's a dummy byte on the end of each post...
                        rowStart = *patchColumn;
                        if (rowStart == 255)
                        {
                            // end of patch
                            if (!wrap)
                            {
                                return;
                            }

                            patchColumn = startColumn;
                            rowStart = *patchColumn;

                            ++patchColumn;
                            numPixels = *patchColumn;
                            ++patchColumn;
                            ++patchColumn; // padding
                            break;
                        }
                        ++patchColumn;
                        numPixels = *patchColumn;
                        ++patchColumn;
                        ++patchColumn; // padding
                    }

                    if (v >= rowStart && v < rowStart + numPixels)
                    {
                        RasterizePixelWithDepth(sx, sy, patchColumn[v - rowStart], column->z);
                    }
                }
            }
        }

        vTotal += dvOverDy;
    }
}

void RasterizeFlatColumn(rastercolumn_t* column, renderflat_t* flat)
{
    int y, yStart, yEnd, sx = g_renderInfo.ScreenLeft + column->screenX, sy;
    int u, v;
    float z, zFlat, numerator, numerator2;
    vector2_t sample;

    if (flat->isFloor && (flat->dh > 0 || flat->dh < -column->dhMax))
    {
        return;
    }
    else if (!flat->isFloor && (flat->dh < 0 || flat->dh > column->dhMax))
    {
        return;
    }

    yStart = g_renderInfo.ScreenCenterY - (int)(flat->dh * g_renderInfo.zPlane / column->z);
    if (flat->isFloor)
    {
        yEnd = g_renderInfo.ScreenHeight;
    }
    else
    {
        yEnd = yStart;
        yStart = 0;
    }

    yStart = min(max(yStart, 0), g_renderInfo.ScreenHeight);
    yEnd = min(max(yEnd, 0), g_renderInfo.ScreenHeight);

    numerator = (float)fabs(flat->dh) * g_renderInfo.zPlane;
    numerator2 = (column->screenX - g_renderInfo.ScreenCenterX) / g_renderInfo.zPlane;

    if (yStart >= yEnd)
    {
        return;
    }

    // flats are considered further in than the wall, so that they don't z fight
    z = column->z + 0.001f;

    for (y = yStart; y < yEnd; ++y)
    {
        sy = g_renderInfo.ScreenTop + y;
        if (z <= RasterizeGetDepthAtPixel(sx, sy))
        {
            zFlat = numerator / (float)fabs(y - g_renderInfo.ScreenCenterY);
            sample = Vector2Add(g_renderInfo.Position, Vector2Add(Vector2Scale(g_renderInfo.Direction, zFlat), Vector2Scale(g_renderInfo.Right, numerator2 * zFlat)));
            u = (int)sample.X % 64;
            v = (int)sample.Y % 64;
            if (u < 0) u += 64;
            if (v < 0) v += 64;
            RasterizePixelWithDepth(sx, sy, flat->flat[v * 64 + u], z);
        }
    }
}

//
// Debug methods
//

void DebugRasterizeTexturedColumn(rastercolumn_t* column, float dhTop, float dhBottom, texture_t* texture, int u, float vStart, BOOL wrap)
{
    int y0 = g_renderInfo.ScreenCenterY - (int)(dhTop * g_renderInfo.zPlane / column->z);
    int y1 = g_renderInfo.ScreenCenterY - (int)(dhBottom * g_renderInfo.zPlane / column->z);

    // clip to screen bounds
    y0 = min(max(y0, 0), g_renderInfo.ScreenHeight);
    y1 = min(max(y1, 0), g_renderInfo.ScreenHeight);

    DebugRasterizePatchColumn(column, y0, y1, NULL, vStart, 1, 1, 0, 1.0f, wrap);
}

void DebugRasterizePatchColumn(rastercolumn_t* column, int yStart, int yEnd, byte_t* patchColumn, float vStart, int textureHeight, int patchHeight, int yOffset, float dyOverDv, BOOL wrap)
{
    int y, sx = g_renderInfo.ScreenLeft + column->screenX, sy;
    textureHeight;
    patchHeight;
    yOffset;

    for (y = yStart; y < yEnd; ++y)
    {
        sy = g_renderInfo.ScreenTop + y;
        if (column->z <= RasterizeGetDepthAtPixel(sx, sy))
        {
            RasterizePixelWithDepth(sx, sy, 150, column->z);
        }
    }
}

void DebugRasterizeFlatColumn(rastercolumn_t* column, renderflat_t* flat)
{
    int y, yStart, yEnd, sx = g_renderInfo.ScreenLeft + column->screenX, sy;

    if (flat->isFloor && (flat->dh > 0 || flat->dh < -column->dhMax))
    {
        return;
    }
    else if (!flat->isFloor && (flat->dh < 0 || flat->dh > column->dhMax))
    {
        return;
    }

    yStart = g_renderInfo.ScreenCenterY - (int)(flat->dh * g_renderInfo.zPlane / column->z);
    if (flat->isFloor)
    {
        yEnd = g_renderInfo.ScreenHeight;
    }
    else
    {
        yEnd = yStart;
        yStart = 0;
    }

    yStart = min(max(yStart, 0), g_renderInfo.ScreenHeight);
    yEnd = min(max(yEnd, 0), g_renderInfo.ScreenHeight);

    if (yStart >= yEnd)
    {
        return;
    }

    for (y = yStart; y < yEnd; ++y)
    {
        sy = g_renderInfo.ScreenTop + y;
        if (column->z <= RasterizeGetDepthAtPixel(sx, sy))
        {
            RasterizePixelWithDepth(sx, sy, 200, column->z);
        }
    }
}

#endif

#endif