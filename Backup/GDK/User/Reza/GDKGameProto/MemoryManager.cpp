#include "StdAfx.h"
#include "MemoryManager.h"

using GDK::MemoryManager;

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
        size_t totalSize = 0;
        bool first = true;
        while (block)
        {
            if (!block->free)
            {
                if (first)
                {
                    first = false;
                    OutputDebugString(L"\n*********************\n");
                    OutputDebugString(L"Leaks Detected!\n");
                    OutputDebugString(L"*********************\n\n");
                }

                wchar_t message[300] = {0};
                StringCchPrintf(message, _countof(message), L"%u bytes at 0x%08x (%S)\n", block->size, (unsigned int)(void*)((unsigned char*)block + sizeof(Block)), (block->tag ? block->tag : "<no tag>"));
                OutputDebugString(message);
                totalSize += block->size;
            }
            block = block->next;
        }

        if (!first)
        {
            wchar_t message[100] = {0};
            StringCchPrintf(message, _countof(message), L"\nTotal Memory Leaked: %u bytes\n", totalSize);
            OutputDebugString(message);
            OutputDebugString(L"\n*********************\n\n");
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
        (*ppMemMgr)->_totalSize = 10000000; // ~10MB

        void* p = malloc((*ppMemMgr)->_totalSize);
        hr = (p ? S_OK : E_OUTOFMEMORY);

        if (SUCCEEDED(hr))
        {
            (*ppMemMgr)->_root = (Block*)p;
            (*ppMemMgr)->_root->next = nullptr;
            (*ppMemMgr)->_root->size = (*ppMemMgr)->_totalSize - sizeof(Block);
            (*ppMemMgr)->_root->tag = nullptr;
            (*ppMemMgr)->_root->free = true;
        }
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

                block->tag = nullptr;
                block->size = block->size - sizeof(Block) - next->size;
                block->next = next;
            }

            // use up the block
            block->free = false;
#ifndef NDEBUG
            if (tag)
            {
                block->tag = _strdup(tag);
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
