#include "common.h"

#define PI_RADIANS      3.14159f
#define MAX_UINT        (unsigned int)-1

const angle_t TWOPI = { MAX_UINT };
const angle_t PI = { MAX_UINT / 2 };
const angle_t PIOVER2 = { MAX_UINT / 4 };
const angle_t PIOVER4 = { MAX_UINT / 8 };

angle_t DegToAngle(fixed_t x)
{
    angle_t temp;
    temp.value = (unsigned int)(FixedToFloat(x) * PI.value / 180.0f);
    return temp;
}

angle_t RadToAngle(fixed_t x)
{
    angle_t temp;
    temp.value = (unsigned int)(FixedToFloat(x) * PI.value / 3.14159f);
    return temp;
}

fixed_t AngleToDeg(angle_t x)
{
    return FloatToFixed(x.value * 180.0f / PI.value);
}

fixed_t AngleToRad(angle_t x)
{
    return FloatToFixed(x.value * 3.14159f / PI.value);
}


#ifdef ENABLE_TESTS

BOOL RunFixedAngleTests()
{
    angle_t a, b;
    BOOL result, total = TRUE;

    a = DegToAngle(FloatToFixed(180.0f));
    b = RadToAngle(FloatToFixed(3.14159f));

    result = (a.value == PI.value && b.value == PI.value);
    if (!result)
    {
        OutputDebugString("XToAngle failed.\n");
    }
    total = result && total;

    result = (FixedToFloat(AngleToDeg(b)) == 180.0f && FixedToFloat(AngleToRad(a)) == 3.14159f);
    if (!result)
    {
        OutputDebugString("AngleToX failed.\n");
    }
    total = result && total;

    return total;
}

#endif
