#include "tinyw3d.h"
#include <math.h> /* not used in core algorithms, just to set up tables and for reference impl */

#define REFERENCE
//#define EIGHT_BIT
//#define SIXTEEN_BIT

static byte g_DebugEnable = 0;
void ToggleDebug()
{
    g_DebugEnable = (g_DebugEnable ? 0 : 1);
}

#ifdef REFERENCE

static unsigned short g_angle;
static short g_posx;
static short g_posy;

static const short g_cellSize = 32; // each cell is 32x32 units to match the 32x32 textures
static const short g_maxCells = 32; // 32x32 level size
static short g_cellSize2; /* cell size squared */
static unsigned short g_unitsPerColumn;

static unsigned short g_halfViewAngle;
static const unsigned short g_90Deg = 16384;
static const unsigned short g_180Deg = 32768;
static const unsigned short g_270Deg = 49151;

static const byte g_Level[32 * 32] = 
{
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
};

static byte g_DebugLevelCollision[32 * 32];

/* Ray cast into the level and draw the visible part */
static void RaycastView();
static void RaycastColumn(short column, short x, short y, unsigned short angle);
static void DebugDraw();

void InitializeGame()
{
    g_halfViewAngle = (unsigned short)(30.0f * (65536 / 360.0f));
    g_unitsPerColumn = (unsigned short)(60.0f * (65536 / 360.0f) / 160);
    g_posx = g_posy = g_cellSize * g_maxCells / 2;
    g_angle = g_90Deg;
    g_cellSize2 = g_cellSize * g_cellSize;
}

void TickGame()
{
    int i;

    for (i = 0; i < g_maxCells * g_maxCells; ++i)
    {
        g_DebugLevelCollision[i] = 0;
    }

    for (i = 0; i < 160 * 100; ++i)
    {
        W3DSetPixel(i % 160, i / 160, 0);
    }

    RaycastView();

    if (g_DebugEnable)
        DebugDraw();
}

void RaycastView()
{
    /* divide up the screen and view angle into slices (columns) and raycast each one */

    unsigned short angle = g_angle - g_halfViewAngle;
    short i = 0;

    for (; i < 160; ++i)
    {
        RaycastColumn(i, g_posx, g_posy, angle);
        angle += g_unitsPerColumn;
    }
}

void RaycastColumn(short column, short x, short y, unsigned short angle)
{
    byte cx, cy; /* current cell */
    sbyte dcx, dcy; /* cell step direction */
    short xDistToCell, yDistToCell;
    float distA, distB, deltaA, deltaB;
    float tanA = tanf(angle * (3.14156f * 2 / 65536.0f));
    float cosA = cosf((g_angle - angle) * (3.14156f * 2 / 65536.0f));
    byte horiz;

    cx = x / g_cellSize;
    cy = y / g_cellSize;

    dcx = (angle <= g_90Deg || angle >= g_270Deg) ? 1 : -1;
    dcy = (angle <= g_180Deg) ? 1 : -1;

    /* dist to first A, B along their primary axis */
    xDistToCell = (dcx > 0) ? 
        (cx + 1) * g_cellSize - x :
            x - (g_cellSize * cx);

    yDistToCell = (dcy > 0) ?
        (cy + 1) * g_cellSize - y :
            y - (g_cellSize * cy);

    /* distance deltas per step */
    deltaA = sqrtf(1 + tanA*tanA);
    deltaB = sqrtf(1 + (1.0f / (tanA*tanA)));

    /* initial values */
    distA = xDistToCell * deltaA;
    distB = yDistToCell * deltaB;

    /* distance deltas per step */
    deltaA = g_cellSize * deltaA;
    deltaB = g_cellSize * deltaB;

    /* DDA */
    for (;;)
    {
        if (distA <= distB)
        {
            cx += dcx;
            if (cx >= g_maxCells)
            {
                break;
            }
            distA += deltaA;
            horiz = 1;
        }
        else
        {
            cy += dcy;
            if (cy >= g_maxCells)
            {
                break;
            }
            distB += deltaB;
            horiz = 0;
        }

        /* light up the path we traverse for debug */
        g_DebugLevelCollision[cy * g_maxCells + cx] = 3;

        /* Test cell (cx, cy) */
        if (g_Level[cy * g_maxCells + cx] != 0)
        {
            float dist = (horiz == 1 ? distA : distB);
            float normalizedDist = cosA * dist;
            float distToPlane = 80.0f /  tanf(30.0f/360.0f * (2 * 3.14156f)); /* constant, move later */
            float halfHeight = distToPlane / normalizedDist * 32;
            int y;

            for (y = 50 - (byte)halfHeight; y < 50 + (byte)halfHeight; ++y)
            {
                if (y < 0 || y >= 100)
                {
                    continue;
                }
                W3DSetPixel(column, y, 1);
            }

            g_DebugLevelCollision[cy * g_maxCells + cx] = 4;
            break;
        }
    }
}

