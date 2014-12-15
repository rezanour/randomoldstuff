#if defined(_MSC_VER)
#pragma once
#endif

#ifndef _GDK_STRINGID_H_
#define _GDK_STRINGID_H_

namespace GDK
{

    // Should be treated as an opaque handle, internals subject to change
    struct StringID
    {
        static const StringID Invalid;

        // Creation
        StringID();
        StringID(_In_ const StringID& other);

        // Operators
        StringID& operator= (_In_ const StringID& other);
        bool operator== (_In_ const StringID& other) const;
        bool operator!= (_In_ const StringID& other) const;
        bool operator< (_In_ const StringID& other) const;
        bool operator> (_In_ const StringID& other) const;
        operator bool() const;

        // Obtaining and resolving string IDs
        static StringID GDK_API         RegisterString(_In_ const wchar_t* value);
        static StringID GDK_API         GetStringID(_In_ const wchar_t* value);
        static const wchar_t* GDK_API   GetString(_In_ const StringID& stringID);

    private:
        StringID(_In_ size_t value);
        size_t _value;

        friend class Var;
    };

} // GDK

#endif // _GDK_STRINGID_H_
