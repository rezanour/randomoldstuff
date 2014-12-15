// shared testing facilities for Lucid projects
#pragma once

#ifdef BUILD_TESTS

namespace Lucid3D
{
    namespace Tests
    {
        // All test functions use this signature
        typedef bool (*TestFunction)();

        struct SubTest
        {
            const char* const Name;
            TestFunction Function;

            SubTest(_In_ const char* const name, _In_ bool (*pfn)())
                : Name(name), Function(pfn)
            {
            }

        private:
            SubTest& operator= (const SubTest&);
        };

        bool RunSubTests(_In_ const char* suiteName, _In_count_(cTests) SubTest* subTests, _In_ size_t cTests);
    }
}

#define DefineSubTest(function) Lucid3D::Tests::SubTest(#function, function)

#include "Lucid3D.Testing.inl"

#endif

