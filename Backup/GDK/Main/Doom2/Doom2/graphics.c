#include "common.h"
#include "graphicsp.h"
#include "images.h"


color_t g_palettes[PALETTE_MAX][256] = {0};
byte_t g_currentPalette = 0;

byte_t g_colormaps[COLORMAP_MAX][256] = {0};
byte_t g_currentColormap = 0;

framebuffer_t g_frame = {0};

viewport_t g_viewport = {0};
short g_hudHeight = 32;

static HDC g_frameBufferDC = NULL;
static float g_aspectRatio = 1;


//
// private method declarations
//

static BOOL GraphicsLoadPalettesAndColormaps();

//
// public graphics methods
//

BOOL GraphicsInitialize(int width, int height)
{
    HWND hwnd = GameGetWindow();
    HDC windowDC = NULL;

    assert(hwnd);

    if (g_frame.Color)
    {
        DebugOut("Graphics: Already Initialized!");
        return FALSE;
    }

    if (!GraphicsLoadPalettesAndColormaps())
    {
        return FALSE;
    }

    // Ensure image system is initialized as well
    if (!ImagesInitialize())
    {
        return FALSE;
    }

    g_frame.Width = width;
    g_frame.Height = height;

    // we start by getting the main window's DC, and creating one that's compatible.
    // this will ensure that we get proper behavior and color translation on blts.
    windowDC = GetDC(hwnd);
    g_frameBufferDC = CreateCompatibleDC(windowDC);

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
        bitmap = CreateDIBSection(g_frameBufferDC, &bmi, DIB_RGB_COLORS, (PVOID*)&g_frame.Color, NULL, 0);
        if (bitmap)
        {
            g_frame.Depth = (float*)MemoryAlloc("depth buffer", sizeof(float) * g_frame.Width * g_frame.Height);
        }

        // select the bitmap format into the DC, which effectively sets it and initializes the color conversion tables.
        SelectObject(g_frameBufferDC, bitmap);

        // delete off the bitmap info (it's already selected into the DC, we don't need to keep our copy)
        DeleteObject(bitmap);
    }

    ReleaseDC(hwnd, windowDC);

    g_aspectRatio = width / (float)(height - g_hudHeight);
    GraphicsSetViewSize(10);

    DebugOut("Graphics initialized successfully");

    return TRUE;
}

void GraphicsUninitialize()
{
    if (g_frame.Depth)
    {
        MemoryFree(g_frame.Depth);
        g_frame.Depth = NULL;
    }

    if (g_frameBufferDC)
    {
        DeleteDC(g_frameBufferDC);
        g_frameBufferDC = NULL;
        g_frame.Color = NULL;
    }

    ImagesUninitialize();

    DebugOut("Graphics uninitialized");
}

short GraphicsGetScreenWidth()
{
    return g_frame.Width;
}

short GraphicsGetScreenHeight()
{
    return g_frame.Height;
}

byte_t GraphicsGetViewSize()
{
    return g_viewport.Width / (g_frame.Width / 10);
}

void GraphicsSetViewSize(byte_t size)
{
    assert(size >= 1 && size <= 10);

    g_viewport.Width = size * (g_frame.Width / 10);
    g_viewport.Height = (uint_t)(g_viewport.Width / g_aspectRatio);
    g_viewport.Left = (g_frame.Width - g_viewport.Width) / 2;
    g_viewport.Top = (g_frame.Height - g_hudHeight - g_viewport.Height) / 2;
}

void GraphicsGetViewport(PRECT viewport)
{
    viewport->left = (g_frame.Width - g_viewport.Width) / 2;
    viewport->right = viewport->left + g_viewport.Width;
    viewport->top = (g_frame.Height - g_hudHeight - g_viewport.Height) / 2;
    viewport->bottom = viewport->top + g_viewport.Height;
}

void GraphicsClear(byte_t color)
{
    int x, y;

    for (y = 0; y < g_frame.Height; ++y)
    {
        for (x = 0; x < g_frame.Width; ++x)
        {
            RasterizePixel(x, y, color);
        }
    }
}

