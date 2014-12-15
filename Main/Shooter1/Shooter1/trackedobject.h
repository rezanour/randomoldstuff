#pragma once

template <MemoryTag Tag>
class TrackedObject
{
public:

    void* operator new (_In_ size_t size)
    {
        GetMemoryTracker().AddAllocation(Tag, size);
        void* p = malloc(size);
        CHECKNOTNULL(p);
        return p;
    }

    void operator delete (_In_opt_ _Post_invalid_ void* p, _In_ size_t size)
    {
        if (p != nullptr)
        {
            GetMemoryTracker().RemoveAllocation(Tag, size);
            free(p);
        }
    }

    void* operator new[] (_In_ size_t size)
    {
        GetMemoryTracker().AddAllocation(Tag, size);
        void* p = malloc(size);
        CHECKNOTNULL(p);
        return p;
    }

    void operator delete[] (_In_opt_ _Post_invalid_ void* p, _In_ size_t size)
    {
        if (p != nullptr)
        {
            GetMemoryTracker().RemoveAllocation(Tag, size);
            free(p);
        }
    }
};
