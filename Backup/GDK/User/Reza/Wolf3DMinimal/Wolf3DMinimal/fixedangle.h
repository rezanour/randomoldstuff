#ifndef _FIXEDANGLE_H_
#define _FIXEDANGLE_H_

//
// Fixed Angles
//
// These are kind of like BAM (Binary angle measurement), but simpler and just use
// plain integer underflow/overflow to manage wrap around
//
typedef struct
{
    unsigned int value;
} angle_t;

extern const angle_t TWOPI;
extern const angle_t PI;
extern const angle_t PIOVER2;
extern const angle_t PIOVER4;

extern angle_t DegToAngle(fixed_t x);
extern angle_t RadToAngle(fixed_t x);
extern fixed_t AngleToDeg(angle_t x);
extern fixed_t AngleToRad(angle_t x);


#ifdef ENABLE_TESTS
extern BOOL RunFixedAngleTests();
#endif

//// angles are represented as unsigned shorts with the max ushort == 2PI
//// we take advantage of overflow and underflow to ensure they wrap properly
//typedef unsigned short angle_t;
//
//#define TWOPI           ((angle_t)-1)
//#define PI              TWOPI / 2
//#define PIOVER2         PI / 2
//#define PIOVER4         PI / 4
//
//
//#define DEGTORAD(x)     (float)((x) * PI_RADIANS / 180.0f)
//#define RADTODEG(x)     (float)((x) * 180.0f / PI_RADIANS)
//#define RADTOANGLE(x)   (angle_t)((x) * PI / PI_RADIANS)
//#define DEGTOANGLE(x)   RADTOANGLE(DEGTORAD(x))
//#define ANGLETORAD(x)   (float)((x) * PI_RADIANS / PI)
//#define ANGLETODEG(x)   RADTODEG(ANGLETORAD(x))
//


#endif // _FIXEDANGLE_H_