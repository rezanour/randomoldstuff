#include <FormatString.h>
#include <GDKError.h>

using namespace GDK;
using namespace GDK::Private;

std::vector<Token> GDK::Private::ParseTokens(const std::wstring &format, int maxIndex)
{
    std::vector<Token> tokens;

    std::wstringstream ss(format);
    wchar_t c;

    // holds token std::string values while we build them up
    std::wstringstream tokenValue;

    while (ss.get(c))
    {
        // look for the opening bracket of an index token
        if (c == '{')
        {
            // try to find the end, while reading the contents into another stream
            std::wstringstream inner;            
            while (ss.get(c) && c != '}')
            {
                inner << c;
            }

            // if we found the closing bracket of the index token, the tokenValue std::string needs to become a 
            // real token in our list and we need to parse the inner value for the index number
            if (c == '}')
            {
                // first create the std::string token value
                {
                    Token t;
                    t.Index = -1;
                    t.Value = tokenValue.str();
                    tokenValue = std::wstringstream();
                    tokens.push_back(t);
                }

                // now get the index from the inner value
                int index;
                CHECK_TRUE(inner >> index && index >= 0 && index <= maxIndex);

                // now we can construct the token for this value
                {
                    Token t;
                    t.Index = index;
                    tokens.push_back(t);
                }
            }
            // otherwise we likely hit the end of the stream, so we just put the inner value into the tokenValue
            else
            {
                tokenValue << inner.str();
            }
        }
        // otherwise just add the character to the token value
        else
        {
            tokenValue << c;
        }
    }

    // if we have some value left over, make sure we add it to our token list
    if (tokenValue.str().size() > 0)
    {
        Token t;
        t.Index = -1;
        t.Value = tokenValue.str();
        tokens.push_back(t);
    }

    return tokens;
}