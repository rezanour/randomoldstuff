#include "precomp.h"
#include "debug.h"

_Use_decl_annotations_
void __cdecl DebugOut(const char* format, ...)
{
    char message[512] = {};

    va_list args;
    va_start(args, format);
    vsprintf_s(message, format, args);
    va_end(args);

    OutputDebugString(message);
}

_Use_decl_annotations_
void __cdecl Error(const char* format, ...)
{
    char message[512] = {};

    va_list args;
    va_start(args, format);
    vsprintf_s(message, format, args);
    va_end(args);

    OutputDebugString("ERROR: ");
    OutputDebugString(message);
    OutputDebugString("\n");

    // Generate an exception to terminate the process
    throw std::exception(message);
}
