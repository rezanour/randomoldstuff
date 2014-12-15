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

std::wstring FileExtensionOnly(LPCWSTR szExtension)
{
    std::wstring extension = szExtension;
    size_t pos = extension.find_last_of(L".");
    if (pos != std::wstring::npos)
    {
        extension = extension.substr(pos + 1);
    }

    return extension;
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

std::wstring FilePathOnlyEx(LPCWSTR szFilePath)
{
    // Trim filenpath without extension from full path
    std::wstring filePath = szFilePath;
    size_t pos = filePath.find_last_of(L"\\");
    filePath = filePath.substr(0, pos);
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

void GetExtensionsListFromFilteredExtensions(LPCWSTR szfilter, std::vector<std::wstring>& extensionsList)
{
    std::wstring sfilter = szfilter;
    std::wistringstream ss( sfilter );
    std::wstring extension;
    
    do
    {
        extension.clear();
        std::getline( ss, extension, L';' );
        if (extension.length() > 0)
        {
            trim_spaces(extension);
            //if (extension != L"*.*")
            {
                trim_left(extension, L"*.");
                extensionsList.push_back(extension);
            }
        }
    }while(extension.length() != 0);
}

void GetExtensionsListFromFilter(LPCWSTR szfilter, std::vector<std::wstring>& extensionsList)
{
    std::wstring sfilter = szfilter;
    std::wistringstream ss( sfilter );
    std::wstring description;
    std::wstring extension;
    
    do
    {
        description.clear();
        extension.clear();

        std::getline( ss, description, L'|' );
        std::getline( ss, extension, L'|' );
        if (extension.length() > 0)
        {
            // more than one extension can be in a single group (; delimited), so further parsing may be required
            GetExtensionsListFromFilteredExtensions(extension.c_str(), extensionsList);
        }
    }while(description.length() != 0 && (extension.length() != 0));

}

void ConvertFilterWithEmbeddedNullsInitialFilterVersion(LPCWSTR szFilter, std::wstring& outFilter)
{
    int i = 0;
    do
    {
        if (szFilter[i] != 0)
        {
            outFilter += szFilter[i];
        }
        else if (szFilter[i] == 0)
        {
            outFilter += L'|';
            if (szFilter[i + 1] == 0)
            {
                return;
            }
        }

        i++;
    }while (true);
}