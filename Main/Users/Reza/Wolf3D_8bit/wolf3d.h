#ifndef _WOLF3D_H_
#define _WOLF3D_H_

//
// Typedefs
//

typedef unsigned char byte_t;

//
// Constants
//

#define GAME_TITLE      "Wolfenstein3D"
#define SCREEN_WIDTH    160
#define SCREEN_HEIGHT   100
#define CELL_SIZE       64
#define MAX_CELLS       64
#define FOV             84  // 60 degrees * 360/256 conversion
#define DIST_TO_PLANE   139 // half screen width / tan (half FOV)
#define COL_PER_ANGLE   2   // Since we have more pixels across than view angle, we need to double up pixels

#endif // _WOLF3D_H_
