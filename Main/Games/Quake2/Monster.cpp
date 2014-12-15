#include "Monster.h"
#include "Items.h"
#include "Quake2Game.h"
#include "QuakeProperties.h"
#include <GameObject.h>
#include <GameWorld.h>

using namespace GDK;

namespace Quake2
{
    struct MonsterInfo
    {
        MonsterInfo(_In_ Monster::Type type, _In_ uint16_t health, _In_ const std::wstring& geometry, _In_ const std::wstring& texture, _In_ const std::wstring& hurtTexture) :
            type(type), health(health), geometry(geometry), texture(texture), hurtTexture(hurtTexture)
        {
        }

        Monster::Type type;
        uint16_t health;
        std::wstring geometry;
        std::wstring texture;
        std::wstring hurtTexture;
    };

    static std::map<std::wstring, std::shared_ptr<MonsterInfo>> g_monsterTypes;

    static void EnsureMonsterTypes()
    {
        if (g_monsterTypes.empty())
        {
            g_monsterTypes[L"monster_berserk"] =            std::make_shared<MonsterInfo>(Monster::Type::Berserk,       100,    L"models\\monsters\\berserk\\tris.md2.geometry",        L"models\\monsters\\berserk\\skin.pcx.texture",      L"models\\monsters\\berserk\\pain.pcx.texture");
            g_monsterTypes[L"monster_chick"] =              std::make_shared<MonsterInfo>(Monster::Type::Bitch,         100,    L"models\\monsters\\bitch\\tris.md2.geometry",          L"models\\monsters\\bitch\\skin.pcx.texture",        L"models\\monsters\\bitch\\pain.pcx.texture");
            g_monsterTypes[L"monster_boss1"] =              std::make_shared<MonsterInfo>(Monster::Type::Boss1,         100,    L"models\\monsters\\boss1\\tris.md2.geometry",          L"models\\monsters\\boss1\\skin.pcx.texture",        L"models\\monsters\\boss1\\pain.pcx.texture");
            g_monsterTypes[L"monster_boss2"] =              std::make_shared<MonsterInfo>(Monster::Type::Boss2,         100,    L"models\\monsters\\boss2\\tris.md2.geometry",          L"models\\monsters\\boss2\\skin.pcx.texture",        L"models\\monsters\\boss2\\pain.pcx.texture");
            g_monsterTypes[L"monster_boss3_stand"] =        std::make_shared<MonsterInfo>(Monster::Type::Boss3Jorg,     100,    L"models\\monsters\\boss3\\jorg\\tris.md2.geometry",    L"models\\monsters\\boss3\\jorg\\skin.pcx.texture",  L"models\\monsters\\boss3\\jorg\\pain.pcx.texture");
            g_monsterTypes[L"monster_jorg"] =               std::make_shared<MonsterInfo>(Monster::Type::Boss3Jorg,     100,    L"models\\monsters\\boss3\\jorg\\tris.md2.geometry",    L"models\\monsters\\boss3\\jorg\\skin.pcx.texture",  L"models\\monsters\\boss3\\jorg\\pain.pcx.texture");
            g_monsterTypes[L"monster_brain"] =              std::make_shared<MonsterInfo>( Monster::Type::Brain,        100,    L"models\\monsters\\brain\\tris.md2.geometry",          L"models\\monsters\\brain\\skin.pcx.texture",        L"models\\monsters\\brain\\pain.pcx.texture");
            g_monsterTypes[L"monster_commander_body"] =     std::make_shared<MonsterInfo>(Monster::Type::Commander,     100,    L"models\\monsters\\commandr\\tris.md2.geometry",       L"models\\monsters\\commandr\\skin.pcx.texture",     L"models\\monsters\\commandr\\pain.pcx.texture");
            g_monsterTypes[L"monster_flipper"] =            std::make_shared<MonsterInfo>(Monster::Type::Flipper,       100,    L"models\\monsters\\flipper\\tris.md2.geometry",        L"models\\monsters\\flipper\\skin.pcx.texture",      L"models\\monsters\\flipper\\pain.pcx.texture");
            g_monsterTypes[L"monster_floater"] =            std::make_shared<MonsterInfo>(Monster::Type::Float,         100,    L"models\\monsters\\float\\tris.md2.geometry",          L"models\\monsters\\float\\skin.pcx.texture",        L"models\\monsters\\float\\pain.pcx.texture");
            g_monsterTypes[L"monster_flyer"] =              std::make_shared<MonsterInfo>(Monster::Type::Flyer,         100,    L"models\\monsters\\flyer\\tris.md2.geometry",          L"models\\monsters\\flyer\\skin.pcx.texture",        L"models\\monsters\\flyer\\pain.pcx.texture");
            g_monsterTypes[L"monster_gladiatr"] =           std::make_shared<MonsterInfo>(Monster::Type::Gladiator,     100,    L"models\\monsters\\gladiatr\\tris.md2.geometry",       L"models\\monsters\\gladiatr\\skin.pcx.texture",     L"models\\monsters\\gladiatr\\pain.pcx.texture");
            g_monsterTypes[L"monster_gunner"] =             std::make_shared<MonsterInfo>(Monster::Type::Gunner,        100,    L"models\\monsters\\gunner\\tris.md2.geometry",         L"models\\monsters\\gunner\\skin.pcx.texture",       L"models\\monsters\\gunner\\pain.pcx.texture");
            g_monsterTypes[L"monster_hover"] =              std::make_shared<MonsterInfo>(Monster::Type::Hover,         100,    L"models\\monsters\\hover\\tris.md2.geometry",          L"models\\monsters\\hover\\skin.pcx.texture",        L"models\\monsters\\hover\\pain.pcx.texture");
            g_monsterTypes[L"monster_infantry"] =           std::make_shared<MonsterInfo>(Monster::Type::Infantry,      100,    L"models\\monsters\\infantry\\tris.md2.geometry",       L"models\\monsters\\infantry\\skin.pcx.texture",     L"models\\monsters\\infantry\\pain.pcx.texture");
            g_monsterTypes[L"monster_medic"] =              std::make_shared<MonsterInfo>( Monster::Type::Medic,        100,    L"models\\monsters\\medic\\tris.md2.geometry",          L"models\\monsters\\medic\\skin.pcx.texture",        L"models\\monsters\\medic\\pain.pcx.texture");
            g_monsterTypes[L"monster_mutant"] =             std::make_shared<MonsterInfo>(Monster::Type::Mutant,        100,    L"models\\monsters\\mutant\\tris.md2.geometry",         L"models\\monsters\\mutant\\skin.pcx.texture",       L"models\\monsters\\mutant\\pain.pcx.texture");
            g_monsterTypes[L"monster_parasite"] =           std::make_shared<MonsterInfo>(Monster::Type::Parasite,      100,    L"models\\monsters\\parasite\\tris.md2.geometry",       L"models\\monsters\\parasite\\skin.pcx.texture",     L"models\\monsters\\parasite\\pain.pcx.texture");
            g_monsterTypes[L"monster_soldier"] =            std::make_shared<MonsterInfo>(Monster::Type::Soldier,       100,    L"models\\monsters\\soldier\\tris.md2.geometry",        L"models\\monsters\\soldier\\skin.pcx.texture",      L"models\\monsters\\soldier\\pain.pcx.texture");
            g_monsterTypes[L"monster_soldier_light"] =      std::make_shared<MonsterInfo>(Monster::Type::SoldierLt,     100,    L"models\\monsters\\soldier\\tris.md2.geometry",        L"models\\monsters\\soldier\\skin_lt.pcx.texture",   L"models\\monsters\\soldier\\skin_ltp.pcx.texture");
            g_monsterTypes[L"monster_soldier_ss"] =         std::make_shared<MonsterInfo>(Monster::Type::SoldierSS,     100,    L"models\\monsters\\soldier\\tris.md2.geometry",        L"models\\monsters\\soldier\\skin_ss.pcx.texture",   L"models\\monsters\\soldier\\skin_ssp.pcx.texture");
            g_monsterTypes[L"monster_tank"] =               std::make_shared<MonsterInfo>(Monster::Type::Tank,          100,    L"models\\monsters\\tank\\tris.md2.geometry",           L"models\\monsters\\tank\\skin.pcx.texture",         L"models\\monsters\\tank\\pain.pcx.texture");
            g_monsterTypes[L"monster_tank_commander"] =     std::make_shared<MonsterInfo>(Monster::Type::Tank,          100,    L"models\\monsters\\tank\\tris.md2.geometry",           L"models\\monsters\\ctank\\skin.pcx.texture",        L"models\\monsters\\ctank\\pain.pcx.texture");
            g_monsterTypes[L"monster_super_tank"] =         std::make_shared<MonsterInfo>(Monster::Type::Tank,          100,    L"models\\monsters\\tank\\tris.md2.geometry",           L"models\\monsters\\ctank\\skin.pcx.texture",        L"models\\monsters\\ctank\\pain.pcx.texture");
            g_monsterTypes[L"monster_insaneguy1"] =         std::make_shared<MonsterInfo>(Monster::Type::InsaneGuy1,    100,    L"models\\monsters\\insane\\tris.md2.geometry",         L"models\\monsters\\insane\\i_skin.pcx.texture",     L"models\\monsters\\insane\\i_skin.pcx.texture");
            g_monsterTypes[L"monster_insaneguy2"] =         std::make_shared<MonsterInfo>(Monster::Type::InsaneGuy2,    100,    L"models\\monsters\\insane\\tris.md2.geometry",         L"models\\monsters\\insane\\i_skin02.pcx.texture",   L"models\\monsters\\insane\\i_skin02.pcx.texture");
            g_monsterTypes[L"monster_insaneguy3"] =         std::make_shared<MonsterInfo>(Monster::Type::InsaneGuy3,    100,    L"models\\monsters\\insane\\tris.md2.geometry",         L"models\\monsters\\insane\\i_skin03.pcx.texture",   L"models\\monsters\\insane\\i_skin03.pcx.texture");
        }
    }

