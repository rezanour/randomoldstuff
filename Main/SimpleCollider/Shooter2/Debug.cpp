#include "Precomp.h"

#ifdef _DEBUG

_Use_decl_annotations_
void __cdecl DebugOut(const wchar_t* format, ...)
{
    wchar_t message[1024] = {};

    va_list args;
    va_start(args, format);
    vswprintf_s(message, format, args);
    va_end(args);

    OutputDebugString(message);
}

#endif
