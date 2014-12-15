#include "common.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    MSG msg = {0};

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    if (!CreateMainWindow(hInstance, nCmdShow))
    {
        return 1;
    }

    DGSetDebugOut(OutputDebugStringA);

    if (!DGInitialize())
    {
        DestroyMainWindow();
        return 2;
    }

    while (msg.message != WM_QUIT)
    {
        if (GetMessage(&msg, NULL, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    DGUninitialize(true);

    // ensure the window is destroyed
    DestroyMainWindow();

    return msg.wParam;
}
