#ifndef _VECTORMATH_H_
#define _VECTORMATH_H_

typedef struct
{
    float X;
    float Y;
} vector2_t;

// conveneince for making one
vector2_t Vector2(float x, float y);
vector2_t Vector2Zero();
vector2_t Vector2One();

// basic arithmetic
vector2_t Vector2Add(vector2_t v1, vector2_t v2);
vector2_t Vector2Subtract(vector2_t v1, vector2_t v2);
vector2_t Vector2Scale(vector2_t v, float s);

// unary operations
vector2_t Vector2Normalize(vector2_t v);
float Vector2LengthSquared(vector2_t v);
float Vector2Length(vector2_t v);

// additional operations
vector2_t Vector2Perp(vector2_t v);
float Vector2Dot(vector2_t v1, vector2_t v2);

vector2_t Vector2Rotate(vector2_t v, float ang);


#endif // _VECTORMATH_H_