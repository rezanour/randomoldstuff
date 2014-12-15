#include <math.h>
#include "common.h"

fixed_t FloatToFixed(float x)
{
    fixed_t temp;
    temp.value = (int)(x * FP_SCALE);
    return temp;
}

fixed_t IntToFixed(int x)
{
    fixed_t temp;
    temp.value = x * FP_SCALE;
    return temp;
}

float FixedToFloat(fixed_t x)
{
    return x.value / (float)FP_SCALE;
}

int WholePart(fixed_t x)
{
    return x.value / FP_SCALE;
}

int FracPart(fixed_t x)
{
    return x.value - (WholePart(x) * FP_SCALE);
}

fixed_t FixedNeg(fixed_t x)
{
    fixed_t temp;
    temp.value = -x.value ;
    return temp;
}

fixed_t FixedAbs(fixed_t x)
{
    fixed_t temp;
    temp.value = abs(x.value);
    return temp;
}

fixed_t FixedAdd(fixed_t a, fixed_t b)
{
    fixed_t temp;
    temp.value = a.value + b.value;
    return temp;
}

fixed_t FixedSubtract(fixed_t a, fixed_t b)
{
    fixed_t temp;
    temp.value = a.value - b.value;
    return temp;
}

fixed_t FixedMul(fixed_t a, fixed_t b)
{
    // to prevent overflow, we do a 2 part product and add the results
    fixed_t temp;
    temp.value = (int)(((long)a.value * (long)b.value) / FP_SCALE);
    return temp;
}

fixed_t FixedDiv(fixed_t a, fixed_t b)
{
    fixed_t temp;
    temp.value = (int)(((long)a.value * FP_SCALE) / (long)b.value);
    return temp;
}


#ifdef ENABLE_TESTS

BOOL RunFixedPointTests()
{
    fixed_t a, b, c;
    BOOL result, total = TRUE;

    a = IntToFixed(30);
    b = IntToFixed(64);

    c = FixedAdd(a, b);
    result = (WholePart(c) == 94 && FracPart(c) == 0);
    if (!result)
    {
        OutputDebugString("Fixed addition failed.\n");
    }
    total = result && total;

    c = FixedSubtract(b, a);
    result = (WholePart(c) == 34 && FracPart(c) == 0);
    if (!result)
    {
        OutputDebugString("Fixed subtraction failed.\n");
    }
    total = result && total;

    c = FixedMul(a, IntToFixed(2));
    result = (WholePart(c) == 60 && FracPart(c) == 0);
    if (!result)
    {
        OutputDebugString("Fixed multiplication failed.\n");
    }
    total = result && total;

    c = FixedMul(a, FloatToFixed(2.25f));
    result = (WholePart(c) == 67 && FracPart(c) == 500);
    if (!result)
    {
        OutputDebugString("Fixed multiplication with fraction failed.\n");
    }
    total = result && total;

    c = FixedDiv(b, FloatToFixed(3.5f));
    result = (WholePart(c) == 18 && FracPart(c) == 285);
    if (!result)
    {
        OutputDebugString("Fixed division failed.\n");
    }
    total = result && total;

    return total;
}

#endif