#ifndef _DGSURFACE_H_
#define _DGSURFACE_H_

typedef struct
{
    short width;
    short height;
    byte_t bpp;
    void* pixels;
} surface_t;

//
// Basic creation and destruction
//

surface_t* DGCreateSurface(short width, short height, byte_t bpp);
void DGDestroySurface(surface_t* surface);


//
// Bit block transfers
//

void DGBitBlt(surface_t* dest, short xDest, short yDest, surface_t* src, short xSrc, short ySrc, short width, short height);


#endif // _DGSURFACE_H_