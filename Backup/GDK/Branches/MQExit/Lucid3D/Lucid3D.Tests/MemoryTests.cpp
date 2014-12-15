#include "StdAfx.h"

#include <Lucid3D.Memory.h>

using namespace Lucid3D;

namespace Lucid3D
{
    namespace Tests
    {
        bool SimpleArenaTests();

        SubTest memorySubTests[] =
        {
            DefineSubTest(SimpleArenaTests),
        };

        bool SimpleArenaTests()
        {
            SimpleArena arena(1000000); // ~1MB arena

            int* pInts = static_cast<int*>(arena.Allocate(100, "a 100 byte alloc"));
            float* pTwoVectors = static_cast<float*>(arena.AllocateAligned(8 * sizeof(float), 16, "two aligned vectors"));

            for (size_t i = 0; i < 8; ++i)
            {
                pTwoVectors[i] = static_cast<float>(i);
            }

            // this should stomp over the floats
            for (int i = 0; i < 100; ++i)
            {
                pInts[i] = i;
            }

            if (pTwoVectors[0] == 0.0f)
                return false;

            return true;
        }

        bool MemoryTests()
        {
            return RunSubTests(__FUNCTION__, memorySubTests, _countof(memorySubTests));
        }
    }
}
