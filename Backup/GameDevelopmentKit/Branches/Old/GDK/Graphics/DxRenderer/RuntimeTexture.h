#pragma once

namespace GDK {
namespace Graphics {

    class RuntimeTexture : public RuntimeObject<IRuntimeTexture>
    {
    public:
        //
        // All textures in Quake2 use the same format, R8G8B8A8_UNORM
        //
        static Microsoft::WRL::ComPtr<RuntimeTexture> GDKAPI Create(_In_ DxGraphicsDevice* device, _In_ GDK::Content::ITextureResource* resource);
        static Microsoft::WRL::ComPtr<RuntimeTexture> GDKAPI CreateFromData(_In_ DxGraphicsDevice* device, _In_ size_t width, _In_ size_t height, _In_ const byte_t* pixels);
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
        void Bind(_In_ DxGraphicsDevice* device);
        void Unbind(_In_ DxGraphicsDevice* device);

    private:
        bool _isBound;
        std::wstring _name;

        Microsoft::WRL::ComPtr<ID3D11Texture2D> _texture;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _srv;
    };

} // Graphics
} // GDK
