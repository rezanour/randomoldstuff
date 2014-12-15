#include "Quake2Game.h"

//#ifndef NDEBUG
#include <Windows.h>
//#endif

void DebugOut(const char* format, ...)
{
//#ifndef NDEBUG
    BOOL error = FALSE;
    char message[2048] = {0};
    va_list list;
    va_start(list, format);
    if (vsprintf_s(message, (sizeof(message) - 2),format, list) < 0)
    {
        error = TRUE;
    }
    va_end(list);

    if (!error)
    {
        if (strcat_s(message, (sizeof(message) - 1), "\n") != 0)
        {
            error = TRUE;
        }
    }

    if (!error)
    {
        OutputDebugStringA(message);
    }

    if (error)
    {
        OutputDebugStringA("DEBUGOUT ERROR: failed to output debug text\n");
    }
//#else
//    UNREFERENCED_PARAMETER(format);
//#endif
}