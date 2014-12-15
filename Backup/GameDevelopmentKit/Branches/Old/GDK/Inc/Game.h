#ifndef _GAME_H_
#define _GAME_H_

namespace GDK
{
    namespace Content
    {
        struct IContent;
    }

    struct IGameHost;
    struct IGame;
    struct IGameEdit;
    struct IWorldEdit;
    struct IGameObjectEdit;
    struct IProperties;
    struct IProperty;

    struct GameCreationParameters
    {
        IGameHost* host;

        // A mounted data archive object for the game to use
        Content::IContent* content;

        // Components for the game to use
        Graphics::IGraphicsDevice* graphicsDevice;
    };

    //
    // Games are advertised and exposed through the same factory pattern as components
    //
    GDKINTERFACE IGameFactory : public IUnknown
    {
        IMPL_GUID(0x9213683, 0xff4c, 0x49eb, 0x95, 0x6c, 0xc3, 0x9f, 0x96, 0xe7, 0xb9, 0x58);

        virtual HRESULT GDKAPI CreateGame(_In_ const GameCreationParameters& parameters, _In_opt_z_ const wchar_t* initialWorld, _COM_Outptr_ IGame** game) = 0;
        virtual HRESULT GDKAPI CreateGameEdit(_In_ const GameCreationParameters& parameters, _COM_Outptr_ IGameEdit** game) = 0;
    };

    //
    // IGameHost is implemented by the host running the game. This can be a platform specific .exe wrapper, 
    // ContentStudio or some other tool.
    //
    GDKINTERFACE IGameHost : public IUnknown
    {
        IMPL_GUID(0x8d9d1a97, 0xc341, 0x440c, 0xac, 0x3f, 0x73, 0x8b, 0xd6, 0xeb, 0xe9, 0x32);
    };

    //
    // The main game simulation interface. All games must implement this.
    //
    GDKINTERFACE IGame : public IUnknown
    {
        IMPL_GUID(0x3ebcd023, 0x42af, 0x480a, 0xb8, 0x3f, 0xb7, 0x83, 0x63, 0x2a, 0xdf, 0xe2);

        virtual const wchar_t* GDKAPI GetName() const = 0;

        // Steps the game simulation one frame. Returns HRESULT_FROM_WIN32(ERROR_NO_MORE_ITEMS) if the simulation has ended.
        virtual HRESULT GDKAPI Update(_In_ double elapsedSeconds) = 0;

        virtual HRESULT GDKAPI Draw(_In_ const Matrix& view, _In_ const Matrix& projection) = 0;
    };

    //
    // The games editing interface, if available. This allows realtime editing and debugging.
    //
    GDKINTERFACE IGameEdit : public IUnknown
    {
        IMPL_GUID(0x7d6fbc53, 0x79f0, 0x4008, 0x8d, 0x2e, 0x1a, 0xe3, 0xb0, 0x79, 0x29, 0x87);

        virtual HRESULT GDKAPI CreateWorld(_In_opt_z_ const wchar_t* name, _COM_Outptr_ IWorldEdit** blankWorld) = 0;
        virtual HRESULT GDKAPI LoadWorld(_In_ IStream* stream, _COM_Outptr_ IWorldEdit** world) = 0;
        virtual HRESULT GDKAPI SetWorld(_In_ IWorldEdit* world) = 0;

        virtual HRESULT GDKAPI GetRequiredResources(_COM_Outptr_ IStringList** requiredResources) const = 0;
        virtual HRESULT GDKAPI SetSelectionColor(_In_ const Vector4& color) = 0;
        virtual HRESULT GDKAPI SetEditWidget(_In_ Graphics::IRuntimeGeometry* widgetGeometry) = 0;
    };

    GDKINTERFACE IWorldEdit : public IUnknown
    {
        IMPL_GUID(0xcaaea36f, 0x3713, 0x4975, 0xb8, 0xef, 0x89, 0xef, 0xe0, 0x9f, 0x85, 0x9b);

        virtual const wchar_t* GDKAPI GetName() const = 0;
        virtual void GDKAPI SetName(_In_z_ const wchar_t* name) = 0;

        virtual HRESULT GDKAPI Save(_In_ IStream* stream) = 0;

        // GameObjects
        virtual HRESULT GDKAPI CreateObject(_In_z_ const wchar_t* type, _COM_Outptr_ IGameObjectEdit** gameObject) = 0;
        virtual HRESULT GDKAPI GetObjects(_COM_Outptr_ IStringList** ids) = 0;
        virtual HRESULT GDKAPI SetSelection(_In_opt_z_ const wchar_t* id) = 0;
        virtual HRESULT GDKAPI RemoveObject(_In_z_ const wchar_t* id) = 0;
        virtual HRESULT GDKAPI GetObject(_In_z_ const wchar_t* id, _COM_Outptr_ IGameObjectEdit** gameObject) = 0;
        virtual HRESULT GDKAPI PickClosestObject(_In_ const Matrix& view, _In_ const Matrix& projection, _In_ const Vector2& normalizedScreenPosition, _COM_Outptr_ IGameObjectEdit** gameObject) = 0;
    };

    GDKINTERFACE IProperties : public IUnknown
    {
        IMPL_GUID(0xb08e3651, 0xc45e, 0x421e, 0xbc, 0xc8, 0x40, 0x60, 0x1, 0x73, 0xb5, 0xc4);

        virtual size_t GDKAPI GetPropertyCount() const = 0;
        virtual HRESULT GDKAPI GetProperty(_In_z_ const wchar_t* name, _COM_Outptr_ IProperty** prop) = 0;
        virtual HRESULT GDKAPI GetPropertyByIndex(_In_ size_t index, _COM_Outptr_ IProperty** prop) = 0;
    };

    GDKINTERFACE IProperty : public IUnknown
    {
        IMPL_GUID(0x3fa477f8, 0xff15, 0x4e16, 0x8d, 0x5b, 0xd9, 0x16, 0xf8, 0xa1, 0x32, 0x3b);

        virtual const wchar_t* GDKAPI GetName() const = 0;
        virtual HRESULT GDKAPI ToString(_COM_Outptr_ IString** string) = 0;
    };

    GDKINTERFACE IStringProperty : public IProperty
    {
        IMPL_GUID(0x4b233561, 0xd7df, 0x4ac4, 0xbc, 0x1c, 0x71, 0xf7, 0x87, 0x68, 0x41, 0xe8);

        virtual const wchar_t* GDKAPI GetStringValue() = 0;
        virtual void GDKAPI SetStringValue(_In_z_ const wchar_t* value) = 0;
    };

    GDKINTERFACE IIntegerProperty : public IProperty
    {
        IMPL_GUID(0x216b0df2, 0x5d39, 0x4998, 0xb2, 0x17, 0xce, 0x25, 0xe3, 0x68, 0x69, 0x4d);

        virtual int64_t GDKAPI GetIntegerValue() = 0;
        virtual void GDKAPI SetIntegerValue(_In_ int64_t value) = 0;
    };

    GDKINTERFACE IGameObjectEdit : public IProperties
    {
        IMPL_GUID(0x20ad4113, 0xda6c, 0x4242, 0x9d, 0x53, 0x8c, 0xc4, 0x1a, 0xc0, 0xfb, 0xb1);
    };
}

#endif // _GAME_H_