#pragma once

#include "Platform.h"

namespace GDK
{
#define GDKNEW new (__FUNCSIGW__, __FILEW__, __LINE__)

    class MemoryObject
    {
    public:
        static void operator delete(_In_ void* p);

    protected:
        static void* operator new(_In_ size_t size, _In_opt_z_ const wchar_t* tag, _In_z_ const wchar_t* filename, _In_ uint32_t lineNumber);
        static void operator delete(_In_ void* p, _In_opt_z_ const wchar_t* tag, _In_z_ const wchar_t* filename, _In_ uint32_t lineNumber);

    private:
        static void* operator new(size_t size);
        static void* operator new(size_t size, const std::nothrow_t& nothrow_constant);
        static void* operator new(size_t size, void* buffer);

        static void operator delete(void* p, const std::nothrow_t& nothrow_constant);
        static void operator delete(void* p, void* buffer);

        static void* operator new[](size_t size);
        static void* operator new[](size_t size, const std::nothrow_t& nothrow_constant);
        static void* operator new[](size_t size, void* buffer);

        static void operator delete[](void* p);
        static void operator delete[](void* p, const std::nothrow_t& nothrow_constant);
        static void operator delete[](void* p, void* buffer);
    };
}
