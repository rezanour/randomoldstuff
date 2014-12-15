#pragma once

#include <GDK.h>
#include <RuntimeObject.h>
#include <ContentCache.h>
#include "Items.h"

namespace Quake2
{
    enum class ArmorType
    {
        Body,
        Combat,
        Jacket
    };

    class Hud : public GDK::RuntimeObject<Hud>
    {
    public:
        static std::shared_ptr<Hud> Create(const GDK::DeviceContext& deviceContext);

        bool Update(_In_ const GDK::GameTime& gameTime);
        void Render();
        bool IsVisible();
        void Show(bool show);

        void SetAmmoType(_In_ Item::Type type);
        void SetAmmo(_In_ byte_t ammo);

        void SetArmorType(_In_ ArmorType type);
        void SetArmor(_In_ byte_t armor);

        void SetWeapon(_In_ Item::Type type);
        void SetHealth(_In_ byte_t health);

        void SetOwnerWorldMatrix(_In_ const GDK::Matrix& world);

    private:
        Hud(const GDK::DeviceContext& deviceContext);

        void DrawNumber(_In_ int x, _In_ int y, _In_ int number);
        void DrawDigits(_In_ int x, _In_ int y, _In_ byte_t* digits, _In_ int count);
        void NumberToDigitArray(_In_ int value, _Inout_ byte_t* digits, _In_ int count);

        bool _visible;

        std::vector<std::wstring> _numberTextures;
        std::vector<std::wstring> _miscTextures;
        std::vector<std::wstring> _armorTextures;
        
        std::map<Item::Type, std::wstring> _vweaponTextures;
        std::map<Item::Type, std::wstring> _vweaponGeometry;

        GDK::Matrix _weaponWorldPosition;

        GDK::DeviceContext _deviceContext;

        Item::Type _weaponType;
        Item::Type _ammoType;
        ArmorType _armorType;
        byte_t _ammo;
        byte_t _health;
        byte_t _armor;
        uint32_t _weaponFrame;
    };
};
