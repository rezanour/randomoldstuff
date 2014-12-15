#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#define SCREENWIDTH     320
#define SCREENHEIGHT    200
#define HUDHEIGHT       40

#define VIEWWIDTH       SCREENWIDTH
#define VIEWHEIGHT      SCREENHEIGHT - HUDHEIGHT

typedef struct
{
    short x;
    short y;
} point_t;

void InitializeTables();
void DDA(point_t p, angle_t view);

#endif // _GRAPHICS_H_