#pragma once

struct SubTest
{
    const char* const Name;
    bool (*Function)();

    SubTest(_In_ const char* const name, _In_ bool (*pfn)())
        : Name(name), Function(pfn)
    {
    }

private:
    SubTest& operator= (const SubTest&);
};

#define DefineSubTest(function) SubTest(#function, function)

bool RunSubTests(_In_ const char* suiteName, _In_count_(cTests) SubTest* subTests, _In_ size_t cTests);

