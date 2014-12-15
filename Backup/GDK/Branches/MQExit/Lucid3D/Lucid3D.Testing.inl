#pragma once

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <cstdlib>
#include <strsafe.h>

namespace Lucid3D
{
    namespace Tests
    {
        L3DINLINE bool RunSubTests(_In_ const char* suiteName, _In_count_(cTests) SubTest* subTests, _In_ size_t cTests)
        {
            bool finalResult = true;
            wchar_t message[1000] = {0};

            if (FAILED(StringCchPrintf(message, _countof(message), L"\nRunning %S\n", suiteName)))
            {
                OutputDebugString(L"BLOCKED: StringCchPrintf failed!\n");
                return false;
            }
            OutputDebugString(message);

            for (size_t i = 0; i < cTests; i++)
            {
                if (FAILED(StringCchPrintf(message, _countof(message), L"  Running sub test: %S... ", subTests[i].Name)))
                {
                    OutputDebugString(L"BLOCKED: StringCchPrintf failed!\n");
                    return false;
                }
                OutputDebugString(message);

                bool result = subTests[i].Function();
                if (FAILED(StringCchPrintf(message, _countof(message), L"%s", result ? L"Passed.\n" : L"Failed.\n")))
                {
                    OutputDebugString(L"BLOCKED: StringCchPrintf failed!\n");
                    return false;
                }
                OutputDebugString(message);

                finalResult = finalResult && result;
            }

            return finalResult;
        }
    }
}
