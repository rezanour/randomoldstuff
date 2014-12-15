#include "common.h"
#include "graphics.h"
#include "debug.h"

static struct
{
    short Left;
    short Top;
    short Width;
    short Height;
    HWND Window;
    HDC DC;
    paletteentry_t* Pixels;
} g_Canvas = {0};

static paletteentry_t g_Palette[256] = {0};

//
// Private Method Declarations
//
static void InitializePalette();
static void DrawPalette();
static void DrawFilledRectangle(short x, short y, short width, short height, color_t value);


//
// Public Methods
//

BOOL InitializeGraphics(HWND hwnd, short width, short height)
{
    HDC windowDC;
    BITMAPINFO bmi = {0};
    HBITMAP bitmap;

    // if we're already initialized, delete the old DC
    if (g_Canvas.DC)
    {
        // TODO: Should we save and transfer contents?
        DeleteDC(g_Canvas.DC);
        g_Canvas.DC = NULL;
        g_Canvas.Pixels = NULL;
    }

    // Store the HWND for later
    g_Canvas.Window = hwnd;

    g_Canvas.Left = 0;
    g_Canvas.Top = 0;
    g_Canvas.Width = width;
    g_Canvas.Height = height;

    // Get the window's DC and create a compatible one to it for our offscreen surface
    windowDC = GetDC(hwnd);
    if (!windowDC)
    {
        DebugOut("InitializeGraphics: Failed to get window DC\n");
        return FALSE;
    }

    g_Canvas.DC = CreateCompatibleDC(windowDC);
    if (!g_Canvas.DC)
    {
        DebugOut("InitializeGraphics: Failed to create memory DC\n");
        ReleaseDC(hwnd, windowDC);
        return FALSE;
    }

    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = g_Canvas.Width;
    bmi.bmiHeader.biHeight = -g_Canvas.Height; // negative because bitmaps go bottom up, we want top down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 24;
    bmi.bmiHeader.biCompression = BI_RGB;

    bitmap = CreateDIBSection(g_Canvas.DC, &bmi, DIB_RGB_COLORS, (PVOID*)&g_Canvas.Pixels, NULL, 0);
    if (!bitmap)
    {
        DebugOut("InitializeGraphics: Failed to create DIB section\n");
        DeleteDC(g_Canvas.DC);
        ReleaseDC(hwnd, windowDC);
        g_Canvas.DC = NULL;
        return FALSE;
    }

    SelectObject(g_Canvas.DC, bitmap);
    DeleteObject(bitmap);

    ReleaseDC(hwnd, windowDC);

    InitializePalette();

    return TRUE;
}

void UninitializeGraphics()
{
    if (g_Canvas.DC)
    {
        DeleteDC(g_Canvas.DC);
        g_Canvas.DC = NULL;
        g_Canvas.Pixels = NULL;
    }
}

void GraphicsRefresh()
{
    HDC windowDC = GetDC(g_Canvas.Window);
    RECT clientBounds;

    GetClientRect(g_Canvas.Window, &clientBounds);

    DrawPalette();

    StretchBlt(
        windowDC, 
        clientBounds.left, 
        clientBounds.top, 
        clientBounds.right - clientBounds.left, 
        clientBounds.bottom - clientBounds.top,
        g_Canvas.DC,
        0,
        0,
        g_Canvas.Width,
        g_Canvas.Height,
        SRCCOPY);
}

void GraphicsGetCanvasDimensions(PRECT canvasDimensions)
{
    canvasDimensions->left = g_Canvas.Left;
    canvasDimensions->top = g_Canvas.Top;
    canvasDimensions->right = g_Canvas.Left + g_Canvas.Width;
    canvasDimensions->bottom = g_Canvas.Top + g_Canvas.Height;
}

void GraphicsSetCurrentPalette(paletteentry_t* entries, short numEntries)
{
    UNREFERENCED_PARAMETER(entries);
    UNREFERENCED_PARAMETER(numEntries);
}

void GraphicsGetCurrentPalette(paletteentry_t* entries, short maxEntries, short* numEntries)
{
    UNREFERENCED_PARAMETER(entries);
    UNREFERENCED_PARAMETER(maxEntries);
    UNREFERENCED_PARAMETER(numEntries);
}

color_t GraphicsGetPixel(short x, short y)
{
    color_t value = {0};

    UNREFERENCED_PARAMETER(x);
    UNREFERENCED_PARAMETER(y);

    return value;
}

void GraphicsSetPixel(short x, short y, color_t value)
{
    g_Canvas.Pixels[y * g_Canvas.Width + x] = g_Palette[value];
}


//
// Private Methods
//

void InitializePalette()
{
    byte_t r, g, b;
    int i = 0;

    // generate some gradients of color
    for (r = 0; r < 6; ++r)
    {
        for (g = 0; g < 6; ++g)
        {
            for (b = 0; b < 6; ++b)
            {
                g_Palette[i].b = b * 40;
                g_Palette[i].g = g * 40;
                g_Palette[i].r = r * 40;
                ++i;
            }
        }
    }

    // fill remaining colors with white for now
    for (; i < ARRAYSIZE(g_Palette); ++i)
    {
        g_Palette[i].b = 255;
        g_Palette[i].g = 255;
        g_Palette[i].r = 255;
    }
}

void DrawPalette()
{
    short x, y;
    color_t i;

    i = 0;

    for (y = 0; y < 32; y += 8)
    {
        for (x = 0; x < 512; x += 8)
        {
            DrawFilledRectangle(x, y, 8, 8, i);
            ++i;
        }
    }
}

void DrawFilledRectangle(short x, short y, short width, short height, color_t value)
{
    short cx, cy;

    for (cy = y; cy < y + height; ++cy)
    {
        for (cx = x; cx < x + width; ++cx)
        {
            GraphicsSetPixel(cx, cy, value);
        }
    }
}
