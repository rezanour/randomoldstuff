#ifndef _RASTERIZER_H_
#define _RASTERIZER_H_

//
// RASTERIZER
//
// The Rasterizer module contains methods for rasterizing output
// to the frame buffer of the graphics system. This includes drawing
// pixels, lines, shapes, and images.
//

// Write a single pixel to the frame buffer, using the currently selected palette
void RasterizePixel(int x, int y, byte_t color);

// Draw a line from one pixel to another
void RasterizeLine(int xStart, int yStart, int xEnd, int yEnd, byte_t color);

void RasterizeSetColormap(byte_t index);

#endif // _RASTERIZER_H_