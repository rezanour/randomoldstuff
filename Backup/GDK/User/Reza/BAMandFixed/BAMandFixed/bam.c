#include "common.h"

angle_t Angle(fixed_t radians)
{
    fixed_t pi = Fixed(PI_RADIANS);
    fixed_t negPi = FixedNeg(pi);
    fixed_t twoPi = FixedMul(pi, Fixed(2));

    angle_t result;

    // ensure within -PI and PI
    while (radians.value < negPi.value)
    {
        radians = FixedAdd(radians, twoPi);
    }

    while (radians.value > pi.value)
    {
        radians = FixedSub(radians, twoPi);
    }

    result.value = FixedWhole(FixedMul(FixedDiv(radians, pi), Fixed(MAX_SHORT)));
    return result;
}

fixed_t AngleRadians(angle_t a);

angle_t AngleAdd(angle_t a, angle_t b);
angle_t AngleSubtract(angle_t a, angle_t b);

fixed_t AngleSin(angle_t a);
fixed_t AngleCos(angle_t a);
fixed_t AngleTan(angle_t a);

angle_t AngleAsin(fixed_t f);
angle_t AngleAcos(fixed_t f);
angle_t AngleAtan(fixed_t y, fixed_t x);

