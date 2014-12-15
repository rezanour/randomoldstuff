#include "common.h"

vector2_t Vector2(float x, float y)
{
    vector2_t result = { x, y };
    return result;
}

vector2_t Vector2Zero()
{
    static vector2_t zero = { 0, 0 };
    return zero;
}

vector2_t Vector2One()
{
    static vector2_t one = { 1, 1 };
    return one;
}

vector2_t Vector2Add(vector2_t v1, vector2_t v2)
{
    return Vector2(v1.X + v2.X, v1.Y + v2.Y);
}

vector2_t Vector2Subtract(vector2_t v1, vector2_t v2)
{
    return Vector2(v1.X - v2.X, v1.Y - v2.Y);
}

vector2_t Vector2Scale(vector2_t v, float s)
{
    return Vector2(v.X * s, v.Y * s);
}

vector2_t Vector2Normalize(vector2_t v)
{
    float oneOverLength = 1.0f / Vector2Length(v);
    return Vector2(v.X * oneOverLength, v.Y * oneOverLength);
}

float Vector2LengthSquared(vector2_t v)
{
    return Vector2Dot(v, v);
}

float Vector2Length(vector2_t v)
{
    float len2 = Vector2LengthSquared(v);
    return len2 == 0.0f ? 0.0f : sqrtf(len2);
}

vector2_t Vector2Perp(vector2_t v)
{
    return Vector2(v.Y, -v.X);
}

float Vector2Dot(vector2_t v1, vector2_t v2)
{
    return v1.X * v2.X + v1.Y * v2.Y;
}

vector2_t Vector2Rotate(vector2_t v, float ang)
{
    // expanded out matrix math for 2D rotation matrix:
    //
    // |cos -sin| |x|  =  |cos * x - sin * y|
    // |sin  cos| |y|  =  |sin * x + cos * y|
    //

    float cosA = (float)cos(ang);
    float sinA = (float)sin(ang);
    vector2_t result = { (cosA * v.X - sinA * v.Y), (sinA * v.X + cosA * v.Y) };
    return result;
}
