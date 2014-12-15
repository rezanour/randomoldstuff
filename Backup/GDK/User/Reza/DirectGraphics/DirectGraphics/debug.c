#include "common.h"
#include "debug.h"

#include <stdio.h>

void __cdecl DebugOut(const char* const format, ...)
{
    static char message[500] = {0};
    va_list args;

    va_start(args, format);
    vsprintf_s(message, ARRAYSIZE(message), format, args);
    va_end(args);

    OutputDebugString(message);
}
