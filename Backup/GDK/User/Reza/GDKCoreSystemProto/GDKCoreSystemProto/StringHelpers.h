#if defined(_MSC_VER)
#pragma once
#endif

#ifndef _GDK_STRINGHELPERS_H_
#define _GDK_STRINGHELPERS_H_

namespace GDK
{
    bool GDK_API IsNullOrEmpty(_In_ const wchar_t* str);
    bool GDK_API IsWhitespaceChar(_In_ wchar_t c);
    bool GDK_API IsCRLFChar(_In_ wchar_t c);
    bool GDK_API IsUTFBOMChar(_In_ wchar_t c);

    bool GDK_API StartsWith(_In_ const wchar_t* str, _In_ const wchar_t* pattern, _In_ bool caseSensitive = true);
    bool GDK_API EndsWith(_In_ const wchar_t* str, _In_ const wchar_t* pattern, _In_ bool caseSensitive = true);

    void GDK_API TrimWhitespaces(_Inout_ wchar_t*& str);
    void GDK_API TrimTrailingWhitespaces(_Inout_ wchar_t*& str);
    void GDK_API TrimCRLF(_Inout_ wchar_t*& str);
    void GDK_API TrimUTFBOM(_Inout_ wchar_t*& str);
} // GDK

#endif // _GDK_STRINGHELPERS_H_
