#include "common.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#ifdef ENABLE_TESTS
    BOOL result = TRUE;

    result = RunFixedPointTests() && result;
    result = RunFixedAngleTests() && result;

    OutputDebugString("Results: ");
    OutputDebugString(result ? "PASSED" : "FAILED");
    OutputDebugString("\n");

    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);

    return 0;
#else
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);

    {
        //point_t player = { 132, 85 };

        InitializeTables();
        //DDA(player, DEGTOANGLE(30));
    }

    return 0;
#endif
}
