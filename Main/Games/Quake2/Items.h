#pragma once

#include <GDK.h>
#include <GameObject.h>
#include "QuakeProperties.h"

namespace Quake2
{
    class Item : 
        public GDK::RuntimeObject<Item>,
        public GDK::IGameObjectController
    {
    public:
        enum class Type
        {
            None,

            // Health
            StimPack,
            MediumHealth,
            LargeHealth,
            MegaHealth,
            
            // Ammo
            Bullets,
            Shells,
            Cells,
            Grenades,
            Rockets,
            Slugs,

            // Weapons
            HandGrenade,
            Blaster,
            Shotgun,
            SuperShotgun,
            Machinegun,
            Chaingun,
            GrenadeLauncher,
            RocketLauncher,
            HyperBlaster,
            Railgun,
            BFG,

            // Armor
            BodyArmor,
            CombatArmor,
            JacketArmor,
            ScreenArmor,
            ShardArmor,
            ShieldArmor,

            // Pickups
            QuadDamage,
            Invulnerability,
            Silencer,
            Breather,
            EnvironmentSuit,
            AncientHead,
            Adrenaline,
            Bandolier,
            AmmoPack,

            // Keys
            CDKey,
            PowerCubeKey,
            PyramidKey,
            DataSpinnerKey,
            PassKey,
            BlueKey,
            RedKey,
            CommandHeadKey,
        };

        // Creation
        static GDK::GameObjectCreateParameters Create(_In_ const std::shared_ptr<GDK::IGameWorld>& gameWorld, _In_ const std::shared_ptr<QuakeProperties>& quakeProps);
        static GDK::GameObjectCreateParameters CreatePlaceholderItem(_In_ const std::shared_ptr<GDK::IGameWorld>& gameWorld, _In_ const std::shared_ptr<QuakeProperties>& quakeProps);

        // IGameObjectController
        virtual uint32_t GetTypeID() const override;
        virtual void OnCreate(_In_ const std::weak_ptr<GDK::IGameObject>& gameObject) override;
        virtual void OnDestroy() override;
        virtual void OnUpdate() override;
        virtual void OnActivate() override;
        virtual void AppendProperties(_Inout_ std::map<std::wstring, std::wstring>& properties) const override;

        Item::Type GetItemType() const;
        uint16_t GetValue() const;

    private:
        Item(_In_ Type type, _In_ uint16_t value);

        std::weak_ptr<GDK::IGameObject> _gameObject;
        Type _type;
        uint16_t _value;
        std::wstring _className;
    };

    struct AmmoInfo
    {
        AmmoInfo() {} // needed to play nice in map
        AmmoInfo(_In_ const std::wstring& name, _In_ Item::Type ammoType, _In_ byte_t ammoAmount, _In_ byte_t maxAmmo, _In_ const std::wstring& pickupObject, _In_ const std::wstring& hudImage) :
            name(name), ammoType(ammoType), ammoAmount(ammoAmount), maxAmmo(maxAmmo), pickupObject(pickupObject), hudImage(hudImage)
        {}

        std::wstring name;
        Item::Type ammoType;

        byte_t maxAmmo;
        byte_t ammoAmount;

        std::wstring pickupObject; // resource name of the .object representing the pickup in-game
        std::wstring hudImage; // resource name of the .texture for the HUD
    };

    struct WeaponInfo
    {
        WeaponInfo() {} // needed to play nice in map
        WeaponInfo(_In_ const std::wstring& name, _In_ Item::Type weaponType, _In_ Item::Type ammoType, _In_ byte_t ammoPerShot, _In_ const std::wstring& pickupObject, _In_ const std::wstring& hudImage, _In_ const std::wstring& pickupSound) :
            name(name), weaponType(weaponType), ammoType(ammoType), ammoPerShot(ammoPerShot), pickupObject(pickupObject), hudImage(hudImage), pickupSound(pickupSound)
        {}

        std::wstring name;
        Item::Type weaponType;

        Item::Type ammoType;
        byte_t ammoPerShot;

        std::wstring pickupObject; // resource name of the .object representing the pickup in-game
        std::wstring hudImage; // resource name of the .texture for the HUD
        std::wstring pickupSound;
    };

    namespace Items
    {
        const AmmoInfo& GetAmmoInfo(_In_ Item::Type type);
        const WeaponInfo& GetWeaponInfo(_In_ Item::Type type);
    }
}
