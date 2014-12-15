#pragma once

namespace GDK {
namespace Content {

    class TextureResource : public RuntimeObject<ITextureResource>
    {
    public:
        explicit TextureResource(_In_ std::istream* stream);

        // ITextureResource
        virtual HRESULT GDKAPI GetName(_Out_ const wchar_t** name);
        virtual HRESULT GDKAPI GetFormat(_Out_ uint32_t* format);
        virtual HRESULT GDKAPI GetImageCount(_Out_ uint32_t* count);
        virtual HRESULT GDKAPI GetSize(_In_ uint32_t index, _Out_ uint32_t* width, uint32_t* height);
        virtual HRESULT GDKAPI GetPixels(_In_ uint32_t index, _Out_ byte_t** pixels);
        
    private:
        TEXTURE_RESOURCE_FILEHEADER _header;
        std::vector<TEXTURE_RESOURCE_IMAGE> _images;
        std::wstring _resourceName;
    };

} // Content
} // GDK
