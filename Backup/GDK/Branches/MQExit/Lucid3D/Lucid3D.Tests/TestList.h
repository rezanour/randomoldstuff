#pragma once

#include <Lucid3D.Geometry\TestList.h>

namespace Lucid3D
{
    namespace Tests
    {
        // Function Declarations
        bool MathTests();
        bool MemoryTests();

        TestFunction pfnTestList[] = 
        {
            MathTests,
            MemoryTests,
        };
    }
}
