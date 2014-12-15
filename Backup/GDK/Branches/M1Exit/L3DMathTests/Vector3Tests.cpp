#define DIRECTXMATH_INTEROP
#include <L3DMath\Vector2.h>
#include <L3DMath\Vector3.h>
#include "SubTest.h"

using namespace L3DMath;
using namespace DirectX;

namespace
{
    bool BasicTest();
    bool WhyMacsSuck();

    SubTest subTests[] =
    {
        DefineSubTest(BasicTest),
        DefineSubTest(WhyMacsSuck),
    };

    bool WhyMacsSuck()
    {
        Vector2 a(11.283f, 14.653f);
        Vector2 b(6.8f, -4.4f);

        float l = a.Length();
        Vector2 aN = Vector2::Normalize(a);

        Vector2 ab = b - a;
        Vector2 perp = Vector2(-ab.Y, ab.X);

        ab.Normalize();
        perp.Normalize();

        Matrix m;
        return true;
    }

    bool BasicTest()
    {
        // This doesn't really test anything yet, just shows various usage patterns (and ensures they all compile)

        Vector3 v;
        v.X = 3;
        v.Y = 4;
        v.Z = 5;
        v.SetX(1).SetY(2).SetZ(3);

        XMFLOAT3 xmf(32, 343, 393);
        XMVECTOR xmv = v;

        Vector3 vf(xmf);

        Vector3 a = xmf;
        Vector3 b = xmv;

        xmf = a;

        Vector3 v1(v);
        v1.Set(4, 5, 6);

        Vector3 v2 = v.Cross(v1.SetY(144));

        float d = v2.Dot(v1);
        v2 = Vector3::Cross(v, v1);

        float f = Vector3::Dot(v2, v);

        if (v1.Normalize().Length() != 1)
            return false;

        Vector3 v3 = Vector3::Forward();
        Vector3 v4 = v1 + v2;
        Vector3 v5 = v4 - v;
        v3 += v2;
        v3 -= v;

        v4 = v5 * f;
        v3 = d * v4;
        v2 *= f;
        v *= d;

        return true;
    }

}

bool Vector3Tests()
{
    return RunSubTests(__FUNCTION__, subTests, _countof(subTests));
}


