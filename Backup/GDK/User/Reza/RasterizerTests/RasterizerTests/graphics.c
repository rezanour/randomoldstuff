#include "common.h"

void RClearBuffer(color_t color)
{
    int i;
    color_t* p = g_pixelBuffer;

    for (i = 0; i < g_screenWidth * g_screenHeight; ++i)
    {
        *p++ = color;
    }
}

void RSetPixel(int x, int y, color_t color)
{
    g_pixelBuffer[y * g_screenWidth + x] = color;
}

void RDrawLine(int x0, int y0, int x1, int y1, color_t color)
{
    int dx, dy, incE, incNE, d, x, y;

    dx = x1 - x0;
    dy = y1 - y0;

    d = 2 * dy - dx;

    incE = 2 * dy;
    incNE = 2 * (dy - dx);

    x = x0;
    y = y0;

    RSetPixel(x, y, color);

    while (x < x1)
    {
        if (d < 0)
        {
            d -= incE;
        }
        else
        {
            d -= incNE;
            --y;
        }

        ++x;

        RSetPixel(x, y, color);
    }
}
