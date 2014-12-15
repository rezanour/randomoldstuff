#include <malloc.h>
#include "wolf3d.h"
#include "rasterizer.h"

//
// Globals
//

static byte_t* g_frame;     // frame buffer
static byte_t g_ceilingColor = 24;
static byte_t g_floorColor = 25;

static const unsigned short* g_map;
static short g_viewX;
static short g_viewY;
static angle_t g_viewAngle;

static float g_Sine[MAX_ANGLES];
static float g_Cosine[MAX_ANGLES];
static float g_Tangent[MAX_ANGLES];

//
// Declarations
//

static void RasterizeColumn(short x, angle_t angle);
static void InitializeTables();

//
// Public Methods
//

int RastInit(byte_t* frame)
{
    g_frame = frame;

    InitializeTables();

    return 0;
}

void RastExit()
{
    g_frame = 0;
}

void RastSetMap(const unsigned short* map)
{
    g_map = map;
}

void RastView3D(short viewX, short viewY, angle_t viewAngle)
{
    short x;
    angle_t angle;

    g_viewX = viewX;
    g_viewY = viewY;
    g_viewAngle = viewAngle;

    angle = ADD_ANGLES(viewAngle, FOV / 2);

    for (x = 0; x < SCREEN_WIDTH; ++x)
    {
        angle = SUB_ANGLES(angle, ANGLE_PER_COL);
        RasterizeColumn(x, angle);
    }
}

//
// Public Methods
//

