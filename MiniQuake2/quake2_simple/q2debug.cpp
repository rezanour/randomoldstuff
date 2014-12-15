#include "stdafx.h"
#include "q2debug.h"

#ifdef _DEBUG

void _cdecl
DbgPrintf(
    PCWSTR format,
    ...
    )
{
    va_list list;
    va_start(list, format);

    static WCHAR message[1000] = {};

    vswprintf_s(message, format, list);

    OutputDebugString(message);

    va_end(list);
}

#endif