void GraphicsClearViewport(byte_t color)
{
    int x, y;
    int yEnd = g_viewport.Top + g_viewport.Height;
    int xEnd = g_viewport.Left + g_viewport.Width;

    for (y = g_viewport.Top; y < yEnd; ++y)
    {
        for (x = g_viewport.Left; x < xEnd; ++x)
        {
            RasterizePixel(x, y, color);
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
            g_frameBufferDC,
            0, 0,
            g_frame.Width,
            g_frame.Height,
            SRCCOPY);
    }

    ReleaseDC(hwnd, windowDC);
}

//
// private graphics methods
//

void GraphicsClearDepthBuffer()
{
    int x, y;

    for (y = 0; y < g_frame.Height; ++y)
    {
        for (x = 0; x < g_frame.Width; ++x)
        {
            g_frame.Depth[y * g_frame.Width + x] = 999999.0f;
        }
    }
}


// This version maps lighting via the current colormap before applying the pixel
void GraphicsSetPixelWithColormap(int x, int y, byte_t color)
{
    RasterizePixel(x, y, g_colormaps[g_currentColormap][color]);
}

BOOL GraphicsLoadPalettesAndColormaps()
{
    int i;

    color_t* palettes;
    long palettesSize;
    byte_t numPalettes;

    byte_t* colormaps;
    long colormapsSize;
    byte_t numColormaps;

    if (!WadGetLump("PLAYPAL", (void**)&palettes, &palettesSize))
    {
        return FALSE;
    }

    if (!WadGetLump("COLORMAP", (void**)&colormaps, &colormapsSize))
    {
        MemoryFree(palettes);
        return FALSE;
    }

    // everything loaded okay, now copy it out to our own data

    ZeroMemory(g_palettes, sizeof(g_palettes));
    ZeroMemory(g_colormaps, sizeof(g_colormaps));

    numPalettes = (byte_t)min(ARRAYSIZE(g_palettes), palettesSize / 768); // each palettes is 3 bytes (r, g, b) * 256 entries
    numColormaps = (byte_t)min(ARRAYSIZE(g_colormaps), colormapsSize / 256); // each colormap is 1 byte * 256 entries

    // first, the palettes
    for (i = 0; i < numPalettes; ++i)
    {
        int j;
        for (j = 0; j < 256; ++j)
        {
            // we need to flip R & B channels (since we're backwards to match GDI)
            color_t c = palettes[i * 256 + j];
            g_palettes[i][j].r = c.b;
            g_palettes[i][j].g = c.g;
            g_palettes[i][j].b = c.r;
        }
    }

    MemoryFree(palettes);

    // then, the colormaps
    for (i = 0; i < numColormaps; ++i)
    {
        int j;
        for (j = 0; j < 256; ++j)
        {
            g_colormaps[i][j] = colormaps[i * 256 + j];
        }
    }

    MemoryFree(colormaps);

    return TRUE;
}

//
// debug methods
//

void DebugGraphicsDrawPalette(byte_t paletteIndex, int x, int y, int size)
{
#ifndef NDEBUG

    int x1, y1, i;
    byte_t old = g_currentPalette;

    g_currentPalette = paletteIndex;

    i = 0;
    for (y1 = y; y1 < y + 16 * size; y1+=size)
    {
        for (x1 = x; x1 < x + 16 * size; x1+=size)
        {
            int a, b;

            for (a = 0; a < size; ++a)
            {
                for (b = 0; b < size; ++b)
                {
                    RasterizePixel(x1 + a, y1 + b, i);
                }
            }

            ++i;
        }
    }

    g_currentPalette = old;

#endif
}

void DebugGraphicsSetColormap(byte_t index)
{
#ifndef NDEBUG

    assert(index < ARRAYSIZE(g_colormaps));
    g_currentColormap = index;

#endif
}

void DebugGraphicsTestLines()
{
#ifndef NDEBUG

    byte_t color = 128;

    // square
    RasterizeLine(5, 5, 15, 5, color);
    RasterizeLine(15, 5, 15, 15, color);
    RasterizeLine(15, 15, 5, 15, color);
    RasterizeLine(5, 15, 5, 5, color);

    // diamond
    RasterizeLine(25, 15, 35, 5, color);
    RasterizeLine(35, 5, 45, 15, color);
    RasterizeLine(45, 15, 35, 25, color);
    RasterizeLine(35, 25, 25, 15, color);

#endif
}

