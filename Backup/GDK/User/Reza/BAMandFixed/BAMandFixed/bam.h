#ifndef _BAM_H_
#define _BAM_H_

// for type safety
typedef struct
{
    short value;
} angle_t;

angle_t Angle(fixed_t radians);
fixed_t AngleRadians(angle_t a);

angle_t AngleAdd(angle_t a, angle_t b);
angle_t AngleSubtract(angle_t a, angle_t b);

fixed_t AngleSin(angle_t a);
fixed_t AngleCos(angle_t a);
fixed_t AngleTan(angle_t a);

angle_t AngleAsin(fixed_t f);
angle_t AngleAcos(fixed_t f);
angle_t AngleAtan(fixed_t y, fixed_t x);

#endif // _BAM_H_