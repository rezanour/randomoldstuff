#include "directgraphics.h"
#include "internal.h"

#include <stdarg.h>

#define MAX_DEBUG_MESSAGE_LENGTH    512

static pfnDebugOut g_debugOut = NULL;


//
// public debug methods
//

void DGSetDebugOut(pfnDebugOut debugOut)
{
    g_debugOut = debugOut;
}

void __cdecl DGDebugOut(const char* format, ...)
{
#ifndef NDEBUG
    static char message[MAX_DEBUG_MESSAGE_LENGTH];

    if (g_debugOut)
    {
        va_list args;

        va_start(args, format);
        _vsnprintf_s(message, MAX_DEBUG_MESSAGE_LENGTH, _TRUNCATE, format, args);
        va_end(args);

        g_debugOut(message);
    }
#endif
}
