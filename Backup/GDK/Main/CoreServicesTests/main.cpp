#include "StdAfx.h"
#include "TestList.h"

int wmain(_In_ int argc, _In_z_count_(argc) wchar_t** argsv)
{
    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argsv);

    size_t cPassed = 0;
    size_t cTotal = 0;

    // run tests
    for (size_t i = 0; i < _countof(pfnTestList); i++)
    {
        if (pfnTestList[i]())
        {
            cPassed++;
        }

        cTotal++;
    }

    // print summary
    wprintf(L"\n\n*** Test Summary: %d / %d passed ***\n\n", cPassed, cTotal);

    return cPassed < cTotal; // return non-0 if pass rate was not 100%
}
