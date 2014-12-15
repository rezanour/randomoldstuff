#include "Player.h"
#include "Quake2Game.h"
#include "QuakeProperties.h"
#include <Transform.h>
#include <GameWorld.h>

using namespace GDK;

namespace Quake2
{
    static float s_playerHeight = 0;

    GameObjectCreateParameters Player::Create(_In_ const std::shared_ptr<IGameWorld>& gameWorld, _In_ const std::shared_ptr<QuakeProperties>& quakeProps)
    {
        GameObjectCreateParameters params;

        params.className = L"player";
        params.position = quakeProps->GetOrigin();
        params.rotation = quakeProps->GetAngle();

        if (gameWorld->IsEditing())
        {
            params.visuals.push_back(VisualInfo(
                Matrix::Identity(), 
                L"models\\monsters\\insane\\tris.md2.geometry", 
                L"models\\monsters\\insane\\i_skin.pcx.texture"));
        }

        auto geometry = Content::LoadGeometryContent(L"models\\monsters\\insane\\tris.md2.geometry");
        auto shape = Collision::AlignedCapsuleFromGeometry(geometry, 0, 10.0f);
        AlignedCapsule* capsule = static_cast<AlignedCapsule*>(shape.get());
        s_playerHeight = capsule->length + 2 * capsule->radius;

        params.physicsType = PhysicsBodyType::Normal;
        params.collisionPrimitive = shape;
        params.controller.reset(GDKNEW Player);

        return params;
    }

    Player::Player() :
        _currentWeapon(Item::Type::Blaster), _armorType(ArmorType::Body), 
        _armor(0), _health(100), _inputEnabled(true), _aim(0.0f), _onGround(false)
    {
        _hasWeapon[_currentWeapon] = true;
    }

    uint32_t Player::GetTypeID() const
    {
        return static_cast<uint32_t>(GameControllerType::Player);
    }

    void Player::OnCreate(_In_ const std::weak_ptr<IGameObject>& gameObject)
    {
        _gameObject = gameObject;
    }

    void Player::OnDestroy()
    {
    }

    void Player::OnActivate()
    {

    }

