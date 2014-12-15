// Tests the various helpers in the stde conversion header
#include "StdAfx.h"
#include "SubTest.h"

using namespace stde;

namespace
{
    bool StringToWStringTest();
    bool WStringToStringTest();
    bool NumericalToStringTest();
    bool StringToNumericalTest();
    bool InvalidTest();

    SubTest subTests[] =
    {
        DefineSubTest(StringToWStringTest),
        DefineSubTest(WStringToStringTest),
        DefineSubTest(NumericalToStringTest),
        DefineSubTest(StringToNumericalTest),
        DefineSubTest(InvalidTest),
    };

    bool StringToWStringTest()
    {
        std::string str("Hello, World");
        std::wstring wstr = to_wstring(str);

        return (wstr == L"Hello, World");
    }

    bool WStringToStringTest()
    {
        std::wstring wstr(L"Hello, World");
        std::string str = to_string(wstr);

        return (str == "Hello, World");
    }

    bool NumericalToStringTest()
    {
        int i = 10;
        float f = 1234.56f;

        std::string s1 = to_string<int>(i);
        std::wstring s2 = to_wstring<float>(f);

        if (s1 != "10")
            return false;

        if (s2 != L"1234.56")
            return false;

        return true;
    }

    bool StringToNumericalTest()
    {
        std::string s1("10");
        std::wstring s2(L"1234.567");

        int i;
        float f;

        if (!from_string<int>(s1, i))
            return false;

        if (!from_wstring<float>(s2, f))
            return false;

        if (i != 10)
            return false;

        if (f != 1234.567f)
            return false;

        return true;
    }

    bool InvalidTest()
    {
        std::string s1("ABC10");

        int i;
        // we expect false return (failure to parse).
        // so if we get true return, then we failed the test (we reported that we successfully parsed)
        if (from_string<int>(s1, i))
            return false;

        return true;
    }
}

bool ConversionTests()
{
    return RunSubTests(__FUNCTION__, subTests, _countof(subTests));
}

