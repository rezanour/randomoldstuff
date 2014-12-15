#pragma once

namespace GDK {
namespace Content {

    class ContentArchiveFactory : public RuntimeObject<IContentFactory>
    {
        IMPL_GUID(0xf954415a, 0x201f, 0x400f, 0xbf, 0xea, 0xa, 0xa4, 0x6f, 0x36, 0x3a, 0xdd);
    public:
        // IContentFactory
        virtual HRESULT GDKAPI CreateContent(_In_ IStream* data, _COM_Outptr_ IContent** content);
    };

    class ContentArchive : public RuntimeObject<IContent>
    {
    public:
        ContentArchive(_In_ IStream* data);

        // IContent
        virtual HRESULT GDKAPI CreateResourceFactory(_COM_Outptr_ IResourceFactory** factory);
        virtual HRESULT GDKAPI GetGeometryResource(_In_z_ const wchar_t* name, _COM_Outptr_ IGeometryResource** resource);
        virtual HRESULT GDKAPI GetTextureResource(_In_z_ const wchar_t* name, _COM_Outptr_ ITextureResource** resource);
        virtual HRESULT GDKAPI GetWorldResource(_In_z_ const wchar_t* name, _COM_Outptr_ IWorldResource** resource);
        
    private:
        ~ContentArchive();
        bool IsAbsolutePath(std::wstring& path);
        std::wstring DirectoryRootFromPath(std::wstring path);
        
        HRESULT SeekToResource(_In_z_ const wchar_t* name, _COM_Outptr_ IStream** stream);
        HRESULT CopyCharArrayAsWideString(char* chars, int numchars, std::wstring& str);

        Microsoft::WRL::ComPtr<IStream> _stream;
        bool _fileBased;
        std::wstring _rootPath;
    };

} // Content
} // GDK