void DebugDraw()
{
    short x, y;
    short mx, my;
    byte color;

    /* level */
    for (y = 0; y < 100; y++)
    {
        my = y * g_maxCells / 100;
        for (x = 0; x < 160; x++)
        {
            mx = x * g_maxCells / 160;

            color = g_DebugLevelCollision[my * g_maxCells + mx];
            if (color == 0)
            {
                color = g_Level[my * g_maxCells + mx];
            }

            W3DSetPixel(x, y, color);
        }
    }

    /* player */
    W3DSetPixel(g_posx * 160 / (g_maxCells * g_cellSize), g_posy * 100 / (g_maxCells * g_cellSize), 2);
}

void MovePlayer(short dist)
{
    float cosA = cosf(g_angle * (2 * 3.14156f) / 65536.0f);
    float sinA = sinf(g_angle * (2 * 3.14156f) / 65536.0f);
    g_posx += cosA * dist;
    g_posy += sinA * dist;

    if (g_posx < 0) g_posx = 0;
    if (g_posx > g_maxCells * g_cellSize) g_posx = g_maxCells * g_cellSize;

    if (g_posy < 0) g_posy = 0;
    if (g_posy > g_maxCells * g_cellSize) g_posy = g_maxCells * g_cellSize;
}

void TurnPlayer(unsigned short angle)
{
    g_angle += angle;
}


#elif defined(EIGHT_BIT)

static byte g_angle;
static byte g_posx;
static byte g_posy;

#define CELLSIZE 16
#define MAPSIZE 16

#define g_90Deg 64
#define g_180Deg 128
#define g_270Deg 192

static const byte g_Level[MAPSIZE * MAPSIZE] = 
{
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1,
    1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
};

static byte g_DebugLevelCollision[MAPSIZE * MAPSIZE];

#define TABLESIZE 256
static byte g_DistA[TABLESIZE];
static byte g_DistB[TABLESIZE];
static sbyte g_Cos[TABLESIZE];
static sbyte g_Sin[TABLESIZE];

static void RaycastView();
static void RaycastColumn(byte column, byte x, byte y, byte angle);
static void DebugDraw();

void InitializeGame()
{
    /* Initialize the table */

    /* HACK HACK: We use floating point here to compute the table at runtime. On the XGameStation, we're going
     to just copy these values and hard code them into a static array, so no floating point needed at runtime */

    const float PI = 3.14156f;
    int i;

    for (i = 0; i < TABLESIZE; i++)
    {
        float angle = i * 2 * PI / 256.0f;
        float tanA = tanf(angle);
        float term = sqrtf(1 + tanA*tanA);

        if (i == g_90Deg || i == g_270Deg)
        {
            g_DistA[i] = g_DistB[i] = 0;
        }
        else
        {
            g_DistA[i] = (byte)(term * 4);
            term = sqrtf(1 + (1.0f / (tanA*tanA)));
            g_DistB[i] = (byte)(term * 4);
        }

        g_Cos[i] = (sbyte)(cosf(angle) * 128);
        if (i == 0)
        {
            g_Cos[i] = 127;
        }
        g_Sin[i] = (sbyte)(sinf(angle) * 128);
    }

    g_posx = g_posy = CELLSIZE * MAPSIZE / 2;
    g_angle = 0;
}

