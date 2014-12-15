#include "Precomp.h"

extern "C" void __cdecl DebugOut(_In_z_ _Printf_format_string_ const wchar_t* format, ...)
{
    static wchar_t message[1000];

    va_list args;
    va_start(args, format);
    vswprintf_s(message, format, args);
    va_end(args);

    OutputDebugString(message);
}
