// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.

#pragma once

#include <malloc.h>
#include <exception>


namespace DirectX
{
    // Derive from this to customize operator new and delete for
    // types that have special heap alignment requirements.
    //
    // Example usage:
    //
    //      _declspec(align(16)) struct MyAlignedType : public AlignedNew<MyAlignedType>

    template<typename TDerived>
    struct AlignedNew
    {
        static_assert(__alignof(TDerived) > 8, "AlignedNew is only useful for types with > 8 byte alignment. Did you forget a _declspec(align) on TDerived?");


        // Allocate aligned memory.
        static void* operator new (size_t size)
        {
            void* ptr = _aligned_malloc(size, __alignof(TDerived));

            if (!ptr)
                throw std::bad_alloc();

            return ptr;
        }


        // Free aligned memory.
        static void operator delete (void* ptr)
        {
            _aligned_free(ptr);
        }


        // Array overloads.
        static void* operator new[] (size_t size)
        {
            return operator new(size);
        }


        static void operator delete[] (void* ptr)
        {
            operator delete(ptr);
        }
    };
}
