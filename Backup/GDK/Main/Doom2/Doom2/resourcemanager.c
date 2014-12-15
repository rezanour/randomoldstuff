#include "common.h"
#include "graphicsp.h"
#include "images.h"
#include "audiop.h"

//
// resource_t handle breakdown
// ____________________________________________________________________
// |31| | | |27| | | |23| | | | | | | | | | | | | | | | | | | | | | |0|
// --------------------------------------------------------------------
// |        |        |
// |        |        +--- bits 0 - 23 (24 bits) store an index value into the appropriate table
// |        |
// |        +-- bits 24 - 27 (4 bits) store the type of resource (sprite, soundfx, etc...)
// |
// +-- bits 28 - 31 (4 bits) store the id of the owning resource manager.
//

#define INDEX_MASK      0x00FFFFFF
#define TYPE_MASK       0x0F000000
#define OWNERID_MASK    0xF0000000
#define MAX_INDEX       INDEX_MASK
#define MAX_TYPE        15
#define MAX_OWNERID     15
#define TYPE_SHIFT      24
#define OWNERID_SHIFT   28

typedef struct
{
    char Name[NAME_SIZE];
    resource_type_t Type;
    void* Data;
} resource_data_t;

typedef struct
{
    byte_t ID;
    arraylist_t Resources[resource_type_max]; // List object
} resourcemanager_private_t;

static byte_t g_nextID = 0;
static list_node_t* g_listHead = NULL;

//
// private method declarations
//
static uint_t IndexFromHandle(resource_t handle);
static byte_t TypeFromHandle(resource_t handle);
static byte_t OwnerIDFromHandle(resource_t handle);
static resource_t BuildHandle(byte_t ownerID, byte_t type, uint_t index);
static void DeleteResource(void* p);


//
// public resource manager methods
//
resourcemanager_t ResourceManagerCreate()
{
    int i;
    list_node_t* node;
    resourcemanager_private_t* rm;

    node = LinkedListCreateNode(sizeof(resourcemanager_private_t));
    if (!node)
    {
        DebugOut("ResourceManager: Failed to allocated node for resource manager creation.");
        return NULL;
    }

    rm = (resourcemanager_private_t*)LinkedListObjectFromNode(node);

    for (i = 0; i < ARRAYSIZE(rm->Resources); ++i)
    {
        rm->Resources[i] = ListCreate(DeleteResource);
        if (!rm->Resources[i])
        {
            // failed, unwind cleaning back up and return
            for (; i >= 0; --i)
            {
                ListDestroy(rm->Resources[i]);
            }

            LinkedListDeleteNode(node);
            return NULL;
        }
    }

    if (!g_listHead)
    {
        g_listHead = node;
    }
    else
    {
        LinkedListAddNode(g_listHead, node);
    }

    rm->ID = g_nextID++;

    if (g_nextID >= MAX_OWNERID)
    {
        g_nextID = 0;
    }

    return rm;
}

void ResourceManagerDestroy(resourcemanager_t resourceManager)
{
    int i;
    resourcemanager_private_t* rm = (resourcemanager_private_t*)resourceManager;
    list_node_t* node;

    assert(rm);

    node = LinkedListNodeFromObject(rm);

    // destroy the lists
    for (i = 0; i < ARRAYSIZE(rm->Resources); ++i)
    {
        ListDestroy(rm->Resources[i]);
    }

    if (g_listHead == node)
    {
        g_listHead = node->Next;
    }

    LinkedListRemoveNode(node);
    LinkedListDeleteNode(node);
}

