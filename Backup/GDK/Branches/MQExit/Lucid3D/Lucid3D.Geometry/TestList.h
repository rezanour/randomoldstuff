#pragma once

#ifdef BUILD_TESTS

namespace Lucid3D
{
    namespace Geometry
    {
        namespace Tests
        {
            bool PrimitiveShapeTests();

            Lucid3D::Tests::TestFunction pfnTestList[] = 
            {
                PrimitiveShapeTests,
            };
        }
    }
}

#endif
