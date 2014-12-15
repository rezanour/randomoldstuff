//
// A simple arena allocator with the following features:
//
// 1. Reset (fast free which allows reuse of the arena)
// 2. Lock-free design, which allows efficient multithreaded access
// 3. Aligned allocations
// 4. Optional profiling and diagnostics
//
#pragma once

#include <cstddef>
#include <sal.h>

namespace Lucid3D
{
    using std::size_t;

    class SimpleArena
    {
        typedef unsigned char byte_t;

    public:
        SimpleArena(_In_ size_t arenaSize);
        ~SimpleArena();

        void* Allocate(_In_ size_t size, _In_opt_ char* tag = nullptr);
        void* AllocateAligned(_In_ size_t size, _In_ size_t alignment, _In_opt_ char* tag = nullptr);

        //
        // WARNING: Use Reset very carefully. It invalidates any outstanding
        // pointers to memory allocated by this allocator, so using those pointers
        // after calling Reset() is undefined.
        //
        void Reset();

    private:
        byte_t* _head;
        byte_t* _root;
        byte_t* _end;

        void TagMemory(_In_ byte_t* p, _In_ char* tag);
        void AddAllocation(_In_ size_t size);

#ifdef _ENABLE_MEMORY_TAGGING_

        //
        // Memory tagging annotates allocations with optional string names, which 
        // can be dumped and viewed later or looked up at runtime.
        //
    public:
        char* GetTag(_In_ void* p) const;
        void DumpAllTagsToOutput() const;

    private:
        struct Annotation
        {
            byte_t* p;
            char* tag;
        };

        Annotation* _annotations;
        size_t _annotationSize;
        size_t _annotationCapacity;

#endif // _ENABLE_MEMORY_TAGGING_

#ifdef _ENABLE_MEMORY_PROFILING_

        //
        // For now, memory profiling only tracks how many allocations are made
        // and what the high watermark reached during operation is. 
        // Combining this with tagging can give a pretty good
        // idea of what kind of allocation profile your app has.
        //
    public:
        size_t GetAllocationCount() const;
        size_t GetAllocationTotalBytes() const;
        void DumpProfileToOutput() const;

    private:
        size_t _allocationCount;
        size_t _totalBytes;

#endif // _ENABLE_MEMORY_PROFILING_
    };
}

#include "SimpleArena.inl"
