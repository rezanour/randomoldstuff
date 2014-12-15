#include <malloc.h>

#include "common.h"

//
// ALLOCATIONS
//

#define MAX_TAG_SIZE    20

// an allocation block header. All data is contained in allocation blocks, which can be audited and leaks detected
typedef struct
{
    uint_t Size;
    char Tag[MAX_TAG_SIZE];
} allocation_t;

// We use the linked list for allocations because they are alloc/freed at random, and often
static list_node_t* g_allocationList = NULL;



//
// REF COUNTING
//

// the refcounted header. All refcounted objects will be prefixed with this header for tracking.
typedef struct
{
    char Name[NAME_SIZE];
    short RefCount;
} refcounted_t;

// We use the regular list for refcounted objects since we need to walk the list often
static arraylist_t g_refcountedList = NULL;



//
// private method declarations
//

static void* ObjectFromAlloc(allocation_t* alloc);
static allocation_t* AllocFromObject(void* p);

static allocation_t* NewAllocation(const char* tag, uint_t numBytes);
static void DeleteAllocation(allocation_t* alloc);


static void* ObjectFromRefCounted(refcounted_t* refcounted);
static refcounted_t* RefCountedFromObject(void* p);

static refcounted_t* NewRefCounted(const char* name, uint_t numBytes);
static void DeleteRefCounted(refcounted_t* refcounted);

// both of these methods addref if they return an object
static refcounted_t* FindRefCounted(const char* name);



//
// public memory manager methods
//

BOOL MemoryManagerInitialize()
{
    if (!g_refcountedList)
    {
        g_refcountedList = ListCreate(NULL);
    }

    return TRUE;
}

void MemoryManagerUninitialize()
{
    if (g_refcountedList)
    {
        ListDestroy(g_refcountedList);
        g_refcountedList = NULL;
    }
}

void* MemoryAlloc(const char* tag, uint_t numBytes)
{
    assert(tag);

#ifndef NDEBUG
    return ObjectFromAlloc(NewAllocation(tag, numBytes));
#else
    return malloc(numBytes);
#endif
}

void* MemoryAllocWithLength(const char* tag, uint_t numBytes)
{
    void* newMemory = NULL;

    assert(tag);

    // allocate caller specified size + sizeof(uint_t) to hold length prefixed size
#ifndef NDEBUG
    newMemory = ObjectFromAlloc(NewAllocation(tag, numBytes + sizeof(uint_t)));
#else
    newMemory = malloc(numBytes + sizeof(uint_t));
#endif

    // set caller specified size as the first bytes of memory
    *((uint_t*)newMemory) = numBytes;

    return newMemory;
}

void MemoryFree(void* p)
{
    assert(p);

#ifndef NDEBUG
    DeleteAllocation(AllocFromObject(p));
#else
    free(p);
#endif;
}

void* MemoryGetRefCountedLump(const char* name, long* size)
{
    refcounted_t* refcounted;

    assert(name);
    assert(size);

    *size = 0;

    refcounted = FindRefCounted(name);
    if (!refcounted)
    {
        // we don't have this lump yet, load it
        void* lump;
        long lumpSize;

        if (!WadGetLump(name, &lump, &lumpSize))
        {
            DebugOut("MemoryManager: Error loading lump from WAD: %s", name);
            return NULL;
        }

        refcounted = NewRefCounted(name, lumpSize);
        if (refcounted)
        {
            memcpy(ObjectFromRefCounted(refcounted), lump, lumpSize);
            *size = lumpSize;
        }

        MemoryFree(lump);
    }

    return ObjectFromRefCounted(refcounted);
}

void MemoryReleaseRefCounted(void* p)
{
    refcounted_t* refcounted = RefCountedFromObject(p);

    assert(refcounted);

    if (--refcounted->RefCount == 0)
    {
        DeleteRefCounted(refcounted);
    }
}


//
// private methods
//

void* ObjectFromAlloc(allocation_t* alloc)
{
    if (alloc)
    {
        return (byte_t*)alloc + sizeof(allocation_t);
    }

    return NULL;
}

allocation_t* AllocFromObject(void* p)
{
    if (p)
    {
        return (allocation_t*)((byte_t*)p - sizeof(allocation_t));
    }

    return NULL;
}

