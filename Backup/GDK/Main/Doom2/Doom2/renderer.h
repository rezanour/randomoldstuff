#ifndef _RENDERER_H_
#define _RENDERER_H_

//
// RENDERER
//
// The Renderer module contains methods pertaining to building a pseudo 3D image
// of the player's view.
//

BOOL RendererInitialize();
void RendererUninitialize();

// consumes the current state of the map and runtime to render a view of the 
// world as seen from the player's perspective
void Render3DView();


#endif // _RENDERER_H_