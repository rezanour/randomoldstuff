#ifndef _GRAPHICSP_H_
#define _GRAPHICSP_H_

//
// PRIVATE graphics methods. Only for use between graphics modules.
//


//
// COLOR
//

// simple 24bpp (no alpha) color format used 
typedef struct
{
    byte_t b;
    byte_t g;
    byte_t r;
} color_t;


//
// PALETTES
//
// the different palettes are used for different scenarios:
//
// normally, we stay on the normal palette. However, for certain effects, 
// such as taking damage or picking up items, we want to the screen to flash
// colors (pulse red when you take damage, for example).
//
// This is done by cycling palettes. The palettes are all copies of each other (same
// basic color layout), the only difference is that they take on various tints/shades.
//
// so, to pulse the screen red, you would animate the current palette from index 2 -> 8
// and then back to 2, then switch back to normal (0).
//

#define PALETTE_NORMAL      0
#define PALETTE_REDSTART    2           // Red shades are for taking damage
#define PALETTE_REDCOUNT    7
#define PALETTE_YELLOWSTART 10          // yellow shades are for picking up powerup
#define PALETTE_YELLOWCOUNT 3
#define PALETTE_GREEN       13          // green palette is for hazard suit
#define PALETTE_MAX         14

extern color_t g_palettes[PALETTE_MAX][256];
extern byte_t g_currentPalette;


//
// COLORMAPS
//
// colormaps are a level of indirection over the palette, that map
// an input palette index to an output palette index, normally taking
// lighting into account to pick another shade of the same color.
//
// colormaps ranging from brightest to darkest are used to map lighting
// for a sector. For example, if you're sector is 50% dark, you would 
// map any sprites you draw in that sector through colormap 16 (50% between BRIGHTEST & DARKEST)
//

#define COLORMAP_BRIGHTEST          0
#define COLORMAP_DARKEST            31
#define COLORMAP_INVULNERABILITY    32
#define COLORMAP_MAX                33

extern byte_t g_colormaps[COLORMAP_MAX][256];
extern byte_t g_currentColormap;


//
// FRAME BUFFER
//

typedef struct
{
    short Width;
    short Height;
    color_t* Color;
    float* Depth;
} framebuffer_t;

extern framebuffer_t g_frame;


//
// VIEWPORT
//

typedef struct
{
    short Left;
    short Top;
    short Width;
    short Height;
} viewport_t;

extern viewport_t g_viewport;
extern short g_hudHeight;


void GraphicsClearDepthBuffer();
float RasterizeGetDepthAtPixel(int x, int y);
void RasterizePixelWithDepth(int x, int y, byte_t color, float z);

#endif // _GRAPHICSP_H_