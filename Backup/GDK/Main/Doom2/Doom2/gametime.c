#include "common.h"

static float g_totalElapsedTime = 1.0f;
static float g_elapsedTime = 1.0f;
static BOOL g_firstUpdate = TRUE;
static LARGE_INTEGER g_performanceFrequency;
static LARGE_INTEGER g_startCounter;
static LARGE_INTEGER g_lastCounter;
static LARGE_INTEGER g_currentCounter;

void GameTimeUpdate()
{
    if (g_firstUpdate)
    {
        g_firstUpdate = FALSE;

        QueryPerformanceFrequency(&g_performanceFrequency);
        QueryPerformanceCounter(&g_startCounter);
        QueryPerformanceCounter(&g_lastCounter);
    }

    QueryPerformanceCounter(&g_currentCounter);
    g_totalElapsedTime = (float)(g_currentCounter.QuadPart - g_startCounter.QuadPart) / (float)g_performanceFrequency.QuadPart;
    g_elapsedTime = (float)(g_currentCounter.QuadPart - g_lastCounter.QuadPart) / (float)g_performanceFrequency.QuadPart;
    g_lastCounter.QuadPart = g_currentCounter.QuadPart;
}

float GameTimeGetTotalElapsed()
{
    return g_totalElapsedTime;
}

float GameTimeGetElapsed()
{
    return g_elapsedTime;
}
