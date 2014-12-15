#ifndef _W3DMATH_H_
#define _W3DMATH_H_

typedef struct
{
    float x;
    float y;
} VEC2, *PVEC2;

VEC2 Vec2Add(VEC2 vec, VEC2 vec2);
VEC2 Vec2Scale(VEC2 vec, float scale);
VEC2 Vec2Neg(VEC2 vec);
float Vec2Len(VEC2 vec);
float Vec2LenSquared(VEC2 vec);
VEC2 Vec2Normalize(VEC2 vec);
VEC2 Vec2Perp(VEC2 vec);
float Vec2Dot(VEC2 vec, VEC2 vec2);

/* common directions */
VEC2 Vec2Zero();
VEC2 Vec2Up();
VEC2 Vec2Left();
VEC2 Vec2Right();
VEC2 Vec2Down();

#endif /* _W3DMATH_H_ */