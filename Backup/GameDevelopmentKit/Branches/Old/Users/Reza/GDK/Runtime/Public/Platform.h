#pragma once

#include <stddef.h>
#include <stdint.h>
#include <assert.h>

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <exception>
#include <stdexcept>
#include <fstream>

typedef uint8_t byte_t;

#ifdef WIN32
#include <sal.h>
#endif

#ifndef UNREFERENCED_PARAMETER
#define UNREFERENCED_PARAMETER(x) (x)
#endif

// math library
#include "MathUtil.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix.h"

namespace GDK
{
    class NonCopyable
    {
    protected:
        NonCopyable() {}

    private:
        NonCopyable(_In_ const NonCopyable&);
        NonCopyable operator= (_In_ const NonCopyable&);
    };

    class MemoryManagedObject
    {
    public:
        // TODO: do we need [] forms? Probably not. We don't want to allocate blocks of the raw types. Collections should be using shared_ptr's to this type

        static void* operator new(_In_ size_t numBytes, _In_z_ const wchar_t* filename, _In_ size_t lineNumber);
        static void operator delete(_In_ void* p, _In_z_ const wchar_t* filename, _In_ size_t lineNumber);

        static void operator delete(_In_ void* p);

    private:
        // Block non-memory-managed allocations

        // TODO: block the rest of the overloads for new/delete
        static void* operator new(size_t);

        static void* operator new(size_t, void*);
        static void operator delete(void*, void*);
    };

    template <class Ty>
    class RuntimeObject : public NonCopyable, public MemoryManagedObject, public std::enable_shared_from_this<Ty>
    {
    };

    class GDKException : public std::exception
    {
    public:
        GDKException(_In_ const std::string& message, _In_ int32_t errorCode) :
            _message(message), 
            _errorCode(errorCode)
        {
        }

        const char* what() const override { return _message.c_str(); }

        int32_t GetErrorCode() const { return _errorCode; }

    private:
        int32_t _errorCode;
        std::string _message;
    };
}
