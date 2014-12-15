#pragma once

#ifdef _DEBUG

void _cdecl
DbgPrintf(
    PCWSTR format,
    ...
    );

#else

#define DbgPrintf(format, ...) {}

#endif
