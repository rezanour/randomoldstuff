#ifndef _FIXEDPOINT_H_
#define _FIXEDPOINT_H_

//
// Fixed Point numbers
//
// To avoid floating point, we store real numbers as fixed point using a radix base of 2^16. High bit is sign (1 = negative)
//
typedef struct
{
    int value;
} fixed_t;

#define FP_SCALE    10000

// creating a fixed point number
extern fixed_t FloatToFixed(float x);
extern fixed_t IntToFixed(int x);
extern float FixedToFloat(fixed_t x);

// extracting parts of a fixed point
extern int     WholePart(fixed_t x);
extern int     FracPart(fixed_t x);

// basic unary operations
extern fixed_t FixedNeg(fixed_t x);
extern fixed_t FixedAbs(fixed_t x);

// basic binary operations
extern fixed_t FixedAdd(fixed_t a, fixed_t b);
extern fixed_t FixedSubtract(fixed_t a, fixed_t b);
extern fixed_t FixedMul(fixed_t a, fixed_t b);
extern fixed_t FixedDiv(fixed_t a, fixed_t b);


#ifdef ENABLE_TESTS
extern BOOL RunFixedPointTests();
#endif

#endif // _FIXEDPOINT_H_