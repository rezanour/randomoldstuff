#ifndef _RASTERIZER_H_
#define _RASTERIZER_H_

//
// NOTE: Rasterizes in bytes (as stored in images). Up to caller to map that something (either 8-bit display or 32bpp using palette)
//

byte_t RastInit(byte_t* frame);
void RastExit();

void RastView3D(const byte_t* map, byte_t posCellX, byte_t posFineX, byte_t posCellY, byte_t posFineY, byte_t angle);

#endif // _RASTERIZER_H_
