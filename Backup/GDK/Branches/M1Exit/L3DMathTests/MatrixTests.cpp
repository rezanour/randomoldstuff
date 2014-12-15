#define DIRECTXMATH_INTEROP
#include <L3DMath\Matrix.h>
#include "SubTest.h"

using namespace L3DMath;
using namespace DirectX;

namespace
{
    bool BasicTest();

    SubTest subTests[] =
    {
        DefineSubTest(BasicTest),
    };

    bool BasicTest()
    {
        // This doesn't really test anything yet, just shows various usage patterns (and ensures they all compile)

        Matrix m;

        m.SetRow(3, Vector3::Forward(), 0.0f);

        Matrix m2(m);

        Matrix m3(Matrix::Transpose(m2));

        m2 = m2 * m3;
        Vector3 v(3, 5, 0);

        Vector3 transformed(v * m3);

        v.Transform(m);

        m3.SetRow(3, v, 1.0f);

        return true;
    }

}

bool MatrixTests()
{
    return RunSubTests(__FUNCTION__, subTests, _countof(subTests));
}


