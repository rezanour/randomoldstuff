#include "common.h"
#include "mapp.h"
#include "rendererp.h"

//
// intermediate data we load from wad before converting into our
// own optimized runtime format in memory
//

typedef struct
{
    ushort_t NumVertexes;
    ushort_t NumLinedefs;
    ushort_t NumSidedefs;
    ushort_t NumSegs;
    ushort_t NumSectors;
    ushort_t NumSubSectors;

    vertex_t* Vertexes;
    maplinedef_t* Linedefs;
    mapsidedef_t* Sidedefs;
    mapseg_t* Segs;
    mapsector_t* Sectors;
    mapsubsector_t* SubSectors;
} mapdata_t;

static mapdata_t g_mapData = {0};
rendererp_t g_renderer = {0};

//
// private method declarations
//

static BOOL RendererpProcessData();


BOOL RendererpLoadMap(const char* lumpName)
{
    BOOL result = FALSE;

    // Ensure renderer isn't holding data
    RendererpDestroy();

    if (MapLoadLumpInto(lumpName, "VERTEXES", &g_mapData.Vertexes, sizeof(vertex_t), &g_mapData.NumVertexes) &&
        MapLoadLumpInto(lumpName, "LINEDEFS", &g_mapData.Linedefs, sizeof(maplinedef_t), &g_mapData.NumLinedefs) &&
        MapLoadLumpInto(lumpName, "SIDEDEFS", &g_mapData.Sidedefs, sizeof(mapsidedef_t), &g_mapData.NumSidedefs) &&
        MapLoadLumpInto(lumpName, "SEGS", &g_mapData.Segs, sizeof(mapseg_t), &g_mapData.NumSegs) &&
        MapLoadLumpInto(lumpName, "SECTORS", &g_mapData.Sectors, sizeof(mapsector_t), &g_mapData.NumSectors) &&
        MapLoadLumpInto(lumpName, "SSECTORS", &g_mapData.SubSectors, sizeof(mapsubsector_t), &g_mapData.NumSubSectors) &&
        MapLoadLumpInto(lumpName, "NODES", &g_map.Nodes, sizeof(node_t), &g_map.NumNodes))
    {
        result = RendererpProcessData();
    }

    // cleanup

    SAFEFREE(g_mapData.SubSectors);
    SAFEFREE(g_mapData.Sectors);
    SAFEFREE(g_mapData.Segs);
    SAFEFREE(g_mapData.Sidedefs);
    SAFEFREE(g_mapData.Linedefs);
    SAFEFREE(g_mapData.Vertexes);

    return result;
}

void RendererpDestroy()
{
    SAFEFREE(g_renderer.linedefs);
    SAFEFREE(g_renderer.sidedefs);
    SAFEFREE(g_renderer.segs);
    SAFEFREE(g_renderer.subsectors);
    SAFEFREE(g_renderer.sectors);

    ZeroMemory(&g_renderer, sizeof(g_renderer));
}


//
// private methods
//

