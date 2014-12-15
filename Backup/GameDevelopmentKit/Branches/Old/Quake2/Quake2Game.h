#ifndef _QUAKE2GAME_H_
#define _QUAKE2GAME_H_

namespace Quake2
{
    //
    // Quake2 Game Factory
    //

    class Quake2GameFactory : public GDK::RuntimeObject<GDK::IGameFactory>
    {
        IMPL_GUID(0x44abc0b3, 0x58e7, 0x4ef9, 0x80, 0x6a, 0xa5, 0x8c, 0x91, 0x39, 0x45, 0xf6);
    public:
        //
        // IGameFactory
        //
        virtual HRESULT GDKAPI CreateGame(_In_ const GDK::GameCreationParameters& parameters, _In_opt_z_ const wchar_t* initialWorld, _COM_Outptr_ GDK::IGame** game);
        virtual HRESULT GDKAPI CreateGameEdit(_In_ const GDK::GameCreationParameters& parameters, _COM_Outptr_ GDK::IGameEdit** game);
    };

    class Quake2Game : public GDK::RuntimeObject<GDK::IGame, GDK::IGameEdit>
    {
        IMPL_GUID(0x44f52b65, 0x2f6d, 0x44b1, 0x9f, 0x34, 0xfa, 0xa1, 0x8c, 0x5c, 0x9a, 0x21);
    public:
        static const wchar_t* const DisplayName;

        static Microsoft::WRL::ComPtr<Quake2Game> GDKAPI Create(_In_ const GDK::GameCreationParameters& parameters, _In_opt_z_ const wchar_t* initialWorld);
        static Microsoft::WRL::ComPtr<Quake2Game> GDKAPI CreateEdit(_In_ const GDK::GameCreationParameters& parameters);
        Quake2Game(_In_ const GDK::GameCreationParameters& parameters, _In_ bool editing);

        //
        // IGame
        //
        virtual const wchar_t* GDKAPI GetName() const;
        virtual HRESULT GDKAPI Update(_In_ double elapsedSeconds);
        virtual HRESULT GDKAPI Draw(_In_ const GDK::Matrix& view, _In_ const GDK::Matrix& projection);

        //
        // IGameEdit
        //
        virtual HRESULT GDKAPI CreateWorld(_In_opt_z_ const wchar_t* name, _COM_Outptr_ GDK::IWorldEdit** blankWorld);
        virtual HRESULT GDKAPI LoadWorld(_In_ GDK::IStream* stream, _COM_Outptr_ GDK::IWorldEdit** world);
        virtual HRESULT GDKAPI SetWorld(_In_ GDK::IWorldEdit* world);
        virtual HRESULT GDKAPI GetRequiredResources(_COM_Outptr_ GDK::IStringList** requiredResources) const;
        virtual HRESULT GDKAPI SetSelectionColor(_In_ const GDK::Vector4& color);
        virtual HRESULT GDKAPI SetEditWidget(_In_ GDK::Graphics::IRuntimeGeometry* widgetGeometry);

        //
        // Internal
        //
        GDK::Graphics::IGraphicsDevice* Graphics() { return _graphicsDevice.Get(); }
        GDK::Content::IContent* Content() { return _content.Get(); }

    private:
        ~Quake2Game();

        Microsoft::WRL::ComPtr<GDK::IGameHost> _host;
        Microsoft::WRL::ComPtr<GDK::Graphics::IGraphicsDevice> _graphicsDevice;
        Microsoft::WRL::ComPtr<GDK::Content::IContent> _content;
        Microsoft::WRL::ComPtr<GameWorld> _world;
        bool _editing;
    };
}

#endif // _QUAKE2GAME_H_