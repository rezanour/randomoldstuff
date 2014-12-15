#ifndef _ENEMY_H_
#define _ENEMY_H_

namespace Quake2
{
    class Enemy : public GameObject
    {
    public:
        static Microsoft::WRL::ComPtr<GameObject> GDKAPI Create(_In_ const std::wstring& type, _In_ GDK::Graphics::IGraphicsDevice* graphics, _In_ GDK::Content::IContent* content);
        Enemy(_In_ const std::wstring& type);

    protected:
        virtual ~Enemy();

        virtual void GDKAPI ConfigureProperties(_Inout_ GameObject::PropertyMap& properties);

    private:
        int64_t _health;
        int64_t _armor;
    };
} // Quake2

#endif // _ENEMY_H_