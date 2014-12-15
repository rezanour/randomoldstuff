#ifndef _GAMEWORLD_H_
#define _GAMEWORLD_H_

namespace Quake2
{
    class Quake2Game;
    class GameObject;

    class GameWorld : public GDK::RuntimeObject<GDK::IWorldEdit>
    {
    public:
        static Microsoft::WRL::ComPtr<GameWorld> Create(_In_ Quake2Game* game, _In_opt_z_ const wchar_t* name);
        static Microsoft::WRL::ComPtr<GameWorld> Create(_In_ Quake2Game* game, _In_ GDK::IStream* stream);
        GameWorld(_In_ Quake2Game* game, _In_opt_z_ const wchar_t* name);

        //
        // IWorldEdit
        //
        virtual const wchar_t* GDKAPI GetName() const;
        virtual void GDKAPI SetName(_In_z_ const wchar_t* name);

        virtual HRESULT GDKAPI Save(_In_ GDK::IStream* stream);

        //
        // GameObjects
        //
        virtual HRESULT GDKAPI CreateObject(_In_z_ const wchar_t* type, _COM_Outptr_ GDK::IGameObjectEdit** gameObject);
        virtual HRESULT GDKAPI GetObjects(_COM_Outptr_ GDK::IStringList** ids);
        virtual HRESULT GDKAPI SetSelection(_In_opt_z_ const wchar_t* id);
        virtual HRESULT GDKAPI RemoveObject(_In_z_ const wchar_t* id);
        virtual HRESULT GDKAPI GetObject(_In_z_ const wchar_t* id, _COM_Outptr_ GDK::IGameObjectEdit** gameObject);
        virtual HRESULT GDKAPI PickClosestObject(_In_ const GDK::Matrix& view, _In_ const GDK::Matrix& projection, _In_ const GDK::Vector2& normalizedScreenPosition, _COM_Outptr_ GDK::IGameObjectEdit** gameObject);

        //
        // Internal
        //
        void GDKAPI Update(_In_ double elapsedSeconds);
        void GDKAPI Draw();

    private:
        ~GameWorld();

        std::wstring _name;
        Microsoft::WRL::ComPtr<Quake2Game> _game;
        std::vector<Microsoft::WRL::ComPtr<GameObject>> _gameObjects;
    };
} // Quake2

#endif // _GAMEWORLD_H_