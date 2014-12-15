#pragma once

namespace GDK
{
    class MemoryManager
    {
    public:
        static HRESULT Create(MemoryManager** ppMemMgr);
        ~MemoryManager();

        void* Alloc(size_t bytes, const char* tag = nullptr);
        void Free(void* p);

    private:
        MemoryManager();

        // dead simple manager for prototype purposes
        struct Block
        {
            Block* next;
            size_t size;
            char* tag;
            bool free;
        };

        Block* _root;
        size_t _totalSize;

        static const size_t MinBlockSize = 16;
    };
} // GDK
