#include "common.h"

typedef struct
{
    byte_t b;
    byte_t g;
    byte_t r;
} color_t;

typedef struct
{
    short Width;
    short Height;
    HDC DC;
    color_t* Pixels;
} framebuffer_t;

static framebuffer_t g_frame = {0};

// helpers
static void RasterizePixel(int x, int y, color_t color);

// external
HWND GameGetWindow();

BOOL GraphicsInitialize(int width, int height)
{
    HWND hwnd = GameGetWindow();
    HDC windowDC = NULL;

    assert(hwnd);

    g_frame.Width = width;
    g_frame.Height = height;

    // we start by getting the main window's DC, and creating one that's compatible.
    // this will ensure that we get proper behavior and color translation on blts.
    windowDC = GetDC(hwnd);
    g_frame.DC = CreateCompatibleDC(windowDC);

    {
        // we then describe our specific bitmap format (bitmap here is the general term, not the .BMP file format)
        // by selecting this into our DC, we can then draw into this DC with this bitmap format. Blting to the final 
        // DC will get translated appropriately by the system.
        BITMAPINFO bmi = {0};
        HBITMAP bitmap;

        // we only support 24bpp, so make sure our color struct hasn't strayed from that
        assert(sizeof(color_t) * 8 == 24);

        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = g_frame.Width;
        bmi.bmiHeader.biHeight = -g_frame.Height;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = sizeof(color_t) * 8;
        bmi.bmiHeader.biCompression = BI_RGB; // no compression

        // create dib section returns us a pointer to the frame buffer we just described, so we can draw directly
        // into it.
        bitmap = CreateDIBSection(g_frame.DC, &bmi, DIB_RGB_COLORS, (PVOID*)&g_frame.Pixels, NULL, 0);

        // select the bitmap format into the DC, which effectively sets it and initializes the color conversion tables.
        SelectObject(g_frame.DC, bitmap);

        // delete off the bitmap info (it's already selected into the DC, we don't need to keep our copy)
        DeleteObject(bitmap);
    }

    ReleaseDC(hwnd, windowDC);

    return TRUE;
}

void GraphicsUninitialize()
{
    if (g_frame.DC)
    {
        DeleteDC(g_frame.DC);
        g_frame.DC = NULL;
        g_frame.Pixels = NULL;
    }
}

void GraphicsClear()
{
    int x, y;
    color_t black = { 0, 0, 0 };

    for (y = 0; y < g_frame.Height; ++y)
    {
        for (x = 0; x < g_frame.Width; ++x)
        {
            RasterizePixel(x, y, black);
        }
    }
}

void GraphicsPresent()
{
    HWND hwnd = GameGetWindow();
    HDC windowDC = GetDC(hwnd);
    RECT clientRect;

    // because we fetch the HWND and clientRect on demand each present (they are cheap operations), we are flexible
    // enough to handle painting our output anywhere. The game could swap HWNDs, resize, etc... and we just keep drawing,
    // stretching as needed.
    if (GetClientRect(hwnd, &clientRect))
    {
        // TODO: StretchBlt has some wierd glitches/artifacts. We should consider writing our own later.
        // sometimes there will be columns of pixels that jiggle a little at certain scale factors.
        StretchBlt(
            windowDC,
            0, 0,
            clientRect.right - clientRect.left,
            clientRect.bottom - clientRect.top,
            g_frame.DC,
            0, 0,
            g_frame.Width,
            g_frame.Height,
            SRCCOPY);
    }

    ReleaseDC(hwnd, windowDC);
}

void RasterizePixel(int x, int y, color_t color)
{
    g_frame.Pixels[y * g_frame.Width + x] = color;
}
