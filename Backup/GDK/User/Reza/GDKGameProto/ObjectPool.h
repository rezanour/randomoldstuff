#pragma once

#include "MemoryObject.h"

namespace GDK
{
    class MemoryManager;

    class ObjectPool : public MemoryObject
    {
    public:
        // Construction/Destruction
        static HRESULT Create(MemoryManager* memoryPool, ObjectPool** ppObjectPool);
        ~ObjectPool();

        // Object creation

        // raw memory allocation for an object
        void* Alloc(size_t size, const char* name = nullptr);
        void Free(void* p);

        // convenience wrappers for types that derive from IObject and have common numbers of arguments
        template <typename Ty>
        Ty* Create();

        template <typename Ty, typename TArg0>
        Ty* Create(TArg0 arg0);

        template <typename Ty, typename TArg0, typename TArg1>
        Ty* Create(TArg0 arg0, TArg1 arg1);

        template <typename Ty, typename TArg0, typename TArg1, typename TArg2>
        Ty* Create(TArg0 arg0, TArg1 arg1, TArg2 arg2);

        template <typename Ty, typename TArg0, typename TArg1, typename TArg2, typename TArg3>
        Ty* Create(TArg0 arg0, TArg1 arg1, TArg2 arg2, TArg3 arg3);

        template <typename Ty>
        void Destroy(Ty*& p);

    private:
        ObjectPool(MemoryManager* memoryPool);

        MemoryManager*  _memory;
        uint32_t        _numObjects;
    };

    template <typename Ty>
    inline Ty* ObjectPool::Create()
    {
        void* p = Alloc(sizeof(Ty), nullptr);

        if (!p)
            return nullptr;

        return ::new(p) Ty();
    }

    template <typename Ty, typename TArg0>
    inline Ty* ObjectPool::Create(TArg0 arg0)
    {
        void* p = Alloc(sizeof(Ty), nullptr);

        if (!p)
            return nullptr;

        return ::new(p) Ty(arg0);
    }

    template <typename Ty, typename TArg0, typename TArg1>
    inline Ty* ObjectPool::Create(TArg0 arg0, TArg1 arg1)
    {
        void* p = Alloc(sizeof(Ty), nullptr);

        if (!p)
            return nullptr;

        return ::new(p) Ty(arg0, arg1);
    }

    template <typename Ty, typename TArg0, typename TArg1, typename TArg2>
    inline Ty* ObjectPool::Create(TArg0 arg0, TArg1 arg1, TArg2 arg2)
    {
        void* p = Alloc(sizeof(Ty), nullptr);

        if (!p)
            return nullptr;

        return ::new(p) Ty(arg0, arg1, arg2);
    }

    template <typename Ty, typename TArg0, typename TArg1, typename TArg2, typename TArg3>
    inline Ty* ObjectPool::Create(TArg0 arg0, TArg1 arg1, TArg2 arg2, TArg3 arg3)
    {
        void* p = Alloc(sizeof(Ty), nullptr);

        if (!p)
            return nullptr;

        return ::new(p) Ty(arg0, arg1, arg2, arg3);
    }

    template <typename Ty>
    inline void ObjectPool::Destroy(Ty*& p)
    {
        p->~Ty();
        Free(p);
        p = nullptr;
    }

} // GDK
