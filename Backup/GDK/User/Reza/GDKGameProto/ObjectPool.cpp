#include "StdAfx.h"
#include "MemoryManager.h"
#include "ObjectPool.h"

#include <new>

using GDK::MemoryManager;
using GDK::ObjectPool;

ObjectPool::ObjectPool(MemoryManager* memoryPool)
    : _memory(memoryPool)
{
}

ObjectPool::~ObjectPool()
{
}

HRESULT ObjectPool::Create(MemoryManager* memoryPool, ObjectPool** ppObjectPool)
{
    if (!memoryPool)
        return E_INVALIDARG;

    if (!ppObjectPool)
        return E_POINTER;

    *ppObjectPool = GDKNEW("ObjectPool") ObjectPool(memoryPool);
    return (*ppObjectPool) ? S_OK : E_OUTOFMEMORY;
}

void* ObjectPool::Alloc(size_t size, const char* name)
{
    return _memory->Alloc(size, name);
}

void ObjectPool::Free(void* p)
{
    _memory->Free(p);
}

