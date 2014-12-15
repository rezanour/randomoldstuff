#include "common.h"

list_node_t* LinkedListCreateNode(uint_t numBytes)
{
    list_node_t* node = (list_node_t*)MemoryAlloc("linked list node", sizeof(list_node_t) + numBytes);
    if (!node)
    {
        return NULL;
    }

    ZeroMemory(node, sizeof(list_node_t) + numBytes);

    return node;
}

void LinkedListDeleteNode(list_node_t* node)
{
    assert(node);

    MemoryFree(node);
}

void* LinkedListObjectFromNode(list_node_t* node)
{
    assert(node);
    return (byte_t*)node + sizeof(list_node_t);
}

list_node_t* LinkedListNodeFromObject(void* obj)
{
    assert(obj);
    return (list_node_t*)((byte_t*)obj - sizeof(list_node_t));
}

void LinkedListAddNode(list_node_t* existing, list_node_t* node)
{
    assert(existing);

    if (existing->Next)
    {
        existing->Next->Prev = node;
    }

    node->Next = existing->Next;
    node->Prev = existing;
    existing->Next = node;
}

void LinkedListRemoveNode(list_node_t* node)
{
    assert(node);

    if (node->Prev)
    {
        node->Prev->Next = node->Next;
    }

    if (node->Next)
    {
        node->Next->Prev = node->Prev;
    }
}
