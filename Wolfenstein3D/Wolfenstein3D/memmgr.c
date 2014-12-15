#include "common.h"
#include <assert.h>

typedef struct
{
    UINT    NumBytes;
    BYTE    RefCount;
    PVOID   Data;
} ALLOCATION, *PALLOCATION;

#define MAX_ALLOCS_PER_RESOURCE 5000
static ALLOCATION g_allocs[RESOURCE_TYPE_MAX][MAX_ALLOCS_PER_RESOURCE] = {0};
static UINT g_nextFree[RESOURCE_TYPE_MAX] = {0};
static UINT g_memUsage[RESOURCE_TYPE_MAX] = {0};

/* HALLOC format
    High 4 bits = resourceType
    Next 28 bits = index
*/

/**************************************************************
    Internal helper method declarations
***************************************************************/
BYTE GetResourceType(HALLOC h);
UINT GetIndex(HALLOC h);
HALLOC BuildHandle(BYTE resourceType, UINT index);
VOID InitializeAlloc(PALLOCATION alloc, UINT numBytes);
VOID FreeAlloc(PALLOCATION alloc);

/**************************************************************
    Public Memory Manager API
***************************************************************/
HALLOC MemAlloc(BYTE resourceType, UINT numBytes)
{
    UINT nextFree;
    PALLOCATION alloc;
    UINT i;

    assert(resourceType < RESOURCE_TYPE_MAX);
    if (resourceType >= RESOURCE_TYPE_MAX)
        return INVALID_HALLOC_VALUE;

    nextFree = g_nextFree[resourceType];

    assert(nextFree < MAX_ALLOCS_PER_RESOURCE);
    if (nextFree >= MAX_ALLOCS_PER_RESOURCE)
        return INVALID_HALLOC_VALUE;

    alloc = &g_allocs[resourceType][nextFree];
    InitializeAlloc(alloc, numBytes);

    assert(alloc->Data);
    if (!alloc->Data)
        return INVALID_HALLOC_VALUE;

    g_memUsage[resourceType] += alloc->NumBytes;

    for (i = nextFree + 1; i < MAX_ALLOCS_PER_RESOURCE; ++i)
    {
        if (g_allocs[resourceType][i].Data == NULL)
            break;
    }
    g_nextFree[resourceType] = i;

    return BuildHandle(resourceType, nextFree);
}

VOID MemFree(HALLOC h)
{
    BYTE resourceType = GetResourceType(h);
    UINT index = GetIndex(h);
    PALLOCATION alloc;

    if (h == INVALID_HALLOC_VALUE)
        return;

    assert(resourceType < RESOURCE_TYPE_MAX);
    assert(index < MAX_ALLOCS_PER_RESOURCE);

    assert(!MemIsInUse(h));

    alloc = &g_allocs[resourceType][index];

    assert(alloc->Data);
    assert(alloc->RefCount == 0);

    assert(g_memUsage[resourceType] >= alloc->NumBytes);
    g_memUsage[resourceType] -= alloc->NumBytes;

    FreeAlloc(alloc);

    if (index < g_nextFree[resourceType])
    {
        g_nextFree[resourceType] = index;
    }
}

BOOL MemIsResource(HALLOC h, BYTE resourceType)
{
    return GetResourceType(h) == resourceType;
}

PVOID MemLock(HALLOC h)
{
    BYTE resourceType = GetResourceType(h);
    UINT index = GetIndex(h);
    PALLOCATION alloc;

    assert(h != INVALID_HALLOC_VALUE);
    if (h == INVALID_HALLOC_VALUE)
        return NULL;

    assert(resourceType < RESOURCE_TYPE_MAX);
    assert(index < MAX_ALLOCS_PER_RESOURCE);

    alloc = &g_allocs[resourceType][index];

    alloc->RefCount++;

    return alloc->Data;
}

VOID MemUnlock(HALLOC h)
{
    BYTE resourceType = GetResourceType(h);
    UINT index = GetIndex(h);
    PALLOCATION alloc;

    assert(h != INVALID_HALLOC_VALUE);
    if (h == INVALID_HALLOC_VALUE)
        return;

    assert(resourceType < RESOURCE_TYPE_MAX);
    assert(index < MAX_ALLOCS_PER_RESOURCE);

    alloc = &g_allocs[resourceType][index];

    assert(alloc->RefCount > 0);
    alloc->RefCount = max((USHORT)alloc->RefCount - 1, 0);
}

BOOL MemIsInUse(HALLOC h)
{
    BYTE resourceType = GetResourceType(h);
    UINT index = GetIndex(h);
    PALLOCATION alloc;

    assert(h != INVALID_HALLOC_VALUE);
    if (h == INVALID_HALLOC_VALUE)
        return FALSE;

    assert(resourceType < RESOURCE_TYPE_MAX);
    assert(index < MAX_ALLOCS_PER_RESOURCE);

    alloc = &g_allocs[resourceType][index];

    return alloc->RefCount > 0;
}

UINT MemBytesUsedBy(BYTE resourceType)
{
    assert(resourceType < RESOURCE_TYPE_MAX);

    return g_memUsage[resourceType];
}

UINT MemTotalBytesUsed()
{
    UINT total = 0;
    UINT i;

    for (i = 0; i < RESOURCE_TYPE_MAX; ++i)
    {
        total += g_memUsage[i];
    }

    return total;
}

/**************************************************************
    Internal helper methods
***************************************************************/
BYTE GetResourceType(HALLOC h)
{
    return h >> 28;
}

UINT GetIndex(HALLOC h)
{
    return h & 0x0FFFFFFF;
}

HALLOC BuildHandle(BYTE resourceType, UINT index)
{
    return ((UINT)resourceType << 28) | index;
}

VOID InitializeAlloc(PALLOCATION alloc, UINT numBytes)
{
    alloc->RefCount = 0;
    alloc->NumBytes = numBytes;
    alloc->Data = malloc(numBytes);
}

VOID FreeAlloc(PALLOCATION alloc)
{
    free(alloc->Data);
    alloc->Data = NULL;
    alloc->NumBytes = 0;
    alloc->RefCount = 0;
}