resource_t ResourceManagerGet(resourcemanager_t resourceManager, resource_type_t type, const char* name)
{
    resourcemanager_private_t* rm = (resourcemanager_private_t*)resourceManager;
    arraylist_t list;
    resource_data_t* resource;
    uint_t i, count;

    assert(rm);
    assert(type >= 0 && type < resource_type_max);

    // do we already have the resource?
    list = rm->Resources[type];
    count = ListSize(list);
    for (i = 0; i < count; ++i)
    {
        resource = (resource_data_t*)ListGet(list, i);
        if (resource)
        {
            if (resource->Name && WadCompareDoomString(resource->Name, name))
            {
                // found it
                return BuildHandle(rm->ID, type, i);
            }
        }
    }

    // didn't find the item, so load it

    resource = (resource_data_t*)MemoryAlloc("resource data", sizeof(resource_data_t));
    if (!resource)
    {
        return INVALID_RESOURCE;
    }

    ZeroMemory(resource, sizeof(resource_data_t));

    WadCopyDoomString(resource->Name, name);
    resource->Type = type;

    switch (type)
    {
    case resource_type_flat:
        resource->Data = ImagesLoadFlat(name);
        if (!resource->Data)
        {
            DebugOut("Loading flat failed! %s", name);
            MemoryFree(resource);
            return INVALID_RESOURCE;
        }
        break;

    case resource_type_sprite:
        resource->Data = ImagesLoadSprite(name);
        if (!resource->Data)
        {
            DebugOut("Loading sprite failed! %s", name);
            MemoryFree(resource);
            return INVALID_RESOURCE;
        }
        break;

    case resource_type_texture:
        resource->Data = ImagesLoadTexture(name);
        if (!resource->Data)
        {
            DebugOut("Loading texture failed! %s", name);
            MemoryFree(resource);
            return INVALID_RESOURCE;
        }
        break;

    case resource_type_soundbuffer:
        resource->Data = AudioLoadSoundBuffer(name);
        if (!resource->Data)
        {
            DebugOut("Loading sound buffer failed! %s", name);
            MemoryFree(resource);
            return INVALID_RESOURCE;
        }
        break;

    default:
        assert(FALSE && "type not supported yet!");
        break;
    }

    ListAdd(list, resource);
    return BuildHandle(rm->ID, type, ListSize(list) - 1);
}

void* ResourceManagerDataFromHandle(resource_type_t type, resource_t handle)
{
    byte_t id;
    resourcemanager_private_t* rm;
    list_node_t* node;

    assert(handle != INVALID_RESOURCE);
    assert(type == TypeFromHandle(handle));

    id = OwnerIDFromHandle(handle);

    node = g_listHead;
    while (node)
    {
        rm = (resourcemanager_private_t*)LinkedListObjectFromNode(node);
        if (rm->ID == id)
        {
            arraylist_t list = rm->Resources[type];
            uint_t index = IndexFromHandle(handle);

            resource_data_t* resource;

            assert(index < ListSize(list));

            resource = (resource_data_t*)ListGet(list, index);
            if (resource)
            {
                return resource->Data;
            }

            // not found in the resource manager
            break;
        }
        node = node->Next;
    }

    return NULL;
}

//
// private methods
//
uint_t IndexFromHandle(resource_t handle)
{
    return handle & INDEX_MASK;
}

byte_t TypeFromHandle(resource_t handle)
{
    return (handle & TYPE_MASK) >> TYPE_SHIFT;
}

byte_t OwnerIDFromHandle(resource_t handle)
{
    return (handle & OWNERID_MASK) >> OWNERID_SHIFT;
}

resource_t BuildHandle(byte_t ownerID, byte_t type, uint_t index)
{
    assert(ownerID <= MAX_OWNERID && type <= MAX_TYPE && index <= MAX_INDEX);

    return (((uint_t)ownerID) << OWNERID_SHIFT) | (((uint_t)type) << TYPE_SHIFT) | index;
}

void DeleteResource(void* p)
{
    resource_data_t* resource = (resource_data_t*)p;

    if (resource)
    {
        if (resource->Data)
        {
            switch (resource->Type)
            {
            case resource_type_flat:
                ImagesFreeFlat((byte_t*)resource->Data);
                break;

            case resource_type_sprite:
                ImagesFreeSprite((sprite_t*)resource->Data);
                break;

            case resource_type_texture:
                ImagesFreeTexture((texture_t*)resource->Data);
                break;

            case resource_type_soundbuffer:
                AudioFreeSound((byte_t*)resource->Data);
                break;

            default:
                assert(FALSE && "deletion of this type not supported yet!");
                break;
            }
        }

        MemoryFree(resource);
    }
}
