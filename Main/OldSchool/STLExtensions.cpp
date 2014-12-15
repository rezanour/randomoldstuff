#include "Precomp.h"

namespace std
{
    _Use_decl_annotations_
    std::wstring AnsiToWide(std::string s)
    {
        std::wstring ws(s.size(), L' ');
        std::copy(s.begin(), s.end(), ws.begin());
        return ws;
    }
}