#include "StdAfx.h"
#include "Executive.h"
#include "MemoryManager.h"
#include "MemoryObject.h"

using GDK::Executive;
using GDK::MemoryObject;

void* MemoryObject::operator new(size_t bytes, const char* tag)
{
    return Executive::GetMemoryManager()->Alloc(bytes, tag);
}

void MemoryObject::operator delete(void* p, const char* tag)
{
    UNREFERENCED_PARAMETER(tag);
    delete p;
}

void MemoryObject::operator delete(void* p)
{
    Executive::GetMemoryManager()->Free(p);
}
