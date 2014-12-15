#include "precomp.h"
#include "debug.h"

#if defined(_DEBUG)

_Use_decl_annotations_
void __cdecl DebugOut(const char* format, ...)
{
    char message[1024];

    va_list args;
    va_start(args, format);
    vsprintf_s(message, format, args);
    va_end(args);

    OutputDebugStringA(message);
}

_Use_decl_annotations_
void __cdecl Error(const char* format, ...)
{
    char message[512] = {};

    va_list args;
    va_start(args, format);
    vsprintf_s(message, format, args);
    va_end(args);

    OutputDebugStringA("ERROR: ");
    OutputDebugStringA(message);
    OutputDebugStringA("\n");

    DebugBreak();

    // Generate an exception to terminate the process
    throw std::exception(message);
}

#endif
