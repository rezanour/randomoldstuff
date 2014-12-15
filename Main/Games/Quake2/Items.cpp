#include "Items.h"
#include "Quake2Game.h"
#include "QuakeProperties.h"
#include <GameObject.h>

using namespace GDK;

namespace Quake2
{
    struct ItemInfo
    {
        ItemInfo(_In_ Item::Type type, _In_ uint16_t value, _In_ const std::wstring& geometry, _In_ const std::wstring& texture) :
            type(type), value(value), geometry(geometry), texture(texture)
        {
        }

        Item::Type type;
        uint16_t value;
        std::wstring geometry;
        std::wstring texture;
    };

    static std::map<std::wstring, std::shared_ptr<ItemInfo>> g_itemTypes;

    static void EnsureItems()
    {
        if (g_itemTypes.empty())
        {
            g_itemTypes[L"item_health"] =           std::make_shared<ItemInfo>(Item::Type::StimPack,        100,    L"models\\items\\healing\\stimpack\\tris.md2.geometry",     L"models\\items\\healing\\stimpack\\skin.pcx.texture");
            g_itemTypes[L"item_health_small"] =     std::make_shared<ItemInfo>(Item::Type::MediumHealth,    100,    L"models\\items\\healing\\medium\\tris.md2.geometry",       L"models\\items\\healing\\medium\\skin.pcx.texture");
            g_itemTypes[L"item_health_large"] =     std::make_shared<ItemInfo>(Item::Type::LargeHealth,     100,    L"models\\items\\healing\\large\\tris.md2.geometry",        L"models\\items\\healing\\large\\skin.pcx.texture");
            g_itemTypes[L"item_health_mega"] =      std::make_shared<ItemInfo>(Item::Type::MegaHealth,      100,    L"models\\items\\mega_h\\tris.md2.geometry",                L"models\\items\\mega_h\\skin.pcx.texture");

            g_itemTypes[L"weapon_shotgun"] =        std::make_shared<ItemInfo>(Item::Type::Shotgun,         100,    L"models\\weapons\\g_shotg\\tris.md2.geometry",             L"models\\weapons\\g_shotg\\skin.pcx.texture");
            g_itemTypes[L"weapon_supershotgun"] =   std::make_shared<ItemInfo>(Item::Type::SuperShotgun,    100,    L"models\\weapons\\g_shotg2\\tris.md2.geometry",            L"models\\weapons\\g_shotg2\\skin.pcx.texture");
            g_itemTypes[L"weapon_machinegun"] =     std::make_shared<ItemInfo>(Item::Type::Machinegun,      100,    L"models\\weapons\\g_machn\\tris.md2.geometry",             L"models\\weapons\\g_machn\\skin.pcx.texture");
            g_itemTypes[L"weapon_hyperblaster"] =   std::make_shared<ItemInfo>(Item::Type::HyperBlaster,    100,    L"models\\weapons\\g_hyperb\\tris.md2.geometry",            L"models\\weapons\\g_hyperb\\skin.pcx.texture");
            g_itemTypes[L"weapon_rocketlauncher"] = std::make_shared<ItemInfo>(Item::Type::RocketLauncher,  100,    L"models\\weapons\\g_rocket\\tris.md2.geometry",            L"models\\weapons\\g_rocket\\skin.pcx.texture");
            g_itemTypes[L"weapon_bfg"] =            std::make_shared<ItemInfo>(Item::Type::BFG,             100,    L"models\\weapons\\g_bfg\\tris.md2.geometry",               L"models\\weapons\\g_bfg\\skin.pcx.texture");

            g_itemTypes[L"item_armor_shard"] =      std::make_shared<ItemInfo>(Item::Type::ShardArmor,      100,    L"models\\items\\armor\\shard\\tris.md2.geometry",          L"models\\items\\armor\\shard\\skin.pcx.texture");
            g_itemTypes[L"item_armor_jacket"] =     std::make_shared<ItemInfo>(Item::Type::JacketArmor,     100,    L"models\\items\\armor\\jacket\\tris.md2.geometry",         L"models\\items\\armor\\jacket\\skin.pcx.texture");
            g_itemTypes[L"item_armor_combat"] =     std::make_shared<ItemInfo>(Item::Type::CombatArmor,     100,    L"models\\items\\armor\\combat\\tris.md2.geometry",         L"models\\items\\armor\\combat\\skin.pcx.texture");
            g_itemTypes[L"item_quad"] =             std::make_shared<ItemInfo>(Item::Type::QuadDamage,      100,    L"models\\items\\quaddama\\tris.md2.geometry",              L"models\\items\\quaddama\\skin.pcx.texture");
            g_itemTypes[L"item_adrenaline"] =       std::make_shared<ItemInfo>(Item::Type::Adrenaline,      100,    L"models\\items\\adrenal\\tris.md2.geometry",               L"models\\items\\adrenal\\skin.pcx.texture");
            g_itemTypes[L"item_silencer"] =         std::make_shared<ItemInfo>(Item::Type::Silencer,        100,    L"models\\items\\silencer\\tris.md2.geometry",              L"models\\items\\adrenal\\skin.pcx.texture");

            g_itemTypes[L"ammo_shells"] =           std::make_shared<ItemInfo>(Item::Type::Shells,          100,    L"models\\items\\ammo\\shells\\medium\\tris.md2.geometry",  L"models\\items\\ammo\\shells\\medium\\skin.pcx.texture");
            g_itemTypes[L"ammo_bullets"] =          std::make_shared<ItemInfo>(Item::Type::Bullets,         100,    L"models\\items\\ammo\\bullets\\medium\\tris.md2.geometry", L"models\\items\\ammo\\bullets\\medium\\skin.pcx.texture");
            g_itemTypes[L"ammo_slugs"] =            std::make_shared<ItemInfo>(Item::Type::Slugs,           100,    L"models\\items\\ammo\\slugs\\medium\\tris.md2.geometry",   L"models\\items\\ammo\\slugs\\medium\\skin.pcx.texture");
            g_itemTypes[L"ammo_rockets"] =          std::make_shared<ItemInfo>(Item::Type::Rockets,         100,    L"models\\items\\ammo\\rockets\\medium\\tris.md2.geometry", L"models\\items\\ammo\\rockets\\medium\\skin.pcx.texture");
            g_itemTypes[L"ammo_cells"] =            std::make_shared<ItemInfo>(Item::Type::Cells,           100,    L"models\\items\\ammo\\cells\\medium\\tris.md2.geometry",   L"models\\items\\ammo\\cells\\medium\\skin.pcx.texture");
            g_itemTypes[L"ammo_grenades"] =         std::make_shared<ItemInfo>(Item::Type::Grenades,        100,    L"models\\items\\ammo\\grenades\\medium\\tris.md2.geometry", L"models\\items\\ammo\\grenades\\medium\\skin.pcx.texture");
        }
    }

