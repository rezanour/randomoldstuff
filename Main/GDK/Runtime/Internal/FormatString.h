#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

namespace GDK
{
    namespace Private
    {
        struct Token
        {
            std::wstring Value;
            int Index;
        };

        // helper to parse a format string into tokens
        std::vector<Token> ParseTokens(const std::wstring &format, int maxIndex);
    }

    template<class T0>
    std::wstring FormatString(const std::wstring &format, T0 value0)
    {
        auto tokens = Private::ParseTokens(format, 0);
        std::wstringstream ss;
        for (auto itr = std::begin(tokens); itr != std::end(tokens); itr++)
        {
            switch (itr->Index)
            {   
                case 0: ss << value0; break;
                default: ss << itr->Value; break;
            }
        }
        return ss.str();
    }
        
    template<class T0, class T1>
    std::wstring FormatString(const std::wstring &format, T0 value0, T1 value1)
    {
        auto tokens = Private::ParseTokens(format, 1);
        std::wstringstream ss;
        for (auto itr = std::begin(tokens); itr != std::end(tokens); itr++)
        {
            switch (itr->Index)
            {   
                case 0: ss << value0; break;
                case 1: ss << value1; break;
                default: ss << itr->Value; break;
            }
        }
        return ss.str();
    }
        
    template<class T0, class T1, class T2>
    std::wstring FormatString(const std::wstring &format, T0 value0, T1 value1, T2 value2)
    {
        auto tokens = Private::ParseTokens(format, 2);
        std::wstringstream ss;
        for (auto itr = std::begin(tokens); itr != std::end(tokens); itr++)
        {
            switch (itr->Index)
            {   
                case 0: ss << value0; break;
                case 1: ss << value1; break;
                case 2: ss << value2; break;
                default: ss << itr->Value; break;
            }
        }
        return ss.str();
    }
        
    template<class T0, class T1, class T2, class T3>
    std::wstring FormatString(const std::wstring &format, T0 value0, T1 value1, T2 value2, T3 value3)
    {
        auto tokens = Private::ParseTokens(format, 3);
        std::wstringstream ss;
        for (auto itr = std::begin(tokens); itr != std::end(tokens); itr++)
        {
            switch (itr->Index)
            {   
                case 0: ss << value0; break;
                case 1: ss << value1; break;
                case 2: ss << value2; break;
                case 3: ss << value3; break;
                default: ss << itr->Value; break;
            }
        }
        return ss.str();
    }
        
    template<class T0, class T1, class T2, class T3, class T4>
    std::wstring FormatString(const std::wstring &format, T0 value0, T1 value1, T2 value2, T3 value3, T4 value4)
    {
        auto tokens = Private::ParseTokens(format, 4);
        std::wstringstream ss;
        for (auto itr = std::begin(tokens); itr != std::end(tokens); itr++)
        {
            switch (itr->Index)
            {   
                case 0: ss << value0; break;
                case 1: ss << value1; break;
                case 2: ss << value2; break;
                case 3: ss << value3; break;
                case 4: ss << value4; break;
                default: ss << itr->Value; break;
            }
        }
        return ss.str();
    }
        
    template<class T0, class T1, class T2, class T3, class T4, class T5>
    std::wstring FormatString(const std::wstring &format, T0 value0, T1 value1, T2 value2, T3 value3, T4 value4, T5 value5)
    {
        auto tokens = Private::ParseTokens(format, 5);
        std::wstringstream ss;
        for (auto itr = std::begin(tokens); itr != std::end(tokens); itr++)
        {
            switch (itr->Index)
            {   
                case 0: ss << value0; break;
                case 1: ss << value1; break;
                case 2: ss << value2; break;
                case 3: ss << value3; break;
                case 4: ss << value4; break;
                case 5: ss << value5; break;
                default: ss << itr->Value; break;
            }
        }
        return ss.str();
    }
        
    template<class T0, class T1, class T2, class T3, class T4, class T5, class T6>
    std::wstring FormatString(const std::wstring &format, T0 value0, T1 value1, T2 value2, T3 value3, T4 value4, T5 value5, T6 value6)
    {
        auto tokens = Private::ParseTokens(format, 6);
        std::wstringstream ss;
        for (auto itr = std::begin(tokens); itr != std::end(tokens); itr++)
        {
            switch (itr->Index)
            {   
                case 0: ss << value0; break;
                case 1: ss << value1; break;
                case 2: ss << value2; break;
                case 3: ss << value3; break;
                case 4: ss << value4; break;
                case 5: ss << value5; break;
                case 6: ss << value6; break;
                default: ss << itr->Value; break;
            }
        }
        return ss.str();
    }
        
    template<class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
    std::wstring FormatString(const std::wstring &format, T0 value0, T1 value1, T2 value2, T3 value3, T4 value4, T5 value5, T6 value6, T7 value7)
    {
        auto tokens = Private::ParseTokens(format, 7);
        std::wstringstream ss;
        for (auto itr = std::begin(tokens); itr != std::end(tokens); itr++)
        {
            switch (itr->Index)
            {   
                case 0: ss << value0; break;
                case 1: ss << value1; break;
                case 2: ss << value2; break;
                case 3: ss << value3; break;
                case 4: ss << value4; break;
                case 5: ss << value5; break;
                case 6: ss << value6; break;
                case 7: ss << value7; break;
                default: ss << itr->Value; break;
            }
        }
        return ss.str();
    }
        
    template<class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
    std::wstring FormatString(const std::wstring &format, T0 value0, T1 value1, T2 value2, T3 value3, T4 value4, T5 value5, T6 value6, T7 value7, T8 value8)
    {
        auto tokens = Private::ParseTokens(format, 8);
        std::wstringstream ss;
        for (auto itr = std::begin(tokens); itr != std::end(tokens); itr++)
        {
            switch (itr->Index)
            {   
                case 0: ss << value0; break;
                case 1: ss << value1; break;
                case 2: ss << value2; break;
                case 3: ss << value3; break;
                case 4: ss << value4; break;
                case 5: ss << value5; break;
                case 6: ss << value6; break;
                case 7: ss << value7; break;
                case 8: ss << value8; break;
                default: ss << itr->Value; break;
            }
        }
        return ss.str();
    }
        
    template<class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
    std::wstring FormatString(const std::wstring &format, T0 value0, T1 value1, T2 value2, T3 value3, T4 value4, T5 value5, T6 value6, T7 value7, T8 value8, T9 value9)
    {
        auto tokens = Private::ParseTokens(format, 9);
        std::wstringstream ss;
        for (auto itr = std::begin(tokens); itr != std::end(tokens); itr++)
        {
            switch (itr->Index)
            {   
                case 0: ss << value0; break;
                case 1: ss << value1; break;
                case 2: ss << value2; break;
                case 3: ss << value3; break;
                case 4: ss << value4; break;
                case 5: ss << value5; break;
                case 6: ss << value6; break;
                case 7: ss << value7; break;
                case 8: ss << value8; break;
                case 9: ss << value9; break;
                default: ss << itr->Value; break;
            }
        }
        return ss.str();
    }
}