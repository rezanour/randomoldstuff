#include <GDK.h>
#include <MemoryObject.h>
#include "Quake2Game.h"
#include "Hud.h"
#include "Items.h"

namespace Quake2
{
    struct uint32Pair_t
    {
        uint32_t x;
        uint32_t y;
    };
    // The pixel coordinates specified here assume a 640x480 layout.

    std::shared_ptr<Hud> Hud::Create(const GDK::DeviceContext& deviceContext)
    {
        return std::shared_ptr<Hud>(GDKNEW Hud(deviceContext));
    }

    Hud::Hud(const GDK::DeviceContext& deviceContext) :
        _visible(false), _ammoType(Item::Type::Shells), _armorType(ArmorType::Jacket), _weaponType(Item::Type::Shotgun), _ammo(0), _health(0), _armor(0), _deviceContext(deviceContext), _weaponFrame(0)
    {
        _numberTextures.push_back(L"pics/num_0.pcx.texture");
        _numberTextures.push_back(L"pics/num_1.pcx.texture");
        _numberTextures.push_back(L"pics/num_2.pcx.texture");
        _numberTextures.push_back(L"pics/num_3.pcx.texture");
        _numberTextures.push_back(L"pics/num_4.pcx.texture");
        _numberTextures.push_back(L"pics/num_5.pcx.texture");
        _numberTextures.push_back(L"pics/num_6.pcx.texture");
        _numberTextures.push_back(L"pics/num_7.pcx.texture");
        _numberTextures.push_back(L"pics/num_8.pcx.texture");
        _numberTextures.push_back(L"pics/num_9.pcx.texture");
        _numberTextures.push_back(L"pics/num_minus.pcx.texture");

        _armorTextures.push_back(L"pics/i_bodyarmor.pcx.texture");
        _armorTextures.push_back(L"pics/i_combatarmor.pcx.texture");
        _armorTextures.push_back(L"pics/i_jacketarmor.pcx.texture");

        _miscTextures.push_back(L"pics/i_health.pcx.texture");    // health kit icon
        _miscTextures.push_back(L"pics/ch1.pcx.texture");         // weapon site

        _vweaponTextures[Item::Type::Blaster] = L"models/weapons/v_blast/skin.pcx.texture";
        _vweaponGeometry[Item::Type::Blaster] = L"models/weapons/v_blast/tris.md2.geometry";
        _vweaponTextures[Item::Type::Shotgun] = L"models/weapons/v_shotg/skin.pcx.texture";
        _vweaponGeometry[Item::Type::Shotgun] = L"models/weapons/v_shotg/tris.md2.geometry";
        _vweaponTextures[Item::Type::SuperShotgun] = L"models/weapons/v_shotg2/skin.pcx.texture";
        _vweaponGeometry[Item::Type::SuperShotgun] = L"models/weapons/v_shotg2/tris.md2.geometry";
        _vweaponTextures[Item::Type::Machinegun] = L"models/weapons/v_machn/skin.pcx.texture";
        _vweaponGeometry[Item::Type::Machinegun] = L"models/weapons/v_machn/tris.md2.geometry";
        _vweaponTextures[Item::Type::Chaingun] = L"models/weapons/v_chain/skin.pcx.texture";
        _vweaponGeometry[Item::Type::Chaingun] = L"models/weapons/v_chain/tris.md2.geometry";
        _vweaponTextures[Item::Type::GrenadeLauncher] = L"models/weapons/v_launch/skin.pcx.texture";
        _vweaponGeometry[Item::Type::GrenadeLauncher] = L"models/weapons/v_launch/tris.md2.geometry";
        _vweaponTextures[Item::Type::RocketLauncher] = L"models/weapons/v_rocket/skin.pcx.texture";
        _vweaponGeometry[Item::Type::RocketLauncher] = L"models/weapons/v_rocket/tris.md2.geometry";
        _vweaponTextures[Item::Type::HyperBlaster] = L"models/weapons/v_hyperb/skin.pcx.texture";
        _vweaponGeometry[Item::Type::HyperBlaster] = L"models/weapons/v_hyperb/tris.md2.geometry";
        _vweaponTextures[Item::Type::Railgun] = L"models/weapons/v_rail/skin.pcx.texture";
        _vweaponGeometry[Item::Type::Railgun] = L"models/weapons/v_rail/tris.md2.geometry";
        _vweaponTextures[Item::Type::BFG] = L"models/weapons/v_bfg/skin.pcx.texture";
        _vweaponGeometry[Item::Type::BFG] = L"models/weapons/v_bfg/tris.md2.geometry";

        _weaponWorldPosition = GDK::Matrix::Identity();
    }

    void Hud::SetOwnerWorldMatrix(_In_ const GDK::Matrix& world)
    {
        _weaponWorldPosition = world;
        _weaponWorldPosition.SetTranslation(world.GetTranslation() + world.GetForward() * 10);

        // TODO: calculate 3D weapon offset for rendering using the owner's 3D position
    }

