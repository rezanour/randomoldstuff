#pragma once

#ifdef WIN32

namespace GDK
{
    GDKINTERFACE IStream : public ::IStream
    {
        IMPL_GUID(0x20ad4113, 0xda6c, 0x4242, 0x9d, 0x53, 0x8c, 0xc4, 0x1a, 0xc0, 0xfb, 0xb1);

        virtual HRESULT GDKAPI SeekTo(DWORD dwOrigin, LONG offset) = 0;
        virtual HRESULT GDKAPI GetName(_Out_ const wchar_t** name) = 0;
    };
    
    extern "C" HRESULT CreateFileStream(_In_z_ const wchar_t* path, bool readOnly, _COM_Outptr_ IStream** stream);
    extern "C" HRESULT CreateMemoryStream(_In_ size_t length, _COM_Outptr_ IStream** stream);
}

#endif