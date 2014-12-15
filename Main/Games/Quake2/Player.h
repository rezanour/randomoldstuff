#pragma once

#include <GDK.h>
#include <GameObject.h>

#include "Items.h"
#include "Hud.h"
#include "QuakeProperties.h"

namespace Quake2
{
    class Player : 
        public GDK::RuntimeObject<Player>,
        public GDK::IGameObjectController
    {
    public:
        // Creation
        static GDK::GameObjectCreateParameters Create(_In_ const std::shared_ptr<GDK::IGameWorld>& gameWorld, _In_ const std::shared_ptr<QuakeProperties>& quakeProps);

        // IGameObjectController
        virtual uint32_t GetTypeID() const override;
        virtual void OnCreate(_In_ const std::weak_ptr<GDK::IGameObject>& gameObject) override;
        virtual void OnDestroy() override;
        virtual void OnUpdate() override;
        virtual void OnActivate() override;
        virtual void AppendProperties(_Inout_ std::map<std::wstring, std::wstring>& properties) const override;

        // Player
        void SetHud(_In_ const std::shared_ptr<Hud>& hud);
        void EnableInput(_In_ bool enable);
        float GetAim() const { return _aim; }
        float GetHeight() const;

    private:
        Player();

        std::weak_ptr<GDK::IGameObject> _gameObject;

        // Ammo management
        byte_t GetAmmo(_In_ Item::Type type);
        void AddAmmo(_In_ Item::Type type, _In_ byte_t amount);
        bool HasEnoughAmmo(_In_ Item::Type, _In_ byte_t neededAmount);
        void ReduceAmmo(_In_ Item::Type type, _In_ byte_t amount);

        // Weapon management
        bool AddWeapon(_In_ Item::Type type);
        bool HasWeapon(_In_ Item::Type type);
        void SwitchWeapon(_In_ Item::Type type);

        // Health management
        bool AddHealth(_In_ uint16_t health);
        bool ReduceHealth(_In_ uint16_t health);

        std::shared_ptr<Hud> _hud; // set to null to detach hud from this player
        std::map<Item::Type, byte_t> _ammos;
        std::map<Item::Type, bool> _hasWeapon;
        Item::Type _currentWeapon;
        ArmorType _armorType;
        byte_t _health;
        byte_t _armor;

        bool _inputEnabled;

        // Movement
        float _aim;
        bool _onGround;
        void UpdateMovement(float elapsedGameTime);
    };
}
