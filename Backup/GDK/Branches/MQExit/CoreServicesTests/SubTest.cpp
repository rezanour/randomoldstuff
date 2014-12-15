#include "StdAfx.h"
#include "SubTest.h"

bool RunSubTests(_In_ const char* suiteName, _In_count_(cTests) SubTest* subTests, _In_ size_t cTests)
{
    bool finalResult = true;

    wprintf(L"\nRunning %S\n", suiteName);
    for (size_t i = 0; i < cTests; i++)
    {
        wprintf(L"  Running sub test: %S... ", subTests[i].Name);

        bool result = subTests[i].Function();
        wprintf(L"%s", result ? L"Passed.\n" : L"Failed.\n");

        finalResult = finalResult && result;
    }

    return finalResult;
}


