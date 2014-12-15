#include "common.h"

#define CHUNK_SIZE  10

typedef struct
{
    void** Items;
    uint_t Count;
    uint_t Capacity;
    item_deleter Deleter;
} list_t;


//
// public list methods
//
arraylist_t ListCreate(item_deleter deleter)
{
    list_t* list = (list_t*)MemoryAlloc("list", sizeof(list_t));
    if (!list)
    {
        return NULL;
    }

    ZeroMemory(list, sizeof(list_t));

    list->Capacity = CHUNK_SIZE;
    list->Items = (void**)MemoryAlloc("list items", sizeof(void*) * list->Capacity);
    if (!list->Items)
    {
        MemoryFree(list);
        return NULL;
    }

    list->Deleter = deleter;

    ZeroMemory(list->Items, sizeof(void*) * list->Capacity);
    return list;
}

void ListDestroy(arraylist_t list)
{
    list_t* p = (list_t*)list;

    assert(list);

    if (p->Items)
    {
        // if we have a deleter, clear out all of the contents first
        if (p->Deleter)
        {
            uint_t i;
            for (i = 0; i < p->Count; ++i)
            {
                p->Deleter(p->Items[i]);
            }
        }

        MemoryFree(p->Items);
    }

    MemoryFree(p);
}

void ListAdd(arraylist_t list, void* item)
{
    list_t* p = (list_t*)list;

    assert(list);

    // if we've reached our current capacity, realloc and grow the list
    if (p->Count >= p->Capacity)
    {
        uint_t i;
        void** newBuffer = NULL;

        p->Capacity += CHUNK_SIZE;
        newBuffer = (void**)MemoryAlloc("list items", sizeof(void*) * p->Capacity);

        assert(newBuffer);

        ZeroMemory(newBuffer, sizeof(void*) * p->Capacity);

        // copy over all the old items
        for (i = 0; i < p->Count; ++i)
        {
            newBuffer[i] = p->Items[i];
        }

        // delete the old buffer
        MemoryFree(p->Items);

        // set the new buffer as the items
        p->Items = newBuffer;
    }

    // add item to end of list
    p->Items[p->Count++] = item;
}

void ListRemove(arraylist_t list, void* item)
{
    // list remove will find and remove all matching instances of item.

    list_t* p = (list_t*)list;
    int i;

    assert(list);

    for (i = p->Count - 1; i >= 0; --i)
    {
        if (p->Items[i] == item)
        {
            uint_t j;

            // if we have a deleter, delete the item
            if (p->Deleter)
            {
                p->Deleter(p->Items[i]);
            }

            --p->Count;

            // compact the rest of the list down over top
            for (j = i; j < p->Count; ++j)
            {
                p->Items[j] = p->Items[j + 1];
            }
        }
    }
}

void ListRemoveAt(arraylist_t list, uint_t index)
{
    list_t* p = (list_t*)list;
    uint_t i;

    assert(list);
    assert(index < p->Count);

    if (p->Deleter)
    {
        p->Deleter(p->Items[index]);
    }

    --p->Count;

    for (i = index; i < p->Count; ++i)
    {
        p->Items[i] = p->Items[i + 1];
    }
}

uint_t ListSize(arraylist_t list)
{
    list_t* p = (list_t*)list;

    assert(list);

    return p->Count;
}

void ListClear(arraylist_t list)
{
    list_t* p = (list_t*)list;

    assert(list);

    if (p->Deleter)
    {
        uint_t i;
        for (i = 0; i < p->Count; ++i)
        {
            p->Deleter(p->Items[i]);
        }
    }

    p->Count = 0;
}

void* ListGet(arraylist_t list, uint_t index)
{
    list_t* p = (list_t*)list;

    assert(list);

    assert(index < p->Count);

    return p->Items[index];
}

void ListSet(arraylist_t list, uint_t index, void* item)
{
    list_t* p = (list_t*)list;

    assert(list);

    assert(index < p->Count);

    p->Items[index] = item;
}