void RasterizeColumn(short x, angle_t angle)
{
    short y, lastStepX;
    float distX, distY;
    float deltaDistX, deltaDistY;
    float dist = -1;
    short cx, cy;
    short deltaCx, deltaCy;
    float cosA, sinA;

    cosA = g_Cosine[angle];
    sinA = g_Sine[angle];

    cx = g_viewX >> CELL_SIZE_SHIFT;
    cy = g_viewY >> CELL_SIZE_SHIFT;

#if 0
    // handle easy special cases (these cause errors in the general algorithm due to divide by 0)
    if (cosA == 0)
    {
        // vertical
        deltaCy = (angle < ANG_180) ? -1 : 1;
        while (cy >= 0 && cy < NUM_CELLS)
        {
            if (g_map[cy * NUM_CELLS + cx] > 0)
            {
                // hit N/S wall
                dist = ABS(cy * CELL_SIZE - g_viewY) << 10;
                break;
            }

            cy += deltaCy;
        }
    }
    else if (sinA == 0)
    {
        // horizontal
        deltaCx = (angle < ANG_90 || angle > ANG_270) ? 1 : -1;
        while (cx >= 0 && cx < NUM_CELLS)
        {
            if (g_map[cy * NUM_CELLS + cx] > 0)
            {
                // hit W/E wall
                dist = ABS(cx * CELL_SIZE - g_viewX) << 10;
                break;
            }

            cx += deltaCx;
        }
    }
    else
    {
        // general case

        // compute starting dists and delta dists based on quadrant
        if (angle < ANG_90)
        {
            deltaCx = 1;
            deltaCy = -1;
            distX = ((CELL_SIZE - g_viewX % CELL_SIZE) << 20) / cosA;
            deltaDistX = (CELL_SIZE << 20) / cosA;
            distY = ((g_viewY % CELL_SIZE) << 20) / sinA;
            deltaDistY = (CELL_SIZE << 20) / sinA;
        }
        else if (angle < ANG_180)
        {
            deltaCx = -1;
            deltaCy = -1;
            distX = ((g_viewX % CELL_SIZE) << 20) / -cosA;
            deltaDistX = (CELL_SIZE << 20) / -cosA;
            distY = ((g_viewY % CELL_SIZE) << 20) / sinA;
            deltaDistY = (CELL_SIZE << 20) / sinA;
        }
        else if (angle < ANG_270)
        {
            deltaCx = -1;
            deltaCy = 1;
            distX = ((g_viewX % CELL_SIZE) << 20) / -cosA;
            deltaDistX = (CELL_SIZE << 20) / -cosA;
            distY = ((CELL_SIZE - g_viewY % CELL_SIZE) << 20) / -sinA;
            deltaDistY = (CELL_SIZE << 20) / -sinA;
        }
        else
        {
            deltaCx = 1;
            deltaCy = 1;
            distX = ((CELL_SIZE - g_viewX % CELL_SIZE) << 20) / cosA;
            deltaDistX = (CELL_SIZE << 20) / cosA;
            distY = ((CELL_SIZE - g_viewY % CELL_SIZE) << 20) / -sinA;
            deltaDistY = (CELL_SIZE << 20) / -sinA;
        }

        for (;;)
        {
            if (distX <= distY)
            {
                cx += deltaCx;
                if (cx < 0 || cx >= NUM_CELLS)
                {
                    break;
                }

                distX += deltaDistX;
                lastStepX = 1;
            }
            else
            {
                cy += deltaCy;
                if (cy < 0 || cy >= NUM_CELLS)
                {
                    break;
                }

                distY += deltaDistY;
                lastStepX = 0;
            }

            if (g_map[cy * NUM_CELLS + cx] > 0)
            {
                // hit
                if (lastStepX)
                {
                    // W/E wall
                    dist = distX;
                }
                else
                {
                    // N/S wall
                    dist = distY;
                }
                break;
            }
        }
    }
#endif

    // handle easy special cases (these cause errors in the general algorithm due to divide by 0)
    if (angle == ANG_90 || angle == ANG_270)
    {
        // vertical
        deltaCy = (angle < ANG_180) ? -1 : 1;
        while (cy >= 0 && cy < NUM_CELLS)
        {
            if (g_map[cy * NUM_CELLS + cx] > 0)
            {
                // hit N/S wall
                dist = (float)ABS(cy * CELL_SIZE - g_viewY);
                break;
            }

            cy += deltaCy;
        }
    }
    else if (angle == 0 || angle == ANG_180)
    {
        // horizontal
        deltaCx = (angle < ANG_90 || angle > ANG_270) ? 1 : -1;
        while (cx >= 0 && cx < NUM_CELLS)
        {
            if (g_map[cy * NUM_CELLS + cx] > 0)
            {
                // hit W/E wall
                dist = (float)ABS(cx * CELL_SIZE - g_viewX);
                break;
            }

            cx += deltaCx;
        }
    }
    else
    {
        // general case

        // compute starting dists and delta dists based on quadrant
        if (angle < ANG_90)
        {
            deltaCx = 1;
            deltaCy = -1;
            distX = (CELL_SIZE - g_viewX % CELL_SIZE) * g_Tangent[angle];
            deltaDistX = CELL_SIZE * g_Tangent[angle];
            distY = (g_viewY % CELL_SIZE) / g_Tangent[angle];
            deltaDistY = CELL_SIZE / g_Tangent[angle];
        }
        else if (angle < ANG_180)
        {
            deltaCx = -1;
            deltaCy = -1;
            distX = (g_viewX % CELL_SIZE) / -cosA;
            deltaDistX = CELL_SIZE / -cosA;
            distY = (g_viewY % CELL_SIZE) / sinA;
            deltaDistY = CELL_SIZE / sinA;
        }
        else if (angle < ANG_270)
        {
            deltaCx = -1;
            deltaCy = 1;
            distX = ((g_viewX % CELL_SIZE) << 20) / -cosA;
            deltaDistX = (CELL_SIZE << 20) / -cosA;
            distY = ((CELL_SIZE - g_viewY % CELL_SIZE) << 20) / -sinA;
            deltaDistY = (CELL_SIZE << 20) / -sinA;
        }
        else
        {
            deltaCx = 1;
            deltaCy = 1;
            distX = ((CELL_SIZE - g_viewX % CELL_SIZE) << 20) / cosA;
            deltaDistX = (CELL_SIZE << 20) / cosA;
            distY = ((CELL_SIZE - g_viewY % CELL_SIZE) << 20) / -sinA;
            deltaDistY = (CELL_SIZE << 20) / -sinA;
        }

        for (;;)
        {
            if (distX <= distY)
            {
                cx += deltaCx;
                if (cx < 0 || cx >= NUM_CELLS)
                {
                    break;
                }

                distX += deltaDistX;
                lastStepX = 1;
            }
            else
            {
                cy += deltaCy;
                if (cy < 0 || cy >= NUM_CELLS)
                {
                    break;
                }

                distY += deltaDistY;
                lastStepX = 0;
            }

            if (g_map[cy * NUM_CELLS + cx] > 0)
            {
                // hit
                if (lastStepX)
                {
                    // W/E wall
                    dist = distX;
                }
                else
                {
                    // N/S wall
                    dist = distY;
                }
                break;
            }
        }
    }

    if (dist > 0)
    {
#if 0
        fixed_t adjustedDist = (fixed_t)(((long long)dist * g_Cosine[SUB_ANGLES(g_viewAngle, angle)]) >> 10);
        short projectedHeight = (short)(((long long)(CELL_SIZE * DIST_TO_PLANE) << 10) / adjustedDist);
#endif
        float adjustedDist = dist * g_Cosine[SUB_ANGLES(g_viewAngle, angle)];
        short projectedHeight = (short)((CELL_SIZE * DIST_TO_PLANE) / adjustedDist);
        short startY = SCREEN_HEIGHT / 2 - projectedHeight / 2;
        short endY = SCREEN_HEIGHT / 2 + projectedHeight / 2;

        if (startY < 0) startY = 0;
        if (startY > SCREEN_HEIGHT) startY = SCREEN_HEIGHT;

        if (endY < 0) endY = 0;
        if (endY > SCREEN_HEIGHT) endY = SCREEN_HEIGHT;

        for (y = 0; y < startY; ++y)
        {
            g_frame[y * SCREEN_WIDTH + x] = g_ceilingColor;
        }

        for (; y < endY; ++y)
        {
            g_frame[y * SCREEN_WIDTH + x] = 127;
        }

        for (; y < SCREEN_HEIGHT; ++y)
        {
            g_frame[y * SCREEN_WIDTH + x] = g_floorColor;
        }
    }
    else
    {
        for (y = 0; y < SCREEN_HEIGHT / 2; ++y)
        {
            g_frame[y * SCREEN_WIDTH + x] = g_ceilingColor;
        }

        for (; y < SCREEN_HEIGHT; ++y)
        {
            g_frame[y * SCREEN_WIDTH + x] = g_floorColor;
        }
    }
}

#include <math.h>
void InitializeTables()
{
    int i;
    static const float PiOver180 = 3.14156f / (float)ANG_180;

    for (i = 0; i < MAX_ANGLES; ++i)
    {
        float angle = i * PiOver180;
        //g_Sine[i] = (fixed_t)(1024.0f * sin(angle));
        //g_Cosine[i] = (fixed_t)(1024.0f * cos(angle));
        g_Sine[i] = sinf(angle);
        g_Cosine[i] = cosf(angle);
        g_Tangent[i] = tanf(angle);
    }
}
