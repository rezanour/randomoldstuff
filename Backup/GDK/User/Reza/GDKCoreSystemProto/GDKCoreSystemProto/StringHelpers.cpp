#include "StdAfx.h"
#include "StringHelpers.h"

bool GDK_API GDK::IsNullOrEmpty(_In_ const wchar_t* str)
{
    return (!str || *str == 0);
}

bool GDK_API GDK::IsWhitespaceChar(_In_ wchar_t c)
{
    return (c == L' ' || c == L'\t');
}

bool GDK_API GDK::IsCRLFChar(_In_ wchar_t c)
{
    return (c == L'\n' || c == L'\r');
}

bool GDK_API GDK::IsUTFBOMChar(_In_ wchar_t c)
{
    // UTF-8 Byte Order Mark is the sequence { 0xEF,0xBB,0xBF }
    // UTF-16 Byte Order Mark is either { 0xFEFF or 0xFFFE depending on endianness }

    return (c == 0xFFFE ||
            c == 0xFEFF ||
            c == 0xEFBB ||
            c == 0xBBEF);
}

bool GDK_API GDK::StartsWith(_In_ const wchar_t* str, _In_ const wchar_t* pattern, _In_ bool caseSensitive)
{
    if (IsNullOrEmpty(str))
        return false;

    if (IsNullOrEmpty(pattern))
        return false;

    size_t strLen = wcslen(str);
    size_t patternLen = wcslen(pattern);

    if (patternLen > strLen)
        return false;

    return (caseSensitive ? 
                wcsncmp(str, pattern, patternLen) == 0 : 
                _wcsnicmp(str, pattern, patternLen) == 0);
}

bool GDK_API GDK::EndsWith(_In_ const wchar_t* str, _In_ const wchar_t* pattern, _In_ bool caseSensitive)
{
    if (IsNullOrEmpty(str))
        return false;

    if (IsNullOrEmpty(pattern))
        return false;

    size_t strLen = wcslen(str);
    size_t patternLen = wcslen(pattern);

    if (patternLen > strLen)
        return false;

    const wchar_t* p = str + (strLen - patternLen);

    return (caseSensitive ? 
                wcsncmp(p, pattern, patternLen) == 0 : 
                _wcsnicmp(p, pattern, patternLen) == 0);
}

void GDK_API GDK::TrimWhitespaces(_Inout_ wchar_t*& str)
{
    if (IsNullOrEmpty(str))
        return;

    while (IsWhitespaceChar(*str))
        ++str;
}

void GDK_API GDK::TrimTrailingWhitespaces(_Inout_ wchar_t*& str)
{
    if (IsNullOrEmpty(str))
        return;

    wchar_t* end = str;
    while (*end != 0)
        ++end;

    --end;
    while (IsWhitespaceChar(*end))
    {
        --end;
    }

    *(end + 1) = 0;
}

void GDK_API GDK::TrimCRLF(_Inout_ wchar_t*& str)
{
    if (IsNullOrEmpty(str))
        return;

    while (IsCRLFChar(*str))
        ++str;
}

void GDK_API GDK::TrimUTFBOM(_Inout_ wchar_t*& str)
{
    if (IsNullOrEmpty(str))
        return;

    while (IsUTFBOMChar(*str))
        ++str;
}
