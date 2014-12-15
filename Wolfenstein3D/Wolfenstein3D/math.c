#include "common.h"

VEC2 Vec2Add(VEC2 vec, VEC2 vec2)
{
    VEC2 result = { vec.x + vec2.x, vec.y + vec2.y };
    return result;
}

VEC2 Vec2Scale(VEC2 vec, float scale)
{
    VEC2 result = { vec.x * scale, vec.y * scale };
    return result;
}

VEC2 Vec2Neg(VEC2 vec)
{
    VEC2 result = { -vec.x, -vec.y };
    return result;
}

VEC2 Vec2Normalize(VEC2 vec)
{
    float scale = 1.0f / Vec2Len(vec);
    return Vec2Scale(vec, scale);
}

VEC2 Vec2Perp(VEC2 vec)
{
    VEC2 result = { -vec.y, vec.x };
    return result;
}

float Vec2Len(VEC2 vec)
{
    return sqrt(Vec2LenSquared(vec));
}

float Vec2LenSquared(VEC2 vec)
{
    return vec.x * vec.x + vec.y * vec.y;
}

VEC2 Vec2Zero()
{
    static VEC2 zero = { 0, 0 };
    return zero;
}

VEC2 Vec2Up()
{
    static VEC2 up = { 0, -1.0f };
    return up;
}

VEC2 Vec2Left()
{
    static VEC2 left = { -1.0f, 0 };
    return left;
}

VEC2 Vec2Right()
{
    static VEC2 right = { 1.0f, 0.0f };
    return right;
}

VEC2 Vec2Down()
{
    static VEC2 down = { 0, 1.0f };
    return down;
}

float Vec2Dot(VEC2 vec, VEC2 vec2)
{
    return vec.x * vec2.x + vec.y * vec2.y;
}
