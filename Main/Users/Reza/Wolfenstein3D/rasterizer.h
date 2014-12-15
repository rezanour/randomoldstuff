#ifndef _RASTERIZER_H_
#define _RASTERIZER_H_

//
// NOTE: Rasterizes bytes as stored in images. Up to caller to handle palette
//

int RastInit(byte_t* frame);
void RastExit();

void RastSetMap(const unsigned short* map);
void RastView3D(short viewX, short viewY, angle_t viewAngle);

#endif // _RASTERIZER_H_
