#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

typedef byte_t color_t;

typedef struct
{
    byte_t b;
    byte_t g;
    byte_t r;
} paletteentry_t;

BOOL InitializeGraphics(HWND hwnd, short width, short height);
void UninitializeGraphics();

void GraphicsRefresh();
void GraphicsGetCanvasDimensions(PRECT canvasDimensions);

void GraphicsSetCurrentPalette(paletteentry_t* entries, short numEntries);
void GraphicsGetCurrentPalette(paletteentry_t* entries, short maxEntries, short* numEntries);

color_t GraphicsGetPixel(short x, short y);
void GraphicsSetPixel(short x, short y, color_t value);

#endif // _GRAPHICS_H_