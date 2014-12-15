#pragma once

namespace GDK {
namespace Graphics {

    class RuntimeTexture : public RuntimeObject<IRuntimeTexture>
    {
    public:
        static Microsoft::WRL::ComPtr<RuntimeTexture> GDKAPI Create(_In_ GDK::Content::ITextureResource* resource);
        static Microsoft::WRL::ComPtr<RuntimeTexture> GDKAPI CreateFromData(_In_ size_t width, _In_ size_t height, _In_ const byte_t* pixels);
        RuntimeTexture(_In_ const std::wstring& name);

        //
        // IRuntimeTexture
        //
        virtual bool GDKAPI IsBound() const
        {
            return _isBound;
        }

        virtual const wchar_t* GDKAPI GetName() const
        {
            return _name.c_str();
        }

        // Internal
        void Bind();
        void Unbind();

    private:
        ~RuntimeTexture();

        bool _isBound;
        std::wstring _name;

        UINT _texture;
    };

} // Graphics
} // GDK