void TickGame()
{
    int i;

    for (i = 0; i < MAPSIZE * MAPSIZE; ++i)
    {
        g_DebugLevelCollision[i] = 0;
    }

    for (i = 0; i < 160 * 100; ++i)
    {
        W3DSetPixel(i % 160, i / 160, 0);
    }

    RaycastView();

    if (g_DebugEnable)
        DebugDraw();
}

void RaycastView()
{
    /* divide up the screen and view angle into slices (columns) and raycast each one */

    byte angle = g_angle - 20;
    byte i = 0;

    for (; i < 40; ++i)
    {
        RaycastColumn(i * 4, g_posx, g_posy, angle);
        RaycastColumn(i * 4 + 1, g_posx, g_posy, angle);
        RaycastColumn(i * 4 + 2, g_posx, g_posy, angle);
        RaycastColumn(i * 4 + 3, g_posx, g_posy, angle);
        angle++;
    }
}

void RaycastColumn(byte column, byte x, byte y, byte angle)
{
    byte cx, cy; /* current cell */
    sbyte dcx, dcy; /* cell step direction */
    byte xDistToCell, yDistToCell;
    unsigned short distA, distB, deltaA, deltaB;
    byte horiz;

    cx = x / CELLSIZE;
    cy = y / CELLSIZE;

    dcx = (angle <= g_90Deg || angle >= g_270Deg) ? 1 : -1;
    dcy = (angle < g_180Deg) ? 1 : -1;

    /* dist to first A, B along their primary axis */
    xDistToCell = (dcx > 0) ? 
        (cx + 1) * CELLSIZE - x :
            x - (CELLSIZE * cx);

    yDistToCell = (dcy > 0) ?
        (cy + 1) * CELLSIZE - y :
            y - (CELLSIZE * cy);

    /* Handle special case with invalid dist functions*/
    if (angle == 0 || angle == g_180Deg)
    {
        distA = xDistToCell;

        for (;;)
        {
            cx += dcx;
            if (cx >= MAPSIZE)
            {
                break;
            }
            distA += CELLSIZE * 4;

            /* light up the path we traverse for debug */
            g_DebugLevelCollision[cy * MAPSIZE + cx] = 3;

            /* Test cell (cx, cy) */
            if (g_Level[cy * MAPSIZE + cx] != 0)
            {
                short normalizedDist = g_Cos[g_angle - angle] * distA;
                short distToPlane = (short)(128 * 80.0f /  tanf(30.0f/360.0f * (2 * 3.14156f))); /* constant, move later */
                short halfHeight = (short)(distToPlane * 2 / normalizedDist); // 8 is half wall height, assuming walls of 16x16. The 4 is to cancel our scales
                short y;

                for (y = 50 - halfHeight; y < 50 + halfHeight; ++y)
                {
                    if (y < 0 || y >= 100)
                    {
                        continue;
                    }
                    W3DSetPixel(column, y, 1);
                }
                g_DebugLevelCollision[cy * MAPSIZE + cx] = 4;
                break;
            }
        }
        return;
    }
    else if (angle == g_90Deg || angle == g_270Deg)
    {
        distB = yDistToCell;
        for (;;)
        {
            cy += dcy;
            if (cy >= MAPSIZE)
            {
                break;
            }

            distB += CELLSIZE * 4;

            /* light up the path we traverse for debug */
            g_DebugLevelCollision[cy * MAPSIZE + cx] = 3;

            /* Test cell (cx, cy) */
            if (g_Level[cy * MAPSIZE + cx] != 0)
            {
                short normalizedDist = g_Cos[g_angle - angle] * distB;
                short distToPlane = (short)(128 * 80.0f /  tanf(30.0f/360.0f * (2 * 3.14156f))); /* constant, move later */
                short halfHeight = (short)(distToPlane * 2 / normalizedDist); // 8 is half wall height, assuming walls of 16x16. The 4 is to cancel our scales
                short y;

                for (y = 50 - halfHeight; y < 50 + halfHeight; ++y)
                {
                    if (y < 0 || y >= 100)
                    {
                        continue;
                    }
                    W3DSetPixel(column, y, 1);
                }
                g_DebugLevelCollision[cy * MAPSIZE + cx] = 4;
                break;
            }
        }
        return;
    }

    /* distance deltas per step */
    deltaA = g_DistA[angle];
    deltaB = g_DistB[angle];

    /* initial values */
    distA = xDistToCell * deltaA;
    distB = yDistToCell * deltaB;

    deltaA = CELLSIZE * deltaA;
    deltaB = CELLSIZE * deltaB;

    /* DDA */
    for (;;)
    {
        if (distA <= distB)
        {
            cx += dcx;
            if (cx >= MAPSIZE)
            {
                break;
            }
            distA += deltaA;
            horiz = 1;
        }
        else
        {
            cy += dcy;
            if (cy >= MAPSIZE)
            {
                break;
            }
            distB += deltaB;
            horiz = 0;
        }

        /* light up the path we traverse for debug */
        g_DebugLevelCollision[cy * MAPSIZE + cx] = 3;

        /* Test cell (cx, cy) */
        if (g_Level[cy * MAPSIZE + cx] != 0)
        {
            short normalizedDist = g_Cos[g_angle - angle] * (horiz == 1 ? distA : distB);
            short distToPlane = (short)(128 * 80.0f /  tanf(30.0f/360.0f * (2 * 3.14156f))); /* constant, move later */
            short halfHeight = (short)(distToPlane * 2 / normalizedDist); // 8 is half wall height, assuming walls of 16x16. The 4 is to cancel our scales
            short y;

            for (y = 50 - halfHeight; y < 50 + halfHeight; ++y)
            {
                if (y < 0 || y >= 100)
                {
                    continue;
                }
                W3DSetPixel(column, y, 1);
            }

            g_DebugLevelCollision[cy * MAPSIZE + cx] = 4;
            break;
        }
    }
}

