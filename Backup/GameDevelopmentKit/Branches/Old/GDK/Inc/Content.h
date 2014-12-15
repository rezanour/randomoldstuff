#pragma once

namespace GDK {
namespace Content {

    struct ITextureResource;
    struct IGeometryResource;
    struct IContent;

#ifdef WIN32
    GDKINTERFACE IContentFactory : public IUnknown
    {
        IMPL_GUID(0x7df62397, 0x6dcd, 0x41ed, 0x90, 0x64, 0xa1, 0xdf, 0x36, 0x9b, 0xac, 0x47);

        virtual HRESULT GDKAPI CreateContent(_In_ IStream* data, _COM_Outptr_ IContent** content)= 0;        
    };
#endif

    GDKINTERFACE IContent : public IUnknown
    {
        IMPL_GUID(0x2ea22bec, 0xddcb, 0x4ae1, 0xb1, 0x2, 0x83, 0xcd, 0x45, 0x6f, 0x4c, 0x9b);

#ifdef WIN32
        virtual HRESULT GDKAPI CreateResourceFactory(_COM_Outptr_ IResourceFactory** factory) = 0;
#endif
        virtual HRESULT GDKAPI GetGeometryResource(_In_z_ const wchar_t* name, _COM_Outptr_ IGeometryResource** resource) = 0;
        virtual HRESULT GDKAPI GetTextureResource(_In_z_ const wchar_t* name, _COM_Outptr_ ITextureResource** resource) = 0;
        virtual HRESULT GDKAPI GetWorldResource(_In_z_ const wchar_t* name, _COM_Outptr_ IWorldResource** resource) = 0;
    };

} // Content
} // GDK
