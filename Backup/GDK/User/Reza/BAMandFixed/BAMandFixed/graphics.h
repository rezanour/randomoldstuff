#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

BOOL GraphicsInitialize(int width, int height);
void GraphicsUninitialize();

void GraphicsClear();

void GraphicsPresent();

#endif // _GRAPHICS_H_