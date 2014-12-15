#ifndef _COMMON_H_
#define _COMMON_H_

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <Windows.h>

#include <assert.h>

#define ENABLE_TESTS

#include "fixedpoint.h"
#include "fixedangle.h"
#include "graphics.h"

#define CELLWIDTH       64
#define CELLHEIGHT      64
#define MAPCELLSHORIZ   64
#define MAPCELLSVERT    64
#define MAPWIDTH        CELLWIDTH * MAPCELLSHORIZ
#define MAPHEIGHT       CELLHEIGHT * MAPCELLSVERT

#endif // _COMMON_H_