BOOL RendererpProcessData()
{
    int i;

    // sectors

    g_renderer.sectors = (sector_t*)MemoryAlloc("Renderer sectors", sizeof(sector_t) * g_mapData.NumSectors);
    if (!g_renderer.sectors)
    {
        DebugOut("Failed to allocate sectors list in renderer");
        return FALSE;
    }
    g_renderer.numSectors = g_mapData.NumSectors;
    for (i = 0; i < g_mapData.NumSectors; ++ i)
    {
        mapsector_t* mapsector = &g_mapData.Sectors[i];
        sector_t* sector = &g_renderer.sectors[i];

        sector->type = mapsector->Type;
        sector->tagNumber = mapsector->TagNumber;
        sector->lightLevel = mapsector->LightLevel;
        sector->floorHeight = mapsector->FloorHeight;
        sector->ceilingHeight = mapsector->CeilingHeight;
        sector->floorFlat = MapGetFlat(mapsector->FloorTexture);
        sector->ceilingFlat = MapGetFlat(mapsector->CeilingTexture);
    }

    // sidedefs

    g_renderer.sidedefs = (sidedef_t*)MemoryAlloc("Renderer sidedefs", sizeof(sidedef_t) * g_mapData.NumSidedefs);
    if (!g_renderer.sidedefs)
    {
        DebugOut("Failed to allocate sidedef list in renderer");
        return FALSE;
    }
    g_renderer.numSidedefs = g_mapData.NumSidedefs;
    for (i = 0; i < g_mapData.NumSidedefs; ++ i)
    {
        mapsidedef_t* mapside = &g_mapData.Sidedefs[i];
        sidedef_t* side = &g_renderer.sidedefs[i];

        side->xOffset = mapside->xOffset;
        side->yOffset = mapside->yOffset;
        side->sector = &g_renderer.sectors[mapside->Sector];
        side->upperTexture = WadCompareDoomString(mapside->UpperTexture, "-") ? INVALID_RESOURCE : MapGetTexture(mapside->UpperTexture);
        side->midTexture = WadCompareDoomString(mapside->MiddleTexture, "-") ? INVALID_RESOURCE : MapGetTexture(mapside->MiddleTexture);
        side->lowerTexture = WadCompareDoomString(mapside->LowerTexture, "-") ? INVALID_RESOURCE : MapGetTexture(mapside->LowerTexture);
    }

    // lines

    g_renderer.linedefs = (linedef_t*)MemoryAlloc("Renderer linedefs", sizeof(linedef_t) * g_mapData.NumLinedefs);
    if (!g_renderer.linedefs)
    {
        DebugOut("Failed to allocate linedef list in renderer");
        return FALSE;
    }
    g_renderer.numLinedefs = g_mapData.NumLinedefs;
    for (i = 0; i < g_mapData.NumLinedefs; ++ i)
    {
        // source and dest
        maplinedef_t* mapline = &g_mapData.Linedefs[i];
        linedef_t* line = &g_renderer.linedefs[i];
        vertex_t* start = &g_mapData.Vertexes[mapline->StartVertex];
        vertex_t* end = &g_mapData.Vertexes[mapline->EndVertex];

        line->flags = mapline->Flags;
        line->special = mapline->Special;
        line->tag = mapline->Tag;
        line->start = Vector2(start->x, start->y);
        line->end = Vector2(end->x, end->y);
        line->rightSide = &g_renderer.sidedefs[mapline->RightSidedef];
        line->leftSide = mapline->LeftSidedef == -1 ? NULL : &g_renderer.sidedefs[mapline->LeftSidedef];
    }

    // segs

    g_renderer.segs = (seg_t*)MemoryAlloc("Renderer segs", sizeof(seg_t) * g_mapData.NumSegs);
    if (!g_renderer.segs)
    {
        DebugOut("Failed to allocate seg list in renderer");
        return FALSE;
    }
    g_renderer.numSegs = g_mapData.NumSegs;
    for (i = 0; i < g_mapData.NumSegs; ++ i)
    {
        mapseg_t* mapseg = &g_mapData.Segs[i];
        seg_t* seg = &g_renderer.segs[i];
        vertex_t* start = &g_mapData.Vertexes[mapseg->StartVertex];
        vertex_t* end = &g_mapData.Vertexes[mapseg->EndVertex];

        seg->start = Vector2(start->x, start->y);
        seg->end = Vector2(end->x, end->y);
        seg->line = &g_renderer.linedefs[mapseg->Linedef];
        seg->side = mapseg->Side == 0 ? seg->line->rightSide : seg->line->leftSide;
    }

    // subsectors

    g_renderer.subsectors = (subsector_t*)MemoryAlloc("Renderer subsectors", sizeof(subsector_t) * g_mapData.NumSubSectors);
    if (!g_renderer.subsectors)
    {
        DebugOut("Failed to allocate subsector list in renderer");
        return FALSE;
    }
    g_renderer.numSubsectors = g_mapData.NumSubSectors;
    for (i = 0; i < g_mapData.NumSubSectors; ++ i)
    {
        mapsubsector_t* mapsubsector = &g_mapData.SubSectors[i];
        subsector_t* subsector = &g_renderer.subsectors[i];

        subsector->FirstSeg = mapsubsector->FirstSeg;
        subsector->NumSegs = mapsubsector->NumSegs;
    }

    return TRUE;
}