#include "tinyw3d.h"

/* HACK HACK - using windows/GDI to test out our code */
#include <windows.h>

static HWND g_hwnd;
static HDC g_hdcOut;
static HDC g_hdcBuffer;
static unsigned char* g_buffer;

#define SETRGBQUAD(q, r, g, b) \
    q.rgbRed = r; \
    q.rgbGreen = g; \
    q.rgbBlue = b; \
    q.rgbReserved = 0;

void InitializeGraphics(void* hwnd)
{
    BITMAPINFO* bmi;
    HBITMAP dib;
    UINT i;

    g_hwnd = (HWND)hwnd;
    g_hdcOut = GetDC(g_hwnd);
    g_hdcBuffer = CreateCompatibleDC(g_hdcOut);

    bmi = (BITMAPINFO*)malloc(sizeof(BITMAPINFO) + sizeof(RGBQUAD) * 256);

    bmi->bmiHeader.biSize = sizeof(bmi->bmiHeader);
    bmi->bmiHeader.biWidth = 160;
    bmi->bmiHeader.biHeight = -100;
    bmi->bmiHeader.biPlanes = 1;
    bmi->bmiHeader.biBitCount = 8;
    bmi->bmiHeader.biCompression = BI_RGB;

    /* generate some random palette for now */
    for (i = 0; i < 256; ++i)
    {
        bmi->bmiColors[i].rgbRed = (i * 1) % 256;
        bmi->bmiColors[i].rgbGreen = (i * 5) % 256;
        bmi->bmiColors[i].rgbBlue = (i * 10) % 256;
        bmi->bmiColors[i].rgbReserved = 0;
    }

    /* a couple of base colors to test with */
    SETRGBQUAD(bmi->bmiColors[0], 0, 0, 0); /* empty space */
    SETRGBQUAD(bmi->bmiColors[1], 0, 0, 255); /* walls */
    SETRGBQUAD(bmi->bmiColors[2], 255, 0, 0); /* player */
    SETRGBQUAD(bmi->bmiColors[3], 128, 128, 128); /* debug trace of dda path */
    SETRGBQUAD(bmi->bmiColors[4], 0, 255, 0); /* collision with wall */

    dib = CreateDIBSection(g_hdcBuffer, bmi, DIB_RGB_COLORS, (PVOID*)&g_buffer, 0, 0);
    if (!dib)
        exit(3);

    SelectObject(g_hdcBuffer, dib);
}

void W3DSetPixel(short x, short y, byte color)
{
    g_buffer[y * 160 + x] = color;
}

void Present()
{
    RECT client;
    GetClientRect(g_hwnd, &client);
    StretchBlt(g_hdcOut, 0, 0, client.right - client.left, client.bottom - client.top, g_hdcBuffer, 0, 0, 160, 100, SRCCOPY);
}

void DestroyGraphics()
{
    g_hwnd = 0;
}
