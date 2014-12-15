#include "common.h"
#include "mapp.h"
#include "rendererp.h"
#include "things.h"

map_t g_map = {0};

//
// private helper method declarations
//
BOOL MapLoadLumpInto(const char* map, const char* lump, void* buffer, ushort_t sizeOfElement, ushort_t* count);

//
// public map methods
//

BOOL MapLoad(const char* name)
{
    thing_t* things;
    ushort_t numThings;

    assert(name);

    // Ensure we don't have any map data loaded
    MapDestroy();

    g_map.ResourceManager = ResourceManagerCreate();

    if (!RendererpLoadMap(name))
    {
        return FALSE;
    }

    // load all the basic direct to memory stuff
    if (MapLoadLumpInto(name, "THINGS", &things, sizeof(thing_t), &numThings))
    {
        int i;
        for (i = 0; i < numThings; ++i)
        {
            if (things[i].Type == THING_TYPE_PLAYER1_START)
            {
                PlayerSetPosition(Vector2(things[i].xPosition, things[i].yPosition));
                PlayerSetDirection(Vector2(1, 0));
                PlayerRotate(TORADIANS(things[i].Angle));
            }

            DebugOut("THING: %d at (%d, %d)", things[i].Type, things[i].xPosition, things[i].yPosition);
        }

        MemoryFree(things);
        return TRUE;
    }

    // failed
    MapDestroy();
    return FALSE;
}

void MapDestroy()
{
    RendererpDestroy();

    SAFEFREE(g_map.Nodes);

    if (g_map.ResourceManager)
    {
        ResourceManagerDestroy(g_map.ResourceManager);
        g_map.ResourceManager = NULL;
    }

    ZeroMemory(&g_map, sizeof(g_map));
}

void MapUpdate()
{
}

resource_t MapGetSound(const char* name)
{
    return ResourceManagerGet(g_map.ResourceManager, resource_type_soundbuffer, name);
}

//
// Private helper methods
//

BOOL MapLoadLumpInto(const char* map, const char* lump, void** buffer, ushort_t sizeOfElement, ushort_t* count)
{
    long size;

    if (!WadGetSubLump(map, lump, buffer, &size))
    {
        DebugOut("MapLoad: Failed to find %s for map: %s", lump, map);
        return FALSE;
    }

    *count = (ushort_t)(size / sizeOfElement);
    return TRUE;
}

resource_t MapGetFlat(const char* name)
{
    return ResourceManagerGet(g_map.ResourceManager, resource_type_flat, name);
}

resource_t MapGetSprite(const char* name)
{
    return ResourceManagerGet(g_map.ResourceManager, resource_type_sprite, name);
}

resource_t MapGetTexture(const char* name)
{
    return ResourceManagerGet(g_map.ResourceManager, resource_type_texture, name);
}


//
// Debug map methods
//

void DebugMapDraw(short xOffset, short yOffset, float scale)
{
    int x1, y1, x2, y2, i;
    int halfWidth = GraphicsGetScreenWidth() / 2;
    int halfHeight = GraphicsGetScreenHeight() / 2;

    // we center the map around the player
    int px = (int)((PlayerGetPosition().X + xOffset) * scale);
    int py = (int)((PlayerGetPosition().Y + yOffset) * scale);

    for (i = 0; i < g_renderer.numLinedefs; ++i)
    {
        linedef_t* line = &g_renderer.linedefs[i];

        x1 = (int)line->start.X;
        y1 = (int)line->start.Y;

        x2 = (int)line->end.X;
        y2 = (int)line->end.Y;

        x1 = (int)(x1 * scale) - px;
        x2 = (int)(x2 * scale) - px;
        y1 = (int)(y1 * scale) - py;
        y2 = (int)(y2 * scale) - py;

        RasterizeLine(halfWidth + x1, halfHeight - y1, halfWidth + x2, halfHeight - y2, 180);
    }

    // draw player arrow
    x1 = (int)(PlayerGetDirection().X * 5);
    y1 = (int)(PlayerGetDirection().Y * 5);

    // base line
    RasterizeLine(halfWidth, halfHeight, halfWidth + x1, halfHeight - y1, 145);
}
