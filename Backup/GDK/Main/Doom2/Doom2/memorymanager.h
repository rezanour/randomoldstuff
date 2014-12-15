#ifndef _MEMORYMANAGER_H_
#define _MEMORYMANAGER_H_

//
// NOTE: The memory manager just simplifies down to malloc/free on Release builds.
// the memory tracking, tagging, and leak detection is only on Debug builds.
//

BOOL MemoryManagerInitialize();
void MemoryManagerUninitialize();

// basic allocation and freeing. The tag parameter is NOT optional. It is used to track allocations.
void* MemoryAlloc(const char* tag, uint_t numBytes);
void MemoryFree(void* p);
void* MemoryAllocWithLength(const char* tag, uint_t numBytes);

// gets a ref counted lump by name. The name is the key used to resolve references.
// requesting an object with the same name again just ups the ref count and returns the same pointer.
// releasing the object just decrements the ref count. Once ref count reaches 0, the object is actually
// freed
void* MemoryGetRefCountedLump(const char* name, long* size);
void MemoryReleaseRefCounted(void* p);


#endif // _MEMORYMANAGER_H_