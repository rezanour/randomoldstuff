#include "StdAfx.h"
#include "MemoryManager.h"

namespace GDK {

MemoryManager::MemoryManager()
    : _root(nullptr), _totalSize(0)
{
}

MemoryManager::~MemoryManager()
{
    if (_root)
    {
        // basic leak detection
        Block* block = _root;
        while (block)
        {
            if (!block->free)
            {
                wchar_t message[100] = {0};
                wsprintf(message, L"Leak Detected! %u bytes at 0x%08x (%S)\n", block->size, (unsigned int)(void*)((unsigned char*)block + sizeof(Block)), (block->tag ? block->tag : "<no tag>"));
                OutputDebugString(message);
            }
            block = block->next;
        }

        free((void*)_root);
    }
}

HRESULT MemoryManager::Create(MemoryManager** ppMemMgr)
{
    if (!ppMemMgr)
        return E_POINTER;

    *ppMemMgr = new MemoryManager;
    HRESULT hr = (*ppMemMgr) ? S_OK : E_OUTOFMEMORY;

    if (SUCCEEDED(hr))
    {
        hr = (*ppMemMgr)->Initialize();
    }

    if (FAILED(hr))
    {
        delete *ppMemMgr;
        *ppMemMgr = nullptr;
    }

    return hr;
}

void* MemoryManager::Alloc(size_t bytes, const char* tag)
{
    UNREFERENCED_PARAMETER(tag);
    // find an available free block
    Block* block = _root;
    while (block)
    {
        if (block->free && block->size >= bytes)
        {
            // is there enough left over to split it? or just use the whole block?
            if (block->size - bytes >= (sizeof(Block) + MinBlockSize))
            {
                Block* next = (Block*)((unsigned char*)block + sizeof(Block) + bytes);
                next->next = block->next;
                next->size = block->size - sizeof(Block) - bytes;
                next->tag = nullptr;
                next->free = true;

                block->size = block->size - sizeof(Block) - next->size;
                block->next = next;
            }

            // use up the block
            block->free = false;
#ifndef NDEBUG
            if (tag)
            {
                block->tag = (char*)malloc(strlen(tag) + 1);
                strcpy_s(block->tag, strlen(tag)+1, tag);
            }
#endif
            return (void*)((unsigned char*)block + sizeof(Block));
        }

        block = block->next;
    }

    // not enough room for allocation!

    return nullptr;
}

void MemoryManager::Free(void* p)
{
    if (p)
    {
        Block* block = (Block*)((unsigned char*)p - sizeof(Block));

        // reclaim block, and then see if we can coalesce with next block
        block->free = true;
#ifndef NDEBUG
        if (block->tag)
        {
            free(block->tag);
        }
#endif
        if (block->next && block->next->free)
        {
            block->size = block->size + block->next->size + sizeof(Block);
            block->next = block->next->next;
        }
    }
}

HRESULT MemoryManager::Initialize()
{
    _totalSize = 10000000; // ~10MB

    void* p = malloc(_totalSize);
    HRESULT hr = (p ? S_OK : E_OUTOFMEMORY);

    if (SUCCEEDED(hr))
    {
        _root = (Block*)p;
        _root->next = nullptr;
        _root->size = _totalSize - sizeof(Block);
        _root->tag = nullptr;
        _root->free = true;
    }

    return hr;
}

} // GDK
