#ifndef _GDKSTRING_H_
#define _GDKSTRING_H_

namespace GDK
{
    //
    // This interface is designed to assist with passing strings across marshaling boundaries
    //
    GDKINTERFACE IString : public IUnknown
    {
        IMPL_GUID(0x4b16d89b, 0x3ecc, 0x45db, 0x90, 0x9f, 0x51, 0xcf, 0x50, 0x8e, 0xec, 0x71);

        virtual bool GDKAPI IsNullOrEmpty() const = 0;
        virtual size_t GDKAPI GetLength() const = 0;
        virtual const wchar_t* GDKAPI GetBuffer() const = 0;
    };

    GDKINTERFACE IStringList : public IUnknown
    {
        IMPL_GUID(0xa500ad44, 0xc80d, 0x4b44, 0x80, 0x13, 0x84, 0xf5, 0xf4, 0xf1, 0xe9, 0x33);

        virtual size_t GDKAPI GetCount() const = 0;
        virtual HRESULT GDKAPI GetAt(_In_ size_t index, _COM_Outptr_ IString** string) const = 0;
    };

    //
    // The GDK engine contains a standard implementation
    //
    extern "C" HRESULT CreateString(_In_opt_z_ const wchar_t* value, _COM_Outptr_ IString** string);
    extern "C" HRESULT CreateStringList(_In_opt_z_count_(count) const wchar_t** values, _In_ size_t count, _COM_Outptr_ IStringList** stringList);
}

#endif // _GDKSTRING_H_