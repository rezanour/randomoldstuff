#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

//
// GRAPHICS
//
// The Graphics module contains basic graphics system methods.
// These include setting up the screen resolution and size, 
// handling of frame buffers, and handling of the viewport.
//

// Initialize the graphics system. The parameters control
// the dimensions of the frame buffer, which is independent of 
// the window size. The contents are then stretched to fit the window.
BOOL GraphicsInitialize(int width, int height);
void GraphicsUninitialize();

// Get the dimensions of the frame buffer
short GraphicsGetScreenWidth();
short GraphicsGetScreenHeight();

// set the viewport size. valid values are 1 -> 10, with 1 being smallest, 10 being fullscreen
byte_t GraphicsGetViewSize();
void GraphicsSetViewSize(byte_t size);
void GraphicsGetViewport(PRECT viewport);

// clears the entire frame buffer
void GraphicsClear(byte_t color);

// clears only the viewport
void GraphicsClearViewport(byte_t color);

// updates the window with the current state of the frame buffer
void GraphicsPresent();

void GraphicsDrawSprite(int x, int y,  resource_t resource);

#endif // _GRAPHICS_H_