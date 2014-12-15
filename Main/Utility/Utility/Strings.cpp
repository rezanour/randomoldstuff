#include <Utility.h>
#include <string.h>

_Use_decl_annotations_
bool ConvertCharToWChar(const char* source, wchar_t* dest, size_t destCount)
{
    size_t size = strlen(source);
    if (destCount < size + 1)
    {
        return false;
    }

    for (size_t i = 0; i < size; ++i)
    {
        dest[i] = (wchar_t)source[i];
    }

    dest[size] = 0;

    return true;
}

_Use_decl_annotations_
bool ConvertWCharToChar(const wchar_t* source, char* dest, size_t destCount)
{
    size_t size = wcslen(source);
    if (destCount < size + 1)
    {
        return false;
    }

    for (size_t i = 0; i < size; ++i)
    {
        dest[i] = (char)source[i];
    }

    dest[size] = 0;

    return true;
}
