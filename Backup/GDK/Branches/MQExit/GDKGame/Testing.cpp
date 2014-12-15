#include "StdAfx.h"

#ifdef BUILD_TESTS

#include "Testing.h"
#include <CoreServices\Debug.h>

bool RunSubTests(_In_ const char* suiteName, _In_count_(cTests) SubTest* subTests, _In_ size_t cTests)
{
    bool finalResult = true;

    DebugOut("\nRunning %s\n", suiteName);
    for (size_t i = 0; i < cTests; i++)
    {
        DebugOut("  Running sub test: %s... ", subTests[i].Name);

        bool result = subTests[i].Function();
        DebugOut("%s", result ? "Passed.\n" : "Failed.\n");

        finalResult = finalResult && result;
    }

    return finalResult;
}

#endif

