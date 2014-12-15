#include "stdafx.h"

#pragma warning(disable : 4244) // '=' : conversion from 'wchar_t' to 'char', possible loss of data

std::wstring FileFromFilePath(LPCWSTR szFilePath, BOOL includeExtensionInName)
{
    // Trim filename from full path
    std::wstring filePath = szFilePath;
    size_t pos = filePath.find_last_of(L"\\");
    std::wstring fileName = filePath.substr(pos + 1);
    
    // Trim filename without extension
    pos = fileName.find_last_of(L".");
    std::wstring name = fileName.substr(0, pos);

    if (includeExtensionInName)
    {
        return fileName;
    }
    else
    {
        return name;
    }
}

std::wstring FilePathwithNewExtension(LPCWSTR szFilePath, LPCWSTR szExtension)
{
    // Trim filenpath without extension from full path
    std::wstring filePath = szFilePath;
    size_t pos = filePath.find_last_of(L".");
    filePath = filePath.substr(0, pos);
    
    std::wstring extension = szExtension;
    pos = extension.find_last_of(L".");
    if (pos != std::wstring::npos)
    {
        extension = extension.substr(pos + 1);
    }

    // Add extension
    filePath = filePath + L"." + extension;

    return filePath;
}

std::wstring FilePathOnly(LPCWSTR szFilePath)
{
    // Trim filenpath without extension from full path
    std::wstring filePath = szFilePath;
    size_t pos = filePath.find_last_of(L"\\");
    filePath = filePath.substr(0, pos);
    filePath += L"\\";
    return filePath;
}

std::wstring AnsiToWide(std::string s)
{
    std::wstring ws(s.size(), L' ');
    std::copy(s.begin(), s.end(), ws.begin());
    return ws;
}

std::string WideToAnsi(std::wstring ws)
{
    std::string s(ws.size(), ' ');
    std::copy(ws.begin(), ws.end(), s.begin());
    return s;
}