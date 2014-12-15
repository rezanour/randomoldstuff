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

    std::wstring FileNameFromPath(_In_ const std::wstring& path)
    {
        std::wstring fileName;
        size_t pos = path.rfind(L"\\");
        if (pos != std::wstring::npos)
        {
            fileName = path.substr(pos + 1);
        }
        else
        {
            fileName = path;
        }

        return fileName;
    }

    void ReplaceCharacter(_Inout_ std::wstring& s, _In_ wchar_t fromCh, _In_ wchar_t toCh)
    {
        for (size_t i = 0; i < s.size(); i++)
        {
            if (s[i] == fromCh)
            {
                s[i] = toCh;
            }
        }
    }

    bool IsEncodedTextureContentPath(_In_ const std::wstring& contentPath )
    {
        return (contentPath.rfind(L"%") != std::wstring::npos);
    }

    std::wstring GetTextureContentPathFromGeometryContentPath(_In_ const std::wstring& contentPath)
    {
        std::wstring texturePath;
        texturePath = std::FileNameFromPath(contentPath);
        std::ReplaceCharacter(texturePath, L'%', L'\\');

        size_t pos = texturePath.find_last_of(L".");
        if (pos != std::wstring::npos)
        {
            texturePath = texturePath.substr(0, pos);
            texturePath.append(L".texture");
        }

        return texturePath;
    }

};