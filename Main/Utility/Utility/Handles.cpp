#include <Utility.h>
#include <assert.h>
#include <memory.h>

#pragma warning(push)
#pragma warning(disable: 4201) // nameless struct
typedef union
{
    struct
    {
        uint32_t        Index       : 18;
        uint32_t        Type        : 8;
        uint32_t        Uniqueness  : 6;
    };

    uint32_t Value;
} HANDLE_INTERNAL;
#pragma warning(pop)

typedef struct
{
    uint32_t        Reserved    : 18;
    uint32_t        Type        : 8;
    uint32_t        Uniqueness  : 6;
    void*           Object;
} HANDLE_TABLE_ENTRY;

static const uint32_t MAX_HANDLES = 1 << 18;

static HANDLE_TABLE_ENTRY* g_HandleTable;
static uint32_t g_HighWaterMark;
static uint32_t* g_FreeList;
static uint32_t g_NumFree;

static HANDLE_TABLE_ENTRY* GetEntry(_In_ Handle handle)
{
    HANDLE_INTERNAL internal;
    internal.Value = handle;

    HANDLE_TABLE_ENTRY* entry = &g_HandleTable[internal.Index];
    if (entry->Type != internal.Type || entry->Uniqueness != internal.Uniqueness)
    {
        // retired or invalid handle
        assert(false);
        return nullptr;
    }

    return entry;
}

bool HandleTableStartup()
{
    assert(g_HandleTable == nullptr);
    assert(g_FreeList == nullptr);

    delete[] g_HandleTable;
    delete[] g_FreeList;

    g_HandleTable = new HANDLE_TABLE_ENTRY[MAX_HANDLES];
    if (g_HandleTable == nullptr)
    {
        return false;
    }

    g_FreeList = new uint32_t[MAX_HANDLES];
    if (g_FreeList == nullptr)
    {
        delete [] g_HandleTable;
        g_HandleTable = nullptr;
        return false;
    }

    memset(g_HandleTable, 0, sizeof(HANDLE_TABLE_ENTRY) * MAX_HANDLES);
    memset(g_FreeList, 0, sizeof(uint32_t) * MAX_HANDLES);

    g_HighWaterMark = 0;
    g_NumFree = 0;

    return true;
}

void HandleTableShutdown()
{
    assert(g_HandleTable != nullptr);
    assert(g_FreeList != nullptr);

    if (g_HighWaterMark > g_NumFree)
    {
        assert("Leaked handles in the table!" && false);
    }

    delete[] g_HandleTable;
    g_HandleTable = nullptr;

    delete[] g_FreeList;
    g_FreeList = nullptr;
}

_Use_decl_annotations_
Handle AllocHandle(void* object, uint8_t type)
{
    if (type == 0)
    {
        assert("0 is not valid for type" && false);
        return InvalidHandle;
    }

    uint32_t index;
    if (g_NumFree > 0)
    {
        index = g_FreeList[--g_NumFree];
    }
    else if (g_HighWaterMark < MAX_HANDLES)
    {
        index = g_HighWaterMark++;
    }
    else
    {
        // No more room in table
        assert(false);
        return InvalidHandle;
    }

    HANDLE_TABLE_ENTRY* entry = &g_HandleTable[index];
    HANDLE_INTERNAL handle;
    handle.Index = index;
    handle.Type = type;
    handle.Uniqueness = entry->Uniqueness;

    entry->Object = object;
    entry->Type = type;

    return handle.Value;
}

_Use_decl_annotations_
void FreeHandle(Handle handle)
{
    HANDLE_TABLE_ENTRY* entry = GetEntry(handle);
    if (entry == nullptr)
    {
        return;
    }

    ++entry->Uniqueness;
    entry->Type = 0;
    entry->Object = nullptr;

    g_FreeList[g_NumFree++] = (uint32_t)(entry - g_HandleTable);
}

_Use_decl_annotations_
void* GetObjectFromHandle(Handle handle)
{
    HANDLE_TABLE_ENTRY* entry = GetEntry(handle);
    if (entry == nullptr)
    {
        return nullptr;
    }

    return entry->Object;
}
