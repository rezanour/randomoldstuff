#include "directgraphics.h"
#include "internal.h"

//
// public surface methods
//

surface_t* DGCreateSurface(short width, short height, byte_t bpp)
{
    uint_t bufferSize;
    surface_t* surface;

    // validate parameters
    if (width <= 0 || height <= 0 || !DGIsValidBpp(bpp))
    {
        DGDebugOut("invalid surface parameters (%d, %d, %d).\n", width, height, bpp);
        return NULL;
    }

    // allocate the surface
    surface = (surface_t*)DGAlloc("surface", sizeof(surface_t));
    if (!surface)
    {
        DGDebugOut("Failed to allocate surface.\n");
        return NULL;
    }

    // fill in the surface info
    surface->width = width;
    surface->height = height;
    surface->bpp = bpp;

    // compute the size of the buffer to hold the pixel data and allocate it
    bufferSize = width * height * (bpp / 8);

    surface->pixels = DGAlloc("pixel data", bufferSize);
    if (!surface->pixels)
    {
        DGDebugOut("Failed to allocate pixel data.\n");
        DGFree(surface);
        return NULL;
    }

    return surface;
}

void DGDestroySurface(surface_t* surface)
{
    assert(surface);
    if (surface)
    {
        if (surface->pixels)
        {
            DGFree(surface->pixels);
        }
        DGFree(surface);
    }
}

void DGBitBlt(surface_t* dest, short xDest, short yDest, surface_t* src, short xSrc, short ySrc, short width, short height)
{
    assert(dest && src);

    if (dest && src)
    {
        assert(dest->bpp == src->bpp);

        if (dest->bpp == src->bpp);
        {
            short xEnd = min(xDest + width, dest->width);
            short yEnd = min(yDest + height, dest->height);
            short pixelStride = xEnd - xDest;
            short pixelSize = dest->bpp / 8;
            short byteStride = pixelStride * pixelSize;

            // TODO: optimize this to take advantage of machine's native addressing size (for example, transfer in DWORD or INT64 sized blocks)
            short i, y;

            for (y = yDest; y < yEnd; ++y)
            {
                byte_t* d = (byte_t*)dest->pixels + (y * dest->width + xDest);
                byte_t* s = (byte_t*)src->pixels + ((ySrc + y - yDest) * src->width + xSrc);

                for (i = 0; i < byteStride; ++i)
                {
                    *d++ = *s++;
                }
            }
        }
    }
}
