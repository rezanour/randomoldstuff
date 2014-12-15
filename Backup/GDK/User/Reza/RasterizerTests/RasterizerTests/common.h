#ifndef _COMMON_H_
#define _COMMON_H_

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

typedef unsigned char byte_t;

typedef struct
{
    byte_t b;
    byte_t g;
    byte_t r;
} color_t;

extern HWND g_hwnd;
extern color_t* g_pixelBuffer;

extern int g_screenWidth;
extern int g_screenHeight;


void RClearBuffer(color_t color);
void RSetPixel(int x, int y, color_t color);
void RDrawLine(int x0, int y0, int x1, int y1, color_t color);


#endif // _COMMON_H_