#pragma once

namespace GDK {
namespace Content {

    class TextureResourceEdit : public RuntimeObject<ITextureResourceEdit, ITextureResource>
    {
    public:
        TextureResourceEdit();

        // IPersistResource
        virtual HRESULT GDKAPI Save(_In_ GDK::IStream* output);
        virtual HRESULT GDKAPI Load(_In_ GDK::IStream* input);
        virtual HRESULT GDKAPI SetName(_In_ const wchar_t* name);

        // ITextureResourceEdit
        virtual HRESULT GDKAPI CreateTextureResource(_COM_Outptr_ ITextureResource** resource);

        // ITextureResource
        virtual HRESULT GDKAPI GetName(_Out_ const wchar_t** name);
        virtual HRESULT GDKAPI GetFormat(_Out_ uint32_t* format);
        virtual HRESULT GDKAPI GetImageCount(_Out_ uint32_t* count);
        virtual HRESULT GDKAPI GetSize(_In_ uint32_t index, _Out_ uint32_t* width, uint32_t* height);
        virtual HRESULT GDKAPI GetPixels(_In_ uint32_t index, _Out_ const byte_t** pixels);
        
    private:
        ~TextureResourceEdit();

        void FreeImages();

        TEXTURE_RESOURCE_FILEHEADER _header;
        std::vector<TEXTURE_RESOURCE_IMAGE> _images;
        std::wstring _resourceName;
    };

} // Content
} // GDK
