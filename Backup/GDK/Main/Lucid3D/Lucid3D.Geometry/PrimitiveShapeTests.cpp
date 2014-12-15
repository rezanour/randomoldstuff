#include "StdAfx.h"
#include "Sphere.h"
#include "AABB.h"

#ifdef BUILD_TESTS

namespace Lucid3D
{
    namespace Geometry
    {
        namespace Tests
        {
            bool BasicAABBTests();
            bool BasicSphereTests();
            bool BasicIntersectionTests();

            Lucid3D::Tests::SubTest primitiveShapeSubTests[] =
            {
                DefineSubTest(BasicAABBTests),
                DefineSubTest(BasicSphereTests),
                DefineSubTest(BasicIntersectionTests),
            };

            bool BasicAABBTests()
            {
                // construction
                AABB box1;
                AABB box2(Vector3(-3, -3, -3), Vector3(3, 3, 3));
                AABB box3(box2);

                // properties
                box1.SetMin(Vector3(-5, -5, -5)).SetMax(box3.GetMax());
                box2.Set(Vector3(0, 0, 0), Vector3(4, 4, 4));

                // Use distance checks instead of equality for better control over precision
                if ((box1.GetMin() - Vector3(-5, -5, -5)).Length() > 0.001f)
                    return false;

                if ((box1.GetMax() - Vector3(3, 3, 3)).Length() > 0.001f)
                    return false;

                if ((box2.GetMin() - Vector3::Zero()).Length() > 0.001f)
                    return false;

                if ((box2.GetMax() - Vector3(4, 4, 4)).Length() > 0.001f)
                    return false;

                return true;
            }

            bool BasicSphereTests()
            {
                // construction
                Sphere s1;
                Sphere s2(Vector3(-3, -3, -3), 4);
                Sphere s3(s2);

                // properties
                s1.SetCenter(Vector3(-5, -5, -5)).SetRadius(2);
                s2.Set(Vector3(0, 0, 0), 1);

                // Use distance checks instead of equality for better control over precision
                if ((s1.GetCenter() - Vector3(-5, -5, -5)).Length() > 0.001f)
                    return false;

                if (abs(s1.GetRadius() - 2) > 0.001f)
                    return false;

                if ((s2.GetCenter() - Vector3::Zero()).Length() > 0.001f)
                    return false;

                if (abs(s2.GetRadius() - 1) > 0.001f)
                    return false;

                return true;
            }

            bool BasicIntersectionTests()
            {
                Sphere s1(Vector3(3, 0, 0), 2);
                Sphere s2(Vector3(0, 0, 0), 2);
                Sphere s3(Vector3(-6, -6, -6), 1);

                AABB box1(Vector3(-7, -7, -7), Vector3(-5, -5, -5));
                AABB box2(Vector3(-2, -2, -2), Vector3(1, 1, 1));
                AABB box3(Vector3(10, 0, 0), Vector3(11, 1, 1));

                if (!s1.Intersects(s2))
                    return false;

                if (s1.Intersects(s3))
                    return false;

                if (!s3.Intersects(box1))
                    return false;

                if (!box2.Intersects(s2))
                    return false;

                if (box3.Intersects(box1))
                    return false;

                return true;
            }

            bool PrimitiveShapeTests()
            {
                return RunSubTests(__FUNCTION__, primitiveShapeSubTests, _countof(primitiveShapeSubTests));
            }
        }
    }
}

#endif