    GameObjectCreateParameters Monster::Create(_In_ const std::shared_ptr<IGameWorld>& gameWorld, _In_ const std::shared_ptr<QuakeProperties>& quakeProps)
    {
        UNREFERENCED_PARAMETER(gameWorld);

        EnsureMonsterTypes();

        auto className = quakeProps->GetClassName();

        auto entry = g_monsterTypes.find(className);
        if (entry == g_monsterTypes.end())
        {
            //throw std::invalid_argument("unknown monster");
            return Item::CreatePlaceholderItem(gameWorld, quakeProps);
        }

        GameObjectCreateParameters params;
        params.className = className;
        params.visuals.push_back(VisualInfo(Matrix::Identity(), entry->second->geometry, entry->second->texture));
        params.physicsType = PhysicsBodyType::Normal;
        params.collisionPrimitive = Collision::AlignedCapsuleFromGeometry(Content::LoadGeometryContent(entry->second->geometry), 0, 0.0f);
        params.controller.reset(GDKNEW Monster(entry->second->type, entry->second->health));

        params.position = quakeProps->GetOrigin();
        params.rotation = quakeProps->GetAngle();

        return params;
    }

    Monster::Monster(_In_ Type type, _In_ uint16_t health) :
        _type(type), _health(health)
    {
    }

    uint32_t Monster::GetTypeID() const
    {
        return static_cast<uint32_t>(GameControllerType::Monster);
    }

    void Monster::OnCreate(_In_ const std::weak_ptr<IGameObject>& gameObject)
    {
        _gameObject = gameObject;
    }

    void Monster::OnDestroy()
    {
    }

    void Monster::OnUpdate()
    {
    }

    void Monster::OnActivate()
    {

    }

    void Monster::AppendProperties(_Inout_ std::map<std::wstring, std::wstring>& properties) const
    {
        UNREFERENCED_PARAMETER(properties);
    }
}
