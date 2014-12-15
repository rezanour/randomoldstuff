#include "StdAfx.h"
#include "TestList.h"

void RunTestList(_In_count_(size_t) Lucid3D::Tests::TestFunction* pfnTests, _In_ size_t cTests, _Inout_ size_t& cPassed, _Inout_ size_t& cTotal)
{
    for (size_t i = 0; i < cTests; i++)
    {
        if (pfnTests[i]())
        {
            cPassed++;
        }

        cTotal++;
    }
}

#define RUNLIST(x) RunTestList(x, _countof(x), cPassed, cTotal); 

int CALLBACK WinMain(
  __in  HINSTANCE hInstance,
  __in  HINSTANCE hPrevInstance,
  __in  LPSTR lpCmdLine,
  __in  int nCmdShow
)
{
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);

    size_t cPassed = 0;
    size_t cTotal = 0;

    // run tests
    RUNLIST(Lucid3D::Tests::pfnTestList);
    RUNLIST(Lucid3D::Geometry::Tests::pfnTestList);

    // print summary
    wchar_t summary[200] = {0};
    StringCchPrintf(summary, _countof(summary), L"\n\n*** Test Summary: %d / %d passed ***\n\n", cPassed, cTotal);
    OutputDebugString(summary);

    return cPassed < cTotal; // return non-0 if pass rate was not 100%
}
