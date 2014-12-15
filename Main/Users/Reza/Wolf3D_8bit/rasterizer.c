#include <malloc.h>
#include "wolf3d.h"
#include "rasterizer.h"

//
// Globals
//

static byte_t* g_frame;     // frame buffer
static byte_t g_ceilingColor = 24;
static byte_t g_floorColor = 25;

static const byte_t* g_map;
static byte_t g_posCellX;
static byte_t g_posFineX;
static byte_t g_posCellY;
static byte_t g_posFineY;
static byte_t g_angle;

// only store for first 90 degrees
static byte_t g_64OverTanTable[64];
static byte_t g_64TanTable[64];
static byte_t g_oneOverTanTable[64];
static byte_t g_16TanTable[64];

//
// Declarations
//

static void RasterizeColumn(byte_t x, byte_t angle);
static void InitializeTables();

//
// Public Methods
//

byte_t RastInit(byte_t* frame)
{
    g_frame = frame;

    InitializeTables();

    return 0;
}

void RastExit()
{
    g_frame = 0;
}

void RastView3D(const byte_t* map, byte_t posCellX, byte_t posFineX, byte_t posCellY, byte_t posFineY, byte_t angle)
{
    byte_t x;
    byte_t startAng;
    byte_t ang;

    g_map = map;
    g_posCellX = posCellX;
    g_posFineX = posFineX;
    g_posCellY = posCellY;
    g_posFineY = posFineY;
    g_angle = angle;

    startAng = angle + FOV / 2;

    for (x = 0; x < SCREEN_WIDTH; ++x)
    {
        ang = startAng - (x / COL_PER_ANGLE);
        RasterizeColumn(x, ang);
    }
}

//
// Public Methods
//

void RasterizeColumn(byte_t x, byte_t angle)
{
    byte_t y;
    byte_t cx, cy;  // cell being tested

    cx = g_posCellX;
    cy = g_posCellY;

    if (angle == 64 || angle == 192)
    {
        // Horizontal special cases
        if (angle == 64)
        {
            // straight up
            while (cy-- > 0)
            {
                // Test cell cx, cy
            }
        }
        else
        {
            // straight down
            while (cy++ < 64)
            {
                // Test cell cx, cy
            }
        }
    }
    else if (angle == 0 || angle == 128)
    {
        // Vertical special cases
        if (angle == 0)
        {
            // straight right
            while (cx++ < 64)
            {
                // Test cell cx, cy
            }
        }
        else
        {
            // straight left
            while (cx-- > 0)
            {
                // Test cell cx, cy
            }
        }
    }
    else
    {
        // General cases
        byte_t dx, dy;      // step for each iteration of opposite points (dx used for finding Ys, and dy for Xs)
        byte_t cx2, cy2;    // used for finding Ys. cx, cy are used for Xs

        cx2 = cx;
        cy2 = cy;

        // reflect to first quadrant, we reverse directions when necessary
        if (angle < 64)
        {
            dx = g_64OverTanTable[angle];
            dy = g_64TanTable[angle];
        }
        else if (angle < 128)
        {
            dx = g_64OverTanTable[128 - angle];
            dy = g_64TanTable[128 - angle];
        }
        else if (angle < 192)
        {
            dx = g_64OverTanTable[angle - 128];
            dy = g_64TanTable[angle - 128];
        }
        else
        {
            dx = g_64OverTanTable[256 - angle];
            dy = g_64TanTable[256 - angle];
        }

        if (angle < 64)
        {
            // northeast case (both x increases, y decreases)
            while (cx++ < 64 && cy2-- > 0)
            {
            }
        }
        else if (angle < 128)
        {
            // northwest case
        }
        else if (angle < 192)
        {
            // southwest case
        }
        else
        {
            // southeast case
        }
    }

    // TODO: Implement. For now, just fill with ceiling & floor colors

    for (y = 0; y < SCREEN_HEIGHT / 2; ++y)
    {
        g_frame[y * SCREEN_WIDTH + x] = g_ceilingColor;
    }

    for (; y < SCREEN_HEIGHT; ++y)
    {
        g_frame[y * SCREEN_WIDTH + x] = g_floorColor;
    }
}

#include <math.h>
void InitializeTables()
{
    int i;
    static const float TwoPiOver256 = 6.28318f / 256.0f;

    for (i = 0; i < 64; ++i)
    {
        g_64OverTanTable[i] = (byte_t)(64.0f / tan(i * TwoPiOver256));
        g_64TanTable[i] = (byte_t)(64.0f * tan(i * TwoPiOver256));
        g_oneOverTanTable[64];
        g_16TanTable[64];
    }
}
