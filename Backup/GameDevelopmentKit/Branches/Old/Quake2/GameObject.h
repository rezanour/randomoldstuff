#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

namespace Quake2
{
    class GameObject : public GDK::RuntimeObject<GDK::IGameObjectEdit>
    {
    public:
        static Microsoft::WRL::ComPtr<GameObject> GDKAPI Create(_In_ const std::wstring& type, _In_ GDK::Graphics::IGraphicsDevice* graphics, _In_ GDK::Content::IContent* content);
        static Microsoft::WRL::ComPtr<GameObject> GDKAPI Load(_In_ GDK::IStream* stream, _In_ GDK::Graphics::IGraphicsDevice* graphics, _In_ GDK::Content::IContent* content);

        // TODO: Move these, not specific to GameObject
        static std::wstring GDKAPI GetStringFromStream(_In_ GDK::IStream* stream);
        static void GDKAPI WriteStringToStream(_In_ GDK::IStream* stream, _In_ const std::wstring& value);

        //
        // IProperties
        //
        virtual size_t GDKAPI GetPropertyCount() const;
        virtual HRESULT GDKAPI GetProperty(_In_z_ const wchar_t* name, _COM_Outptr_ GDK::IProperty** prop);
        virtual HRESULT GDKAPI GetPropertyByIndex(_In_ size_t index, _COM_Outptr_ GDK::IProperty** prop);

        //
        // Internal
        //
        const std::wstring& GDKAPI GetId() const
        {
            return _id;
        }

        void GDKAPI SetPosition(_In_ const GDK::Vector3& position)
        {
            _position = position;
            UpdateWorld();
        }

        const GDK::Matrix& GetWorld() const { return _worldMatrix; }

        GDK::Graphics::IRuntimeGeometry* GDKAPI GetGeometry() const { return _geometry.Get(); }
        GDK::Graphics::IRuntimeTexture* GDKAPI GetTexture() const { return _texture.Get(); }

        void GDKAPI Save(_In_ GDK::IStream* stream);

    protected:
        GameObject(_In_ const std::wstring& type);
        virtual ~GameObject();

        typedef std::map<std::wstring, Microsoft::WRL::ComPtr<GDK::IProperty>> PropertyMap;
        virtual void GDKAPI ConfigureProperties(_Inout_ PropertyMap& properties) = 0;

        void GDKAPI SetGraphicsAssets(_In_ GDK::Graphics::IRuntimeGeometry* geometry, _In_ GDK::Graphics::IRuntimeTexture* texture)
        {
            _geometry = geometry;
            _texture = texture;
        }

    private:
        void Initialize();
        void UpdateWorld();

        std::wstring _id;
        std::wstring _type;
        GDK::Vector3 _position;
        float _rotation;
        GDK::Matrix _worldMatrix;

        Microsoft::WRL::ComPtr<GDK::Graphics::IRuntimeGeometry> _geometry;
        Microsoft::WRL::ComPtr<GDK::Graphics::IRuntimeTexture> _texture;

        PropertyMap _properties;
    };
} // Quake2

#endif // _GAMEOBJECT_H_