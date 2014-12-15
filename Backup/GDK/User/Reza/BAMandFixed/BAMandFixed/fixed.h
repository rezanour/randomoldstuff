#ifndef _FIXED_H_
#define _FIXED_H_

typedef struct
{
    int value;
} fixed_t;

fixed_t Fixed(float value);
float FixedFloat(fixed_t a);
short FixedWhole(fixed_t a);
float FixedFrac(fixed_t a);

fixed_t FixedNeg(fixed_t a);
fixed_t FixedAdd(fixed_t a, fixed_t b);
fixed_t FixedSub(fixed_t a, fixed_t b);
fixed_t FixedMul(fixed_t a, fixed_t b);
fixed_t FixedDiv(fixed_t a, fixed_t b);

#endif // _FIXED_H_