void DebugDraw()
{
    short x, y;
    short mx, my;
    byte color;

    /* level */
    for (y = 0; y < 100; y++)
    {
        my = y * MAPSIZE / 100;
        for (x = 0; x < 160; x++)
        {
            mx = x * MAPSIZE / 160;

            color = g_DebugLevelCollision[my * MAPSIZE + mx];
            if (color == 0)
            {
                color = g_Level[my * MAPSIZE + mx];
            }

            W3DSetPixel(x, y, color);
        }
    }

    /* player */
    W3DSetPixel(g_posx * 160 / (MAPSIZE * CELLSIZE), g_posy * 100 / (MAPSIZE * CELLSIZE), 2);
}

void MovePlayer(short dist)
{
    g_posx += dist * g_Cos[g_angle] / 128;
    g_posy += dist * g_Sin[g_angle] / 128;

    if (g_posx > MAPSIZE * CELLSIZE) g_posx = (byte)(MAPSIZE * CELLSIZE);
    if (g_posy > MAPSIZE * CELLSIZE) g_posy = (byte)(MAPSIZE * CELLSIZE);
}

void TurnPlayer(unsigned short angle)
{
    g_angle += (byte)((unsigned short)angle * 256 / 65536);
}

#else

static unsigned short g_angle;
static short g_posx;
static short g_posy;

static unsigned short g_unitsPerDegree = (unsigned short)(65536 / 360.0f);
static unsigned short g_unitsPerRad = (unsigned short)(65536 / (2 * 3.14156f));
static unsigned short g_unitsPerColumn; /* fov / screen width * unitsPerDegree */

static const short g_cellSize = 32; // each cell is 32x32 units to match the 32x32 textures
static const short g_maxCells = 32; // 32x32 level size

static unsigned short g_halfViewAngle;
static const unsigned short g_90Deg = 16384;
static const unsigned short g_180Deg = 32768;
static const unsigned short g_270Deg = 49151;

static const byte g_Level[32 * 32] = 
{
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
};

static byte g_DebugLevelCollision[32 * 32];

/* Ray cast into the level and draw the visible part */
static void RaycastView();
static void RaycastColumn(short column, short x, short y, unsigned short angle);
static void DebugDraw();

#define TABLESIZE 512
#define SHIFT 7
static unsigned short g_DistA[TABLESIZE];
static unsigned short g_DistB[TABLESIZE];

