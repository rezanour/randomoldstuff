#include "stdafx.h"

namespace std
{
    std::wstring AnsiToWide(std::string s)
    {
        std::wstring ws(s.size(), L' ');
        std::copy(s.begin(), s.end(), ws.begin());
        return ws;
    }

    std::wstring DirectoryRootFromPath(std::wstring path)
    {
        std::wstring root;
        size_t pos = path.find_last_of(L"\\");
        if (pos != std::wstring::npos)
        {
            root = path.substr(0, pos);
        }
        else
        {
            root = path;
        }

        return root;
    }
};