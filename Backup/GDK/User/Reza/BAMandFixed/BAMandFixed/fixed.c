#include "common.h"

#define FP_SCALE    16

fixed_t Fixed(float value)
{
    fixed_t result = { (int)(value * (1 << FP_SCALE)) };
    return result;
}

float FixedFloat(fixed_t a)
{
    return a.value / (float)(1 << FP_SCALE);
}

short FixedWhole(fixed_t a)
{
    short offset = a.value < 0 ? 1 : 0;
    return (a.value >> FP_SCALE) + offset;
}

float FixedFrac(fixed_t a)
{
    float f = a.value / (float)(1 << FP_SCALE);
    return f - (int)f;
}

fixed_t FixedNeg(fixed_t a)
{
    fixed_t result = { -a.value };
    return result;
}

fixed_t FixedAdd(fixed_t a, fixed_t b)
{
    fixed_t result = { a.value + b.value };
    return result;
}

fixed_t FixedSub(fixed_t a, fixed_t b)
{
    fixed_t result = { a.value - b.value };
    return result;
}

fixed_t FixedMul(fixed_t a, fixed_t b)
{
    fixed_t result = { (int)(((int64_t)a.value * (int64_t)b.value) >> FP_SCALE) };
    return result;
}

fixed_t FixedDiv(fixed_t a, fixed_t b)
{
    fixed_t result = { (int)(((int64_t)a.value << FP_SCALE) / (int64_t)b.value) };
    return result;
}