    void Player::OnUpdate()
    {
        static struct KeyToWeaponMapping
        {
            QuakeKey key;
            Item::Type weapon;
        } Mappings[] =
        {
            { QuakeKey::SelectBlaster, Item::Type::Blaster },
            { QuakeKey::SelectRailgun, Item::Type::Railgun },
            { QuakeKey::SelectBFG, Item::Type::BFG },
            { QuakeKey::SelectChaingun, Item::Type::Chaingun },
            { QuakeKey::SelectGrenadeLauncher, Item::Type::GrenadeLauncher },
            { QuakeKey::SelectRocketLauncher, Item::Type::RocketLauncher },
            { QuakeKey::SelectMachinegun, Item::Type::Machinegun },
            { QuakeKey::SelectShotgun, Item::Type::Shotgun },
            { QuakeKey::SelectSuperShotgun, Item::Type::SuperShotgun },
            { QuakeKey::SelectHyperBlaster, Item::Type::HyperBlaster },
        };

        auto gameObject = _gameObject.lock();
        auto gameWorld = gameObject->GetGameWorld();

        //
        // Process contacts
        //
        _onGround = false;
        auto& contacts = gameObject->GetContacts();
        for (auto i = contacts.begin(); i != contacts.end(); ++i)
        {
            if (!i->other)
            {
                // static geometry. If normal is "close enough" to upward, count as on ground
                if (Vector3::Dot(i->normal, Vector3::Up()) > 0.707f)
                {
                    _onGround = true;
                }
            }
            else
            {
                auto controller = i->other->GetController();
                switch (static_cast<GameControllerType>(controller->GetTypeID()))
                {
                case GameControllerType::Item:
                    {
                        Item* theItem = static_cast<Item*>(controller.get());
                        switch(theItem->GetItemType())
                        {
                        case Item::Type::StimPack:
                        case Item::Type::MediumHealth:
                        case Item::Type::LargeHealth:
                        case Item::Type::MegaHealth:
                            if (AddHealth(theItem->GetValue()))
                            {
                                gameWorld->RemoveObject(i->other);
                                gameWorld->GetDeviceContext().audioDevice->PlayClip(Quake2Game::GetContentCache()->GetAudioClip(L"sound\\items\\s_health.wav"));
                            }
                            break;
                        case Item::Type::HandGrenade:
                        case Item::Type::Blaster:
                        case Item::Type::Shotgun:
                        case Item::Type::SuperShotgun:
                        case Item::Type::Machinegun:
                        case Item::Type::Chaingun:
                        case Item::Type::GrenadeLauncher:
                        case Item::Type::RocketLauncher:
                        case Item::Type::HyperBlaster:
                        case Item::Type::Railgun:
                        case Item::Type::BFG:
                            if (AddWeapon(theItem->GetItemType()))
                            {
                                gameWorld->RemoveObject(i->other);
                                gameWorld->GetDeviceContext().audioDevice->PlayClip(Quake2Game::GetContentCache()->GetAudioClip(L"sound\\misc\\w_pkup.wav"));
                            }
                            break;
                        }
                    }
                    break;
                }
            }
        }

        if (_hud)
        {
            if (_inputEnabled)
            {
                for (uint32_t i = 0; i < _countof(Mappings); ++i)
                {
                    auto weapon = Mappings[i].weapon;
                    if (GDK::Input::WasButtonPressed(Mappings[i].key) && HasWeapon(weapon))
                    {
                        SwitchWeapon(weapon);
                    }
                }

                if (GDK::Input::WasButtonPressed(QuakeKey::Fire))
                {
                    auto& weaponInfo = Items::GetWeaponInfo(_currentWeapon);

                    if (HasEnoughAmmo(weaponInfo.ammoType, weaponInfo.ammoPerShot))
                    {
                        // Fire!
                        ReduceAmmo(weaponInfo.ammoType, weaponInfo.ammoPerShot);
                        _hud->SetAmmo(GetAmmo(weaponInfo.ammoType));
                    }
                }

                if (_onGround && GDK::Input::WasButtonPressed(QuakeKey::Jump))
                {
                    gameObject->AddImpulse(Vector3::Up() * 600.0f);
                }

                _hud->SetHealth(_health);

                UpdateMovement(gameObject->GetGameWorld()->GetTime().deltaTime);
            }
        }
    }

    void Player::AppendProperties(_Inout_ std::map<std::wstring, std::wstring>& properties) const
    {
        // fix the classname to make this a spawn location
        properties[L"classname"] = L"info_player_start";
    }

    void Player::SetHud(_In_ const std::shared_ptr<Hud>& hud)
    {
        _hud = hud;

        auto& weaponInfo = Items::GetWeaponInfo(_currentWeapon);

        _hud->SetWeapon(_currentWeapon);
        _hud->SetAmmoType(weaponInfo.ammoType);
        _hud->SetAmmo(GetAmmo(weaponInfo.ammoType));
        _hud->SetArmorType(_armorType);
        _hud->SetArmor(_armor);
        _hud->SetHealth(_health);
    }

    void Player::SwitchWeapon(_In_ Item::Type type)
    {
        auto& weaponInfo = Items::GetWeaponInfo(type);

        _currentWeapon = type;
        _hud->SetWeapon(type);
        _hud->SetAmmoType(weaponInfo.ammoType);
        _hud->SetAmmo(GetAmmo(weaponInfo.ammoType));
    }

    byte_t Player::GetAmmo(_In_ Item::Type type)
    {
        auto it = _ammos.find(type);
        return (it != _ammos.end()) ? it->second : 0;
    }

    void Player::AddAmmo(_In_ Item::Type type, _In_ byte_t amount)
    {
        // TODO: powerups that allow us to carry more ammo would affect the calculation below

        // Note: the casts to int16_t are to protect against possible overflow, especially if we allow much more than 100 or so ammo for this type
        auto total = std::min(static_cast<int16_t>(GetAmmo(type)) + amount, 255); // cap to 255, the max we can store in a byte
        _ammos[type] = std::min(static_cast<byte_t>(total), Items::GetAmmoInfo(type).maxAmmo); // cap to the actual max ammo allowed for this type
    }

