#pragma once

#include "Platform.h"

namespace GDK
{
    _Use_decl_annotations_
    void* MemoryManagedObject::operator new(size_t numBytes, const wchar_t* filename, size_t lineNumber)
    {
        // TODO: implement a memory tracker
        UNREFERENCED_PARAMETER(filename);
        UNREFERENCED_PARAMETER(lineNumber);

        return malloc(numBytes);
    }

    _Use_decl_annotations_
    void MemoryManagedObject::operator delete(void* p, const wchar_t* filename, size_t lineNumber)
    {
        UNREFERENCED_PARAMETER(filename);
        UNREFERENCED_PARAMETER(lineNumber);
        delete(p);
    }

    _Use_decl_annotations_
    void MemoryManagedObject::operator delete(void* p)
    {
        free(p);
    }
}
