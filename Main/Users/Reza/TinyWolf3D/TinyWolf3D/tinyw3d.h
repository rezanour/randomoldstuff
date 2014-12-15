#ifndef _TINYW3D_H_
#define _TINYW3D_H_

/*

NOTE:
    This is not a fully functional Wolfenstein 3D port. This is a staging area for forumlating
    and optimizing algorithms and data structures for the XGameStation port of Wolfenstein 3D.

    Specs of the XGameStation ME that we're targeting:

    * 4k of ROM for code AND data. You read that right. Targeting 2k for code and 2k for all assets
    * 128k of RAM at runtime, including frame buffer. We can decompress our assets into here for runtime.
    * No dedicated pixel hardware. We have to compute color burst and brightness (chroma and luma) signals.
    * 80Mhz processor, we need to be very very careful with how many instructions certain things take.
    * 8-bit processor (registers). We have to be crafty about addressing schemes and multibyte math/reads/writes

*/

/* COMMON */
typedef unsigned char byte;
typedef signed char sbyte;

/* GRAPHICS */

/* HACK HACK - For testing on Windows, we use GDI to render our buffer to the screen and validate our algorithms */
void InitializeGraphics(void* hwnd);
void DestroyGraphics();

void W3DSetPixel(short x, short y, byte color);
void Present();


/* GAME LOOP */
void InitializeGame();
void TickGame();

void ToggleDebug();

void MovePlayer(short dist);
void TurnPlayer(unsigned short angle);

#endif // _TINYW3D_H_
