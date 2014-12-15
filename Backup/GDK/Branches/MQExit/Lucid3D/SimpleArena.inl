// Implementation of SimpleArena
#pragma once

#include <cassert>
#include <cstdlib>
#include <malloc.h>

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h> // is there no better interlocked compare exchange that doesn't require all of windows.h?

namespace Lucid3D
{
    SimpleArena::SimpleArena(_In_ size_t arenaSize)
    {
        assert(arenaSize > 0 && "Allocating 0 byte arena is not useful and fragments memory unnecessarily");

        _root = static_cast<byte_t*>(malloc(arenaSize));
        assert(_root && "OUT OF MEMORY! Couldn't allocate arena.");

        _head = _root;
        _end = _head + arenaSize;

#ifdef _ENABLE_MEMORY_TAGGING_
        _annotations = nullptr;
        _annotationSize = 0;
        _annotationCapacity = 100;
#endif // _ENABLE_MEMORY_TAGGING_
#ifdef _ENABLE_MEMORY_PROFILING_
        _allocationCount = 0;
        _totalBytes = 0;
#endif
    }

    SimpleArena::~SimpleArena()
    {
        Reset();

        if (_root)
        {
            free(_root);
        }
    }

    void* SimpleArena::Allocate(_In_ size_t size, _In_opt_ char* tag)
    {
        byte_t* current;
        do
        {
            current = _head;
            if (current + size > _end)
            {
                return nullptr;
            }

        } while (InterlockedCompareExchangePointer(reinterpret_cast<void**>(&_head), current + size, current) != current);

        TagMemory(current, tag);
        AddAllocation(size);

        return static_cast<void*>(current);
    }

    void* SimpleArena::AllocateAligned(_In_ size_t size, _In_ size_t alignment, _In_opt_ char* tag)
    {
        assert(alignment > 0 && "alignment must be greater than 0");

        //
        // move the head pointer to the next aligned address
        //
        byte_t* current;
        byte_t* p;
        size_t offset;
        do
        {
            current = _head;
            p = current;

            offset = reinterpret_cast<size_t>(current) % alignment;
            if (offset > 0)
            {
                p = current + alignment - offset;
                if (p > _end)
                {
                    return nullptr;
                }
            }
            if (p + size > _end)
            {
                return nullptr;
            }
        } while (InterlockedCompareExchangePointer(reinterpret_cast<void**>(&_head), p + size, current) != current);

        TagMemory(p, tag);
        AddAllocation(size);

        return static_cast<void*>(p);
    }

    void SimpleArena::Reset()
    {
        _head = _root;

#ifdef _ENABLE_MEMORY_TAGGING_

        if (_annotations)
        {
            for (size_t i = 0; i < _annotationSize; ++i)
            {
                if (_annotations[i].tag)
                {
                    free (_annotations[i].tag);
                }
            }
        }
        _annotationSize = 0;

#endif // _ENABLE_MEMORY_TAGGING_

#ifdef _ENABLE_MEMORY_PROFILING_

        _allocationCount = 0;
        _totalBytes = 0;

#endif // _ENABLE_MEMORY_PROFILING_
    }

    void SimpleArena::TagMemory(_In_ byte_t* p, _In_ char* tag)
    {
#ifdef _ENABLE_MEMORY_TAGGING_
        assert(p && "Can't annotate null memory block.");

        if (!p || !tag)
        {
            return;
        }

        if (!_annotations)
        {
            _annotations = static_cast<Annotation*>(malloc(sizeof(Annotation) * _annotationCapacity));
            assert(_annotations && "OUT OF MEMORY! Failed to allocate annotation buffer.");
        }
        else if (_annotationSize == _annotationCapacity)
        {
            _annotationCapacity += 100;
            _annotations = static_cast<Annotation*>(realloc(_annotations, sizeof(Annotation) * _annotationCapacity));
            assert(_annotations && "OUT OF MEMORY! Failed to allocate annotation buffer.");
        }

        size_t len = strlen(tag);

        _annotations[_annotationSize].p = p;
        _annotations[_annotationSize].tag = static_cast<char*>(malloc(len + 1));
        memcpy_s(_annotations[_annotationSize].tag, len + 1, tag, len + 1);
        _annotationSize++;
#else
        UNREFERENCED_PARAMETER(p);
        UNREFERENCED_PARAMETER(tag);
#endif // _ENABLE_MEMORY_TAGGING_
    }

    void SimpleArena::AddAllocation(_In_ size_t size)
    {
#ifdef _ENABLE_MEMORY_PROFILING_
        _allocationCount++;
        _totalBytes += size;
#else
        UNREFERENCED_PARAMETER(size);
#endif
    }

#ifdef _ENABLE_MEMORY_TAGGING_
    char* SimpleArena::GetTag(_In_ void* p) const
    {
        if (_annotations)
        {
            for (size_t i = 0; i < _annotationSize; ++i)
            {
                if (_annotations[i].p == p)
                {
                    return _annotations[i].tag;
                }
            }
        }

        return nullptr;
    }

    void SimpleArena::DumpAllTagsToOutput() const
    {
        if (_annotations)
        {
            wchar_t output[256] = {};
            for (size_t i = 0; i < _annotationSize; ++i)
            {
                wsprintf(output, L"Address: 0x%08x, Tag: %S\n", _annotations[i].p, _annotations[i].tag);
                OutputDebugString(output);
            }
        }
    }

#endif // _ENABLE_MEMORY_TAGGING_

#ifdef _ENABLE_MEMORY_PROFILING_

    size_t SimpleArena::GetAllocationCount() const
    {
        return _allocationCount;
    }

    size_t SimpleArena::GetAllocationTotalBytes() const
    {
        return _totalBytes;
    }

    void SimpleArena::DumpProfileToOutput() const
    {
        wchar_t output[256] = {};
        wsprintf(output, L"Allocation Count: %lu\nAllocation Total Bytes: %lu\n", _allocationCount, _totalBytes);
        OutputDebugString(output);
    }

#endif // _ENABLE_MEMORY_PROFILING_
}
