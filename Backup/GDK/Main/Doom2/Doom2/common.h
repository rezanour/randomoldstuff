#ifndef _COMMON_H_
#define _COMMON_H_

// windows
#define WIN32_LEAN_AND_MEAN
#define VCEXTRALEAN
#include <Windows.h>

// standard library and math
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

//#define USE_RENDEREZA

// game
#include "types.h"
#include "list.h"
#include "memorymanager.h"
#include "resourcemanager.h"
#include "wad.h"
#include "audio.h"
#include "input.h"
#include "gametime.h"
#include "game.h"
#include "debug.h"
#include "linkedlist.h"
#include "vectormath.h"
#include "map.h"
#include "player.h"
#include "graphics.h"
#include "renderer.h"
#include "rasterizer.h"
#include "bsp.h"
#include "menu.h"
#include "hud.h"
#include "messages.h"

#define NAME_SIZE   8
#define FONTCHARACTER_WIDTH 8

#define SWAP(a, b) { int temp = a; a = b; b = temp; }
#define FSWAP(a, b) { float temp = a; a = b; b = temp; }

#define SAFEFREE(x) if (x) { MemoryFree((x)); x = NULL; }

#define TORADIANS(x) (x) * 3.1415f / 180.0f

#define SIGN(x) (int)(x) / (int)abs((int)(x))
#define FSIGN(x) (float)(x) / (float)fabs((float)(x))

#endif // _COMMON_H_