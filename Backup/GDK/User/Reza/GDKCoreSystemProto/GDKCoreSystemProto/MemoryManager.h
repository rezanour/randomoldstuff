#pragma once

namespace GDK {

class MemoryManager
{
public:
    static _Check_return_ HRESULT Create(_Deref_out_ MemoryManager** ppMemMgr);
    virtual ~MemoryManager();

    _Ret_ void* Alloc(_In_ size_t bytes, _In_opt_z_ const char* tag = nullptr);
    void Free(_In_ _Post_ptr_invalid_ void* p);

private:
    MemoryManager();
    _Check_return_ HRESULT Initialize();

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
