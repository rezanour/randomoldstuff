#ifndef _WOLF3D_H_
#define _WOLF3D_H_

//
// Typedefs
//

typedef unsigned char byte_t;
typedef long fixed_t;
typedef unsigned short angle_t;     // Only 12bits are used for Binary Angle Measurements (BAM)

//
// Constants
//

#define GAME_TITLE      "Wolfenstein3D"

#define SCREEN_WIDTH    320
#define SCREEN_HEIGHT   200

#define CELL_SIZE       64
#define CELL_SIZE_SHIFT 6
#define NUM_CELLS       64

#define MAX_ANGLES      4096
#define FOV             640 // 56.25 degrees in BAM. 60 is ideal, but doesn't lend itself well to 320pixel width screen
#define DIST_TO_PLANE   299 // half screen width / tan (half FOV)
#define ANGLE_PER_COL   2

#define ANG_90          1024
#define ANG_180         2048
#define ANG_270         3072

//
// BAMs
//

#define ADD_ANGLES(a, b)    ((a + b) % MAX_ANGLES)
#define SUB_ANGLES(a, b)    (a >= b ? (a - b) : (MAX_ANGLES + a - b))

//
// General
//
#define ABS(x) ((x) < 0 ? -(x) : (x))

#endif // _WOLF3D_H_