    void Hud::SetAmmoType(_In_ Item::Type type)
    {
        _ammoType = type;
    }

    void Hud::SetAmmo(_In_ byte_t ammo)
    {
        _ammo = ammo;
    }

    void Hud::SetArmorType(_In_ ArmorType type)
    {
        _armorType = type;
    }

    void Hud::SetArmor(_In_ byte_t armor)
    {
        _armor = armor;
    }

    void Hud::SetWeapon(_In_ Item::Type type)
    {
        _weaponType = type;

        _weaponFrame = 0;
        auto weaponGeometry = GDK::Content::LoadGeometryContent(_vweaponGeometry[_weaponType]);
        auto numAnimations = weaponGeometry->GetNumAnimations();
        auto animations = weaponGeometry->GetAnimations();
        for (uint32_t i = 0; i < numAnimations; ++i)
        {
            if (_strnicmp(animations[i].Name, "pow", 3) == 0)
            {
                _weaponFrame = animations[i].StartFrame;
                break;
            }
        }
    }

    void Hud::SetHealth(_In_ byte_t health)
    {
        _health = health;
    }

    bool Hud::Update(_In_ const GDK::GameTime&)
    {
        return true;
    }

    void Hud::Render()
    {
        _deviceContext.graphicsDevice->Set2DWorkArea(640, 480);

        _deviceContext.graphicsDevice->ClearDepth(1.0f);

        std::shared_ptr<GDK::ContentCache> _content =  Quake2Game::GetContentCache();

        // Draw 3D weapon
        _deviceContext.graphicsDevice->EnableZBuffer(true);
        _deviceContext.graphicsDevice->BindTexture(0, _content->GetTexture(_vweaponTextures[_weaponType]));
        _deviceContext.graphicsDevice->BindGeometry(_content->GetGeometry(_vweaponGeometry[_weaponType]));
        _deviceContext.graphicsDevice->Draw(_weaponWorldPosition, _weaponFrame);
        _deviceContext.graphicsDevice->EnableZBuffer(false);

        auto& weaponInfo = Items::GetWeaponInfo(_weaponType);
        auto& ammoInfo = Items::GetAmmoInfo(_ammoType);

        // Draw ammo
        DrawNumber(295,456, _ammo);
        _deviceContext.graphicsDevice->Draw2D(_content->GetTexture(ammoInfo.hudImage), 310, 456);

        // Draw health
        DrawNumber(194,456, _health);
        _deviceContext.graphicsDevice->Draw2D(_content->GetTexture(_miscTextures[0]), 210, 456);

        // Draw armor
        DrawNumber(394,456, _armor);
        _deviceContext.graphicsDevice->Draw2D(_content->GetTexture(_armorTextures[(int)_armorType]), 410, 456);

        // Draw weapon image
        _deviceContext.graphicsDevice->Draw2D(_content->GetTexture(weaponInfo.hudImage), 456, 456);

        // Draw site
        _deviceContext.graphicsDevice->Draw2D(_content->GetTexture(_miscTextures[1]), 316, 236);
    }

    void Hud::DrawNumber(_In_ int x, _In_ int y, _In_ int number)
    {
        byte_t digitArray[8] = {};
        NumberToDigitArray(number, digitArray, _countof(digitArray));
        DrawDigits(x,y, digitArray, _countof(digitArray));
    }

    bool Hud::IsVisible()
    {
        return _visible;
    }

    void Hud::Show(bool show)
    {
        _visible = show;
    }

    void Hud::DrawDigits(_In_ int x, _In_ int y, _In_ byte_t* digits, _In_ int count)
    {
        std::shared_ptr<GDK::ContentCache> _content =  Quake2Game::GetContentCache();

        byte_t i = 0;
        int left = x;
        int top = y;

        for (i = 0; i < count; i++)
        {
            if (digits[i] == 255)
            {
                break;
            }

            _deviceContext.graphicsDevice->Draw2D(_content->GetTexture(_numberTextures[digits[i]]), left, top);
            left -= _content->GetTexture(_numberTextures[digits[i]])->GetWidth();
        }
    }

    _Use_decl_annotations_
    void Hud::NumberToDigitArray(int value, byte_t* digits, int count)
    {
        int temp = value;
        int i = 0;
        int totalDigits = 0;

        memset(digits, 255, count);

        if (value == 0)
        {
            digits[0] = 0;
            return;
        }

        for(;temp!=0;temp=temp/10) 
        {
            totalDigits++;
        }

        if (totalDigits > count)
        {
            return;
        }

        temp = value;
        while (temp!=0)
        {
            digits[i] = temp % 10;
            temp /= 10;
            i++;
        }
    }

};