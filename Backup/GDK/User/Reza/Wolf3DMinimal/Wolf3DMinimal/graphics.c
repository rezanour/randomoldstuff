#include "common.h"
#include <math.h>

static fixed_t Tangent[4096];
static fixed_t InvTangent[4096];

// helpers for getting tanget and 1/tangent from tables.
// these take in angle_t
#define TAN(x)      Tangent[(x) >> 4]
#define INVTAN(x)   InvTangent[(x) >> 4]

void InitializeTables()
{
    int i;
    for (i = 0; i < ARRAYSIZE(Tangent); ++i)
    {
        //Tangent[i] = TOFIXED((float)tan(ANGLETORAD(i << 4)));
        //InvTangent[i] = Tangent[i] ? FixedDiv(TOFIXED(1), Tangent[i]) : TOFIXED(10000); // really large number for infinity case
    }
}

void DDA(point_t p, angle_t view)
{
    // horizontal intersections
    point_t a;
    //short dx;
    short dy;
    //fixed_t tang;
    //fixed_t invTang;

    view = view;
    //tang = TAN(view);
    //invTang = INVTAN(view);

    // find the first horizontal intersection
    a.y = (p.y / CELLHEIGHT) * CELLHEIGHT;
    //a.x = p.x + (short)WholePart(FixedMul(IntToFixed(p.y - a.y), invTang));

    dy = -CELLHEIGHT;
    //dx = (short)WholePart(FixedMul(IntToFixed(CELLWIDTH), invTang));

}
