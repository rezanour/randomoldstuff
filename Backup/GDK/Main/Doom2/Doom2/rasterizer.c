#include "common.h"
#include "graphicsp.h"
#include "images.h"

//
// Public Rasterizer Methods
//

void RasterizePixel(int x, int y, byte_t color)
{
    assert(g_frame.Color);

    if (x >= 0 && x < g_frame.Width && y >= 0 && y < g_frame.Height)
    {
        g_frame.Color[y * g_frame.Width + x] = g_palettes[g_currentPalette][color];
    }
}

void RasterizeLine(int xStart, int yStart, int xEnd, int yEnd, byte_t color)
{
    static short multiplier = 2000; // used to avoid floats
    short x = xStart;
    short y = yStart;
    short dx = xEnd - xStart;
    short dy = yEnd - yStart;
    short slope;
    short change;
    short progress;
    short dir;

    if (dx == 0 && dy == 0)
    {
        // optimized case (dot)
        RasterizePixel(x, y, color);
    }
    else if (dx == 0)
    {
        // optimized case (straight vertical line)
        if (y > yEnd)
        {
            SWAP(y, yEnd);
        }

        while (y <= yEnd)
        {
            RasterizePixel(x, y++, color);
        }
    }
    else if (dy == 0)
    {
        // optimized case (horizontal line)
        if (x > xEnd)
        {
            SWAP(x, xEnd);
        }

        while (x <= xEnd)
        {
            RasterizePixel(x++, y, color);
        }
    }
    else if (abs(dx) >= abs(dy))
    {
        // x change is larger than y, which means we have 1 pixel for each x

        // ensure our loop moves forward
        if (x > xEnd)
        {
            SWAP(x, xEnd);
            SWAP(y, yEnd);
        }

        dx = xEnd - x;
        dy = yEnd - y;

        slope = multiplier * dy / dx;
        change = abs(slope);
        progress = 0;
        dir = slope / change;

        while (x <= xEnd)
        {
            RasterizePixel(x++, y, color);

            progress += change;
            if (progress >= multiplier)
            {
                progress -= multiplier;
                y += dir;
            }
        }
    }
    else
    {
        // y change is larger than x, which means we have 1 pixel for each y

        // ensure our loop moves forward
        if (y > yEnd)
        {
            SWAP(x, xEnd);
            SWAP(y, yEnd);
        }

        dx = xEnd - x;
        dy = yEnd - y;

        slope = multiplier * dx / dy;
        change = abs(slope);
        progress = 0;
        dir = slope / change;

        while (y <= yEnd)
        {
            RasterizePixel(x, y++, color);

            progress += change;
            if (progress >= multiplier)
            {
                progress -= multiplier;
                x += dir;
            }
        }
    }
}

void RasterizeSetColormap(byte_t index)
{
    assert(index < ARRAYSIZE(g_colormaps));

    g_currentColormap = index;
}

//
// Private Rasterizer Methods
//

float RasterizeGetDepthAtPixel(int x, int y)
{
    return g_frame.Depth[y * g_frame.Width + x];
}

void RasterizePixelWithDepth(int x, int y, byte_t color, float z)
{
    int index = y * g_frame.Width + x;

    RasterizePixel(x, y, g_colormaps[g_currentColormap][color]);
    g_frame.Depth[index] = z;
}