allocation_t* NewAllocation(const char* tag, uint_t numBytes)
{
    // we don't use the LinkedListCreate and Destroy methods because they use this same memory manager, which would get
    // recursive and stack overflow :)
    list_node_t* node;
    allocation_t* alloc;
    int len = min(strlen(tag) + 1, MAX_TAG_SIZE);

    node = (list_node_t*)malloc(sizeof(list_node_t) + sizeof(allocation_t) + numBytes);
    if (!node)
    {
        DebugOut("MemoryManager: Failed allocating %s", tag);
        return NULL;
    }

    DebugOut("MemoryManager: Allocated %s", tag);

    ZeroMemory(node, sizeof(list_node_t) + sizeof(allocation_t) + numBytes);

    alloc = (allocation_t*)LinkedListObjectFromNode(node);
    alloc->Size = numBytes;
    strncpy_s(alloc->Tag, ARRAYSIZE(alloc->Tag), tag, len - 1);
    alloc->Tag[len] = '\0';

    if (!g_allocationList)
    {
        g_allocationList = node;
    }
    else
    {
        LinkedListAddNode(g_allocationList, node);
    }

    return alloc;
}

void DeleteAllocation(allocation_t* alloc)
{
    list_node_t* node;

    DebugOut("MemoryManager: Freeing %s", alloc->Tag);

    node = LinkedListNodeFromObject(alloc);

    if (g_allocationList == node)
    {
        g_allocationList = node->Next;
    }

    LinkedListRemoveNode(node);

    free(node);
}

void* ObjectFromRefCounted(refcounted_t* refcounted)
{
    if (refcounted)
    {
        return (byte_t*)refcounted + sizeof(refcounted_t);
    }

    return NULL;
}

refcounted_t* RefCountedFromObject(void* p)
{
    if (p)
    {
        return (refcounted_t*)((byte_t*)p - sizeof(refcounted_t));
    }

    return NULL;
}

refcounted_t* NewRefCounted(const char* name, uint_t numBytes)
{
    refcounted_t* refcounted;

    assert(name);

    refcounted = (refcounted_t*)MemoryAlloc(name, sizeof(refcounted_t) + numBytes);
    if (!refcounted)
    {
        DebugOut("MemoryManager: Failed allocating ref counted lump: %s", name);
        return NULL;
    }

    DebugOut("MemoryManager: Allocated ref counted lump: %s", name);

    WadCopyDoomString(refcounted->Name, name);
    refcounted->RefCount = 1;

    ListAdd(g_refcountedList, refcounted);

    return refcounted;
}

void DeleteRefCounted(refcounted_t* refcounted)
{
    assert(refcounted);

    DebugOut("MemoryManager: Freeing ref counted lump: %s", refcounted->Name);

    ListRemove(g_refcountedList, refcounted);
    MemoryFree(refcounted);
}

refcounted_t* FindRefCounted(const char* name)
{
    int i, count;
    refcounted_t* refcounted;

    count = ListSize(g_refcountedList);

    for (i = 0; i < count; ++i)
    {
        refcounted = (refcounted_t*)ListGet(g_refcountedList, i);

        if (WadCompareDoomString(refcounted->Name, name))
        {
            ++refcounted->RefCount;
            return refcounted;
        }
    }

    return NULL;
}



//
// debug methods
//

// print out memory details
void DebugMemoryReport()
{
#ifndef NDEBUG
    BOOL hadLeaks = FALSE;
    list_node_t* node = g_allocationList;

    DebugOut("MemoryManager: Summary Report");
    DebugOut("----------------------------------------------------------");

    while (node)
    {
        allocation_t* alloc = (allocation_t*)LinkedListObjectFromNode(node);

        DebugOut("    Allocation: %8d bytes  %s", alloc->Size, alloc->Tag);

        hadLeaks = TRUE;
        node = node->Next;
    }

    DebugOut("----------------------------------------------------------");

    if (hadLeaks)
    {
        // leaks detected. Please refer to the debug output or walk the list above to analyze
        if (IsDebuggerPresent())
        {
            DebugBreak();
        }
        else
        {
            assert(FALSE && "Leaks detected!");
        }
    }
#endif
}
