#include "directgraphics.h"
#include "internal.h"

#include <malloc.h>
#include <string.h>

#define MAX_TAG_LENGTH  32

typedef struct allocation_s
{
    char tag[MAX_TAG_LENGTH];
    uint_t size;
    struct allocation_s* prev;
    struct allocation_s* next;
} allocation_t;

static allocation_t* g_head = NULL;


//
// private memory method declarations
//

static allocation_t* DGNewAllocation(const char* tag, uint_t numBytes);
static void DGDeleteAllocation(allocation_t* node);


//
// public memory methods
//

void* DGAlloc(const char* const tag, uint_t numBytes)
{
    allocation_t* node = DGNewAllocation(tag, numBytes);
    if (node)
    {
        if (g_head)
        {
            node->next = g_head;
            g_head->prev = node;
            g_head = node;
        }
        else
        {
            g_head = node;
        }

        return (byte_t*)node + sizeof(allocation_t);
    }

    return NULL;
}

void DGFree(void* p)
{
    assert(p);
    if (p)
    {
        allocation_t* node = (allocation_t*)((byte_t*)p - sizeof(allocation_t));
        if (node->prev)
        {
            node->prev->next = node->next;
        }
        if (node->next)
        {
            node->next->prev = node->prev;
        }

        if (node == g_head)
        {
            g_head = node->next;
        }

        DGDeleteAllocation(node);
    }
}


//
// internal memory manager methods
//

bool_t DGInitializeMemoryManager()
{
    DGDebugOut("Initializing DirectGraphics MemoryManager.\n");

    return true;
}

void DGUninitializeMemoryManager(bool_t reportMemoryLeaks)
{
    allocation_t* node = g_head;
    bool_t foundLeaks = false;

    DGDebugOut("Uninitializing DirectGraphics MemoryManager.\n");

    while (node)
    {
        allocation_t* next = node->next;

        if (reportMemoryLeaks)
        {
            DGDebugOut("* Leak Detected: %d bytes (%s)\n", node->size, node->tag);
            foundLeaks = true;
        }

        DGDeleteAllocation(node);

        node = next;
    }

    assert(!foundLeaks);

    g_head = NULL;
}


//
// private memory methods
//

allocation_t* DGNewAllocation(const char* tag, uint_t numBytes)
{
    allocation_t* node = (allocation_t*)malloc(sizeof(allocation_t) + numBytes);
    if (node)
    {
        if (tag)
        {
            strncpy_s(node->tag, MAX_TAG_LENGTH, tag, _TRUNCATE);
        }
        else
        {
            node->tag[0] = '\0';
        }

        node->size = numBytes;
        node->prev = node->next = NULL;
    }

    return node;
}

void DGDeleteAllocation(allocation_t* node)
{
    free(node);
}