void InitializeGame()
{
    /* Initialize the table */

    unsigned int i;
    float angle, tanA;
    float PiRads = 3.14156f;
    float term;

    for (i = 0; i < TABLESIZE; i++)
    {
        angle = (i << SHIFT) * (2 * PiRads) / 65536.0f;
        tanA = tanf(angle);
        term = sqrtf(1 + tanA * tanA);
        g_DistA[i] = (unsigned short)(term * 32);
        term = sqrtf(1 + (1.0f / (tanA * tanA)));
        g_DistB[i] = (unsigned short)(term * 32);
    }

    g_unitsPerColumn = (unsigned short)(60.0f / 160.0f * g_unitsPerDegree);
    g_halfViewAngle = (unsigned short)(30.0f * g_unitsPerDegree);
    g_posx = g_posy = g_cellSize * g_maxCells / 2;
    g_angle = g_90Deg;
}

void TickGame()
{
    int i;

    for (i = 0; i < g_maxCells * g_maxCells; ++i)
    {
        g_DebugLevelCollision[i] = 0;
    }

    RaycastView();

    if (g_DebugEnable)
        DebugDraw();
}

void RaycastView()
{
    /* divide up the screen and view angle into slices (columns) and raycast each one */

    unsigned short angle = g_angle - g_halfViewAngle;
    short i = 0;

    for (; i < 160; ++i)
    {
        RaycastColumn(i, g_posx, g_posy, angle);
        angle += g_unitsPerColumn;
    }
}