    bool Player::HasEnoughAmmo(_In_ Item::Type type, _In_ byte_t neededAmount)
    {
        return GetAmmo(type) >= neededAmount;
    }

    void Player::ReduceAmmo(_In_ Item::Type type, _In_ byte_t amount)
    {
        auto total = std::max(static_cast<int16_t>(GetAmmo(type)) - amount, 0);
        _ammos[type] = static_cast<byte_t>(total);
    }

    bool Player::AddWeapon(_In_ Item::Type type)
    {
        bool autoSwitchWeapon = !_hasWeapon[type];

        WeaponInfo weaponInfo = Items::GetWeaponInfo(type);
        AmmoInfo ammoInfo = Items::GetAmmoInfo(weaponInfo.ammoType);
        uint16_t totalAmmo = _ammos[ammoInfo.ammoType] + ammoInfo.ammoAmount;
        if (_hasWeapon[type] && totalAmmo > ammoInfo.maxAmmo)
        {
            return false;
        }

        _hasWeapon[type] = true;
        AddAmmo(ammoInfo.ammoType, ammoInfo.ammoAmount);

        if (autoSwitchWeapon)
        {
            SwitchWeapon(type);
        }

        return true;
    }

    bool Player::HasWeapon(_In_ Item::Type type)
    {
        auto it = _hasWeapon.find(type);
        return (it != _hasWeapon.end() && it->second);
    }

    bool Player::AddHealth(_In_ uint16_t health)
    {
        if (_health == 255)
        {
            return false;
        }

        uint16_t newHealth = health + _health;
        if (newHealth > 255)
        {
            _health = 255;
        }
        else
        {
            _health = (byte_t)newHealth;
        }

        return true;
    }

    bool Player::ReduceHealth(_In_ uint16_t health)
    {
        int newHealth = _health - health;
        if (newHealth > 0)
        {
            _health = (byte_t)newHealth;
            return true;
        }

        _health = 0;

        return false;
    }

    void Player::EnableInput(_In_ bool enable)
    {
        _inputEnabled = enable;
    }

    void Player::UpdateMovement(float elapsedGameTime)
    {
        static const float RotateSpeed = 0.008f;
        static const float MoveSpeed = 250.0f;

        GDK::Vector2 mouseCurDelta = Input::GetMouseDelta();
        // protect against invalid/first frame huge deltas
        if (mouseCurDelta.Length() > 300)
        {
            mouseCurDelta = Vector2::Zero();
        }

        GDK::Vector2 rotationDelta(mouseCurDelta * RotateSpeed);

        GDK::Vector3 positionOffset;

        _aim += rotationDelta.y;

        auto gameObject = _gameObject.lock();

        float rotation = gameObject->GetTransform().GetRotation();
        rotation += rotationDelta.x;

        // Limit aim to straight up or straight down
        auto newAim = std::max(-GDK::Math::PiOver2, _aim);
        newAim = std::min(GDK::Math::PiOver2, newAim);

        _aim = newAim;

        Matrix worldMatrix = gameObject->GetTransform().GetWorld();
        auto forward = worldMatrix.GetForward();
        auto right = worldMatrix.GetRight();

        if (GDK::Input::IsButtonDown(QuakeKey::MoveLeft))
        {
            positionOffset -= right;
        }

        if (GDK::Input::IsButtonDown(QuakeKey::MoveRight))
        {
            positionOffset += right;
        }

        if (GDK::Input::IsButtonDown(QuakeKey::MoveForward))
        {
            positionOffset += forward;
        }

        if (GDK::Input::IsButtonDown(QuakeKey::MoveBackward))
        {
            positionOffset -= forward;
        }

        if (positionOffset.LengthSquared() > 0)
        {
            // normalize * scale movement uniformly to avoid "cheats" where moving diagonal is faster than straight :)
            positionOffset = Vector3::Normalize(positionOffset) * elapsedGameTime * MoveSpeed;
        }

        // Update Player's rotation and position
        gameObject->GetTransform().SetRotation(rotation);
        gameObject->GetTransform().SetPosition(gameObject->GetTransform().GetPosition() + positionOffset);
    }

    float Player::GetHeight() const
    {
        return s_playerHeight;
    }
}
