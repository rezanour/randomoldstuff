#ifndef _COMMON_H_
#define _COMMON_H_

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define PI_RADIANS  3.14159f
#define MAX_SHORT 0x7FFF

typedef unsigned char byte_t;
typedef long long int64_t;
typedef struct
{
    short x;
    short y;
} point_t;

#include "fixed.h"
#include "bam.h"
#include "gametime.h"
#include "input.h"
#include "graphics.h"

#endif // _COMMON_H_