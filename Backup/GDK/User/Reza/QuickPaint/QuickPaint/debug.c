#include "common.h"

#include <stdio.h>
#include <stdarg.h>

void __cdecl DebugOut(const wchar_t* const format, ...)
{
    static wchar_t message[500] = {0};
    va_list args;

    va_start(args, format);
    vswprintf_s(message, ARRAYSIZE(message), format, args);
    va_end(args);

    OutputDebugString(message);
}
