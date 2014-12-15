#include <Platform.h>
#include <MemoryObject.h>

namespace GDK
{
    void* MemoryObject::operator new(_In_ size_t size, _In_opt_z_ const wchar_t* tag, _In_z_ const wchar_t* filename, _In_ uint32_t lineNumber)
    {
        UNREFERENCED_PARAMETER(size);
        UNREFERENCED_PARAMETER(tag);
        UNREFERENCED_PARAMETER(filename);
        UNREFERENCED_PARAMETER(lineNumber);
        return malloc(size);
    }

    void MemoryObject::operator delete(_In_ void* p, _In_opt_z_ const wchar_t* tag, _In_z_ const wchar_t* filename, _In_ uint32_t lineNumber)
    {
        UNREFERENCED_PARAMETER(tag);
        UNREFERENCED_PARAMETER(filename);
        UNREFERENCED_PARAMETER(lineNumber);
        free(p);
    }

    void MemoryObject::operator delete(_In_ void* p)
    {
        free(p);
    }
}