void RaycastColumn(short column, short x, short y, unsigned short angle)
{
    /*
        This is a highly custom, Reza-optimized DDA. Requires pencil and paper and a lot of 
        diagrams to fully explain, but I've reduced all the parameters and steps down to a very 
        tight incremental process. Here's a quick overview:

        1. Starting from any point within any cell (x, y), we draw a line along your viewing angle.

        2. This line intersects cells along various edges as it passes through the grid
          We call the first intersection with a left/right edge of a cell point A
          We call the first intersection with a top/bottom edge of a cell point B

        3. We take the closer of A or B, and determine collision with the cell that we enter by passing through
          that point. Cells can be entered from one of the 4 cardinal directions, and it's obvious which one from 
          the point that we intersect and the direction we're moving along the ray.

        4. We then move the point we used (either A or B) out along the ray to the next intersection for
          that class of point. If it's A we're moving (recall A represents left/right collisions), then we 
          move it to the next point that the ray intersects a left/right wall. If it's B, we move it to the next
          top/bottom collision.

        5. Repeat steps 3 & 4 until we find a hit or either A or B leave the grid.

        Now, the magic is in how optimized we make this. Notice we don't actually use the specific
        values of A and B. All we really care about is the distance of each to the start, so we can pick 
        which one to do next. But we don't really even care about the exact distance, square distances would do too
        since it's a comparison. Also, we know we always move A in the X direction by 1 cell width, and B along the 
        Y direction by one cell height, and then adjust their other dimension to match the slope. We can 
        compute a value distA and distB for each point that represents the distance increase per step. We
        just add this to an accumulated distance for each and compare again, avoiding actual trig :)

        Here's some of the math for reference. Note that this math just shows how we computed the optimal values
        below, it's not actually executed in code.

        A) for a given angle, the slope is tanA = deltaY / deltaX

        B) The distance of a point from the start = sqrt(dx*dx + dy*dy), but we don't actually need sqrt so 
            reduces to dx*dx + dy*dy
          B.1) Since A always increases in x by cellSize, it's dx = cellSize. dy can be computed from slope.
            Putting it all together: deltaA = cellSize*cellSize + (cellSize * tan)
          B.2) Since B always increases in y by cellSize, it's dy = cellSize. dx can be computed from slope.
            Putting it all together: deltaB = cellSize*cellSize + (cellSize / tan)

        C) We need an initial starting point for the dists, which is dist to initial A and B.
          C.1) initial dx of A is dist to next cell left/top instead of cellSize. Similar for B.
          Adjusting, we get:
             distA = cellSize*cellSize + (xdistToCell * tan)
             distB = cellSize*cellSize + (ydistToCell / tan)

        D) if our ray is moving to the right, then each time we pick A over B, we move 1 cell to the right.
            if our ray is moving to the left, then move once cell to the left each time A is picked over B.
            Same for B, except moving up or down a cell. This is how we know which cell to test. 
            Just keep track of what cell we've traced into and each step moves either x or y by 1/-1

        E) tan requires a real angle, but we use a BAM. Also, tan is a rational number, but we don't use
            floats. What to do!? Oh, wait, I know! We can precompute scaled tangents and store them in a
            lookup table! The format of each stored entry is this:
                Table[angle >> 8] = cellSize * tan(TO_RADS(angle))

            So, this means the formulas for the distances reduce to:

            deltaA = cellSize*cellSize + tableValue;
            deltaB = cellSize*cellSize + (cellSize*cellSize)/tableValue;

            For initial values of A and B, we adjust accordingly

            initialDistToA = cellSize*cellSize + (xDistToCell * tableValue / cellSize);
            initialDistToB = cellSize*cellSize + (yDistToCell * cellSize / tableValue);

        Now we're talking! Enjoy the awesomely optimized RezaDDA below :)
    */

    byte cx, cy; /* current cell */
    sbyte dcx, dcy; /* cell step direction */
    byte xDistToCell, yDistToCell;
    unsigned short distA, distB, deltaA, deltaB;

    cx = x / g_cellSize;
    cy = y / g_cellSize;

    dcx = (angle <= g_90Deg || angle >= g_270Deg) ? 1 : -1;
    dcy = (angle < g_180Deg) ? 1 : -1;

    /* dist to first A, B along their primary axis */
    xDistToCell = (dcx > 0) ? 
        (cx + 1) * g_cellSize - x :
            x - (g_cellSize * cx);

    yDistToCell = (dcy > 0) ?
        (cy + 1) * g_cellSize - y :
            y - (g_cellSize * cy);

    /* distance deltas per step */
    deltaA = g_DistA[angle >> SHIFT];
    deltaB = g_DistB[angle >> SHIFT];

    /* initial values */
    distA = xDistToCell * deltaA / 128;
    distB = yDistToCell * deltaB / 128;

    /* distance deltas per step */
    deltaA = g_cellSize * deltaA / 128;
    deltaB = g_cellSize * deltaB / 128;

    /* DDA */
    for (;;)
    {
        if (distA <= distB)
        {
            cx += dcx;
            if (cx >= g_maxCells)
            {
                break;
            }
            distA += deltaA;
        }
        else
        {
            cy += dcy;
            if (cy >= g_maxCells)
            {
                break;
            }
            distB += deltaB;
        }

        /* light up the path we traverse for debug */
        g_DebugLevelCollision[cy * g_maxCells + cx] = 3;

        /* Test cell (cx, cy) */
        if (g_Level[cy * g_maxCells + cx] != 0)
        {
            g_DebugLevelCollision[cy * g_maxCells + cx] = 4;
            break;
        }
    }
}

void DebugDraw()
{
    short x, y;
    short mx, my;
    byte color;

    /* level */
    for (y = 0; y < 100; y++)
    {
        my = y * g_maxCells / 100;
        for (x = 0; x < 160; x++)
        {
            mx = x * g_maxCells / 160;

            color = g_DebugLevelCollision[my * g_maxCells + mx];
            if (color == 0)
            {
                color = g_Level[my * g_maxCells + mx];
            }

            W3DSetPixel(x, y, color);
        }
    }

    /* player */
    W3DSetPixel(g_posx * 160 / (g_maxCells * g_cellSize), g_posy * 100 / (g_maxCells * g_cellSize), 2);
}

void MovePlayer(short dx, short dy)
{
    g_posx += dx;
    g_posy += dy;

    if (g_posx < 0) g_posx = 0;
    if (g_posx > g_maxCells * g_cellSize) g_posx = g_maxCells * g_cellSize;

    if (g_posy < 0) g_posy = 0;
    if (g_posy > g_maxCells * g_cellSize) g_posy = g_maxCells * g_cellSize;
}

void TurnPlayer(unsigned short angle)
{
    g_angle += angle;
}

#endif
