#pragma once

namespace GDK
{
    class MemoryObject
    {
    public:
        // Ensure new and delete for this object go through the executive's memory manager
        void* operator new(size_t bytes, const char* tag = nullptr);
        void operator delete(void *p, const char* tag);
        void operator delete(void *p);
    };
} // GDK