    GameObjectCreateParameters Item::Create(_In_ const std::shared_ptr<IGameWorld>& gameWorld, _In_ const std::shared_ptr<QuakeProperties>& quakeProps)
    {
        UNREFERENCED_PARAMETER(gameWorld);

        EnsureItems();

        auto className = quakeProps->GetClassName();
        auto entry = g_itemTypes.find(className);
        if (entry == g_itemTypes.end())
        {
            //throw std::invalid_argument("unknown item");
            return CreatePlaceholderItem(gameWorld, quakeProps);
        }

        GameObjectCreateParameters params;
        params.className = className;
        params.visuals.push_back(VisualInfo(Matrix::Identity(), entry->second->geometry, entry->second->texture));
        params.physicsType = PhysicsBodyType::Sensor;
        params.collisionPrimitive = Collision::AlignedCapsuleFromGeometry(Content::LoadGeometryContent(entry->second->geometry), 0, 0.0f);
        params.controller.reset(GDKNEW Item(entry->second->type, entry->second->value));

        params.position = quakeProps->GetOrigin();
        params.rotation = quakeProps->GetAngle();

        return params;
    }

    GameObjectCreateParameters Item::CreatePlaceholderItem(_In_ const std::shared_ptr<IGameWorld>& gameWorld, _In_ const std::shared_ptr<QuakeProperties>& quakeProps)
    {
        UNREFERENCED_PARAMETER(gameWorld);

        //EnsureItems();
        //auto entry = g_itemTypes.find(L"item_health_small");

        GameObjectCreateParameters params;
        params.className = quakeProps->GetClassName();
        //params.visuals.push_back(VisualInfo(Matrix::Identity(), entry->second->geometry, entry->second->texture));
        params.controller.reset(GDKNEW Item(Item::Type::BFG, 0));

        params.position = quakeProps->GetOrigin();
        params.rotation = quakeProps->GetAngle();
        params.targetName = quakeProps->GetTargetName();

        DebugOut("Creating Placeholder Item for - %S(%S)...",quakeProps->GetClassName().c_str(), quakeProps->GetTargetName().c_str());

        return params;
    }

    Item::Item(_In_ Type type, _In_ uint16_t value) :
        _type(type), _value(value)
    {
    }

    uint32_t Item::GetTypeID() const
    {
        return static_cast<uint32_t>(GameControllerType::Item);
    }

    void Item::OnCreate(_In_ const std::weak_ptr<IGameObject>& gameObject)
    {
        _gameObject = gameObject;
    }

    void Item::OnDestroy()
    {
    }

    void Item::OnUpdate()
    {
    }

    void Item::OnActivate()
    {
        DebugOut("ITEM (%S)- ACTIVATED", _className.c_str());
    }

    void Item::AppendProperties(_Inout_ std::map<std::wstring, std::wstring>& properties) const
    {
        UNREFERENCED_PARAMETER(properties);
    }

    Item::Type Item::GetItemType() const
    {
        return _type;
    }

    uint16_t Item::GetValue() const
    {
        return _value;
    }

    static AmmoInfo g_ammoInfos[] = 
    {
        // TODO: Add the rest
        AmmoInfo(L"Bullet",  Item::Type::Bullets,  50, 100, L"items\\bullet.object",   L"pics\\a_bullets.pcx.texture"),
        AmmoInfo(L"Shell",   Item::Type::Shells,   50, 100, L"items\\shells.object",   L"pics\\a_shells.pcx.texture"),
        AmmoInfo(L"Grenade", Item::Type::Grenades,  5,  10, L"items\\grenades.object", L"pics\\a_grenades.pcx.texture"),
        AmmoInfo(L"Rocket",  Item::Type::Rockets,   5, 100, L"items\\rockets.object",  L"pics\\a_rockets.pcx.texture"),
        AmmoInfo(L"Cell",    Item::Type::Cells,    50, 200, L"items\\cells.object",    L"pics\\a_cells.pcx.texture"),
        AmmoInfo(L"Slug",    Item::Type::Slugs,    10, 100, L"items\\slugs.object",    L"pics\\a_slugs.pcx.texture"),
    };
    static std::map<Item::Type, AmmoInfo> g_ammoLookup;

    static WeaponInfo g_weaponInfos[] = 
    {
        // TODO: Add the rest
        WeaponInfo(L"Hand Grenade",     Item::Type::HandGrenade,     Item::Type::Grenades, 1, L"weapons\\grenade.object",      L"pics\\w_hgrenade.pcx.texture",     L"sound\\misc\\w_pkup.wav"),
        WeaponInfo(L"Blaster",          Item::Type::Blaster,         Item::Type::Bullets,  1, L"weapons\\blast.object",        L"pics\\w_blaster.pcx.texture",      L"sound\\misc\\w_pkup.wav"),
        WeaponInfo(L"Shotgun",          Item::Type::Shotgun,         Item::Type::Shells,   1, L"weapons\\shotg.object",        L"pics\\w_shotgun.pcx.texture",      L"sound\\misc\\w_pkup.wav"),
        WeaponInfo(L"Super Shotgun",    Item::Type::SuperShotgun,    Item::Type::Shells,   2, L"weapons\\shotg2.object",       L"pics\\w_sshotgun.pcx.texture",     L"sound\\misc\\w_pkup.wav"),
        WeaponInfo(L"Machine Gun",      Item::Type::Machinegun,      Item::Type::Bullets,  3, L"weapons\\machinegun.object",   L"pics\\w_machinegun.pcx.texture",   L"sound\\misc\\w_pkup.wav"),
        WeaponInfo(L"Chaingun",         Item::Type::Chaingun,        Item::Type::Bullets,  5, L"weapons\\chaingun.object",     L"pics\\w_chaingun.pcx.texture",     L"sound\\misc\\w_pkup.wav"),
        WeaponInfo(L"Grenade Launcher", Item::Type::GrenadeLauncher, Item::Type::Grenades, 1, L"weapons\\glauncher.object",    L"pics\\w_glauncher.pcx.texture",    L"sound\\misc\\w_pkup.wav"),
        WeaponInfo(L"Rocket Launcher",  Item::Type::RocketLauncher,  Item::Type::Rockets,  1, L"weapons\\rlauncher.object",    L"pics\\w_rlauncher.pcx.texture",    L"sound\\misc\\w_pkup.wav"),
        WeaponInfo(L"Hyper Blaster",    Item::Type::HyperBlaster,    Item::Type::Cells,    1, L"weapons\\hyperblaster.object", L"pics\\w_hyperblaster.pcx.texture", L"sound\\misc\\w_pkup.wav"),
        WeaponInfo(L"Rail Gun",         Item::Type::Railgun,         Item::Type::Slugs,    2, L"weapons\\railgun.object",      L"pics\\w_railgun.pcx.texture",      L"sound\\misc\\w_pkup.wav"),
        WeaponInfo(L"BFG",              Item::Type::BFG,             Item::Type::Cells,   10, L"weapons\\bfg.object",          L"pics\\w_bfg.pcx.texture",          L"sound\\misc\\w_pkup.wav"),
    };
    static std::map<Item::Type, WeaponInfo> g_weaponLookup;

    namespace Items
    {
        static void EnsureLookupTables()
        {
            static bool initialized = false;

            if (!initialized)
            {
                for (uint32_t i = 0; i < _countof(g_ammoInfos); ++i)
                {
                    g_ammoLookup[g_ammoInfos[i].ammoType] = g_ammoInfos[i];
                }

                for (uint32_t i = 0; i < _countof(g_weaponInfos); ++i)
                {
                    g_weaponLookup[g_weaponInfos[i].weaponType] = g_weaponInfos[i];
                }

                initialized = true;
            }
        }

        const WeaponInfo& GetWeaponInfo(_In_ Item::Type type)
        {
            EnsureLookupTables();
            return g_weaponLookup[type];
        }

        const AmmoInfo& GetAmmoInfo(_In_ Item::Type type)
        {
            EnsureLookupTables();
            return g_ammoLookup[type];
        }
    }
}
