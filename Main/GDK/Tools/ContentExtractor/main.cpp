#include "stdafx.h"

struct QUAKE_CONTENT
{
    char* name;
    char* md2Model;
    char* skin1;
    char* skin2;
    wchar_t* gameContentPath;
    bool looseFiles;
};

//#define BUILD_LOOSE_FILES_ONLY

#ifdef BUILD_LOOSE_FILES_ONLY
QUAKE_CONTENT quake_content[] = 
{
    {"player","C:\\Users\\coopp\\Documents\\Visual Studio 2012\\Projects\\GDK\\Games\\Quake2\\content\\Pak\\files\\male\\tris.md2", "C:\\Users\\coopp\\Documents\\Visual Studio 2012\\Projects\\GDK\\Games\\Quake2\\content\\Pak\\files\\male\\skin.pcx", "C:\\Users\\coopp\\Documents\\Visual Studio 2012\\Projects\\GDK\\Games\\Quake2\\content\\Pak\\files\\male\\skin.pcx", L"\\player\\player.object\\", true},
    {"weapon","C:\\Users\\coopp\\Documents\\Visual Studio 2012\\Projects\\GDK\\Games\\Quake2\\content\\Pak\\files\\male\\weapon.md2", "C:\\Users\\coopp\\Documents\\Visual Studio 2012\\Projects\\GDK\\Games\\Quake2\\content\\Pak\\files\\male\\weapon.pcx", "C:\\Users\\coopp\\Documents\\Visual Studio 2012\\Projects\\GDK\\Games\\Quake2\\content\\Pak\\files\\male\\weapon.pcx", L"\\player\\weapon.object\\", true},
};
#else

QUAKE_CONTENT quake_content[] = 
{
    {"noname","models/items/ammo/bullets/medium/tris.md2"   ,"models/items/ammo/bullets/medium/skin.pcx",   "models/items/ammo/bullets/medium/skin.pcx",       L"\\items\\bullet.object\\", false}, 
    {"noname","models/items/ammo/cells/medium/tris.md2"     ,"models/items/ammo/cells/medium/skin.pcx",     "models/items/ammo/cells/medium/skin.pcx",         L"\\items\\cells.object\\", false},
    {"noname","models/items/ammo/grenades/medium/tris.md2"  ,"models/items/ammo/grenades/medium/skin.pcx",  "models/items/ammo/grenades/medium/skin.pcx",      L"\\items\\grenades.object\\", false},
    {"noname","models/items/ammo/mines/tris.md2"            ,"models/items/ammo/mines/skin.pcx",            "models/items/ammo/mines/skin.pcx",                L"\\items\\mines.object\\", false},
    {"noname","models/items/ammo/nuke/tris.md2"             ,"models/items/ammo/nuke/skin.pcx",             "models/items/ammo/nuke/skin.pcx",                 L"\\items\\nuke.object\\", false},
    {"noname","models/items/ammo/rockets/medium/tris.md2"   ,"models/items/ammo/rockets/medium/skin.pcx",   "models/items/ammo/rockets/medium/skin.pcx",       L"\\items\\rockets.object\\", false},
    {"noname","models/items/ammo/shells/medium/tris.md2"    ,"models/items/ammo/shells/medium/skin.pcx",    "models/items/ammo/shells/medium/skin.pcx",        L"\\items\\shells.object\\", false},
    {"noname","models/items/ammo/slugs/medium/tris.md2"     ,"models/items/ammo/slugs/medium/skin.pcx",     "models/items/ammo/slugs/medium/skin.pcx",         L"\\items\\slugs.object\\", false},
    {"noname","models/items/armor/body/tris.md2"            ,"models/items/armor/body/skin.pcx",            "models/items/armor/body/skin.pcx",                L"\\items\\bodyarmor.object\\", false},
    {"noname","models/items/armor/combat/tris.md2"          ,"models/items/armor/combat/skin.pcx",          "models/items/armor/combat/skin.pcx",              L"\\items\\combatarmor.object\\", false},
    {"noname","models/items/armor/jacket/tris.md2"          ,"models/items/armor/jacket/skin.pcx",          "models/items/armor/jacket/skin.pcx",              L"\\items\\jacketarmor.object\\", false},
    {"noname","models/items/armor/shard/tris.md2"           ,"models/items/armor/shard/skin.pcx",           "models/items/armor/shard/skin.pcx",               L"\\items\\shardarmor.object\\", false},
    {"noname","models/items/armor/screen/tris.md2"          ,"models/items/armor/screen/skin.pcx",          "models/items/armor/screen/skin.pcx",              L"\\items\\screenarmor.object\\", false},
    {"noname","models/items/armor/shield/tris.md2"          ,"models/items/armor/shield/skin.pcx",          "models/items/armor/shield/skin.pcx",              L"\\items\\shieldarmor.object\\", false},
    {"noname","models/items/armor/effect/tris.md2"          ,"models/items/armor/effect/skin.pcx",          "models/items/armor/effect/skin.pcx",              L"\\items\\effectarmor.object\\", false},
    {"noname","models/items/healing/medium/tris.md2"        ,"models/items/healing/medium/skin.pcx",        "models/items/healing/medium/skin.pcx",            L"\\items\\mediumhealth.object\\", false},
    {"noname","models/items/healing/stimpack/tris.md2"      ,"models/items/healing/stimpack/skin.pcx",      "models/items/healing/stimpack/skin.pcx",          L"\\items\\stimpackhealth.object\\", false},
    {"noname","models/items/healing/large/tris.md2"         ,"models/items/healing/large/skin.pcx",         "models/items/healing/large/skin.pcx",             L"\\items\\largehealth.object\\", false},
    {"noname","models/items/mega_h/tris.md2"                ,"models/items/mega_h/skin.pcx",                "models/items/mega_h/skin.pcx",                    L"\\items\\megahealth.object\\", false},
    {"noname","models/objects/gibs/arm/tris.md2"            ,"models/objects/gibs/arm/skin.pcx",            "models/objects/gibs/arm/skin.pcx",                L"\\objects\\arm.object\\", false},
    {"noname","models/objects/gibs/bone/tris.md2"           ,"models/objects/gibs/bone/skin.pcx",           "models/objects/gibs/bone/skin.pcx",               L"\\objects\\bone.object\\", false},
    {"noname","models/objects/gibs/bone2/tris.md2"          ,"models/objects/gibs/bone2/skin.pcx",          "models/objects/gibs/bone2/skin.pcx",              L"\\objects\\bone2.object\\", false},
    {"noname","models/objects/gibs/chest/tris.md2"          ,"models/objects/gibs/chest/skin.pcx",          "models/objects/gibs/chest/skin.pcx",              L"\\objects\\chest.object\\", false},
    {"noname","models/objects/gibs/gear/tris.md2"           ,"models/objects/gibs/gear/skin.pcx",           "models/objects/gibs/gear/skin.pcx",               L"\\objects\\gear.object\\", false},
    {"noname","models/objects/gibs/head/tris.md2"           ,"models/objects/gibs/head/skin.pcx",           "models/objects/gibs/head/skin.pcx",               L"\\objects\\head.object\\", false},
    {"noname","models/objects/gibs/head2/tris.md2"          ,"models/objects/gibs/head2/skin.pcx",          "models/objects/gibs/head2/skin.pcx",              L"\\objects\\head2.object\\", false},
    {"noname","models/objects/gibs/leg/tris.md2"            ,"models/objects/gibs/leg/skin.pcx",            "models/objects/gibs/leg/skin.pcx",                L"\\objects\\leg.object\\", false},
    {"noname","models/objects/gibs/skull/tris.md2"          ,"models/objects/gibs/skull/skin.pcx",          "models/objects/gibs/skull/skin.pcx",              L"\\objects\\skull.object\\", false},
    {"noname","models/objects/gibs/sm_meat/tris.md2"        ,"models/objects/gibs/sm_meat/skin.pcx",        "models/objects/gibs/sm_meat/skin.pcx",            L"\\objects\\meat.object\\", false},
    {"noname","models/objects/gibs/sm_metal/tris.md2"       ,"models/objects/gibs/sm_metal/skin.pcx",       "models/objects/gibs/sm_metal/skin.pcx",           L"\\objects\\metal.object\\", false},
    {"noname","models/objects/debris1/tris.md2"             ,"models/objects/debris1/skin.pcx",             "models/objects/debris1/skin.pcx",                 L"\\objects\\debris1.object\\", false},
    {"noname","models/objects/debris2/tris.md2"             ,"models/objects/debris2/skin.pcx",             "models/objects/debris2/skin.pcx",                 L"\\objects\\debris2.object\\", false},
    {"noname","models/objects/debris3/tris.md2"             ,"models/objects/debris3/skin.pcx",             "models/objects/debris3/skin.pcx",                 L"\\objects\\debris3.object\\", false},
    {"noname","models/objects/laser/tris.md2"               ,"models/objects/laser/skin.pcx",               "models/objects/laser/skin.pcx",                   L"\\objects\\laser.object\\", false},
    {"noname","models/objects/rocket/tris.md2"              ,"models/objects/rocket/skin.pcx",              "models/objects/rocket/skin.pcx",                  L"\\objects\\rocket.object\\", false},
    {"noname","models/objects/bomb/tris.md2"                ,"models/objects/bomb/skin.pcx",                "models/objects/bomb/skin.pcx",                    L"\\objects\\bomb.object\\", false},
    {"noname","models/objects/grenade/tris.md2"             ,"models/objects/grenade/skin.pcx",             "models/objects/grenade/skin.pcx",                 L"\\objects\\grenade.object\\", false},
    {"noname","models/objects/grenade2/tris.md2"            ,"models/objects/grenade2/skin.pcx",            "models/objects/grenade2/skin.pcx",                L"\\objects\\grenade2.object\\", false},
    {"noname","models/objects/flash/tris.md2"               ,"models/objects/flash/skin.pcx",               "models/objects/flash/skin.pcx",                   L"\\objects\\flash.object\\", false},
    {"noname","models/objects/smoke/tris.md2"               ,"models/objects/smoke/skin.pcx",               "models/objects/smoke/skin.pcx",                   L"\\objects\\smoke.object\\", false},
    {"noname","models/objects/explode/tris.md2"             ,"models/objects/explode/skin.pcx",             "models/objects/explode/skin.pcx",                 L"\\objects\\explode.object\\", false},
    {"noname","models/objects/r_explode/tris.md2"           ,"models/objects/r_explode/skin.pcx",           "models/objects/r_explode/skin.pcx",               L"\\objects\\r_explode.object\\", false},
    {"noname","models/objects/barrels/tris.md2"             ,"models/objects/barrels/skin.pcx",             "models/objects/barrels/skin.pcx",                 L"\\objects\\barrels.object\\", false},
    {"noname","models/objects/minelite/light1/tris.md2"     ,"models/objects/minelite/light1/skin.pcx",     "models/objects/minelite/light1/skin.pcx",         L"\\objects\\light1.object\\", false},
    {"noname","models/objects/minelite/light2/tris.md2"     ,"models/objects/minelite/light2/skin.pcx",     "models/objects/minelite/light2/skin.pcx",         L"\\objects\\light2.object\\", false},
    {"noname","models/deadbods/dude/tris.md2"               ,"models/deadbods/dude/skin.pcx",               "models/deadbods/dude/skin.pcx",                   L"\\objects\\deadbody.object\\", false},
    {"noname","models/items/c_head/tris.md2"                ,"models/items/c_head/skin.pcx",                "models/items/c_head/skin.pcx",                    L"\\items\\c_head.object\\", false},
    {"noname","models/objects/dmspot/tris.md2"              ,"models/objects/dmspot/skin.pcx",              "models/objects/dmspot/skin.pcx",                  L"\\objects\\dmspot.object\\", false},
    {"noname","models/items/quaddama/tris.md2"              ,"models/items/quaddama/skin.pcx",              "models/items/quaddama/skin.pcx",                  L"\\items\\quaddama.object\\", false},
    {"noname","models/items/invulner/tris.md2"              ,"models/items/invulner/skin.pcx",              "models/items/invulner/skin.pcx",                  L"\\items\\invulner.object\\", false},
    {"noname","models/items/silencer/tris.md2"              ,"models/items/silencer/skin.pcx",              "models/items/silencer/skin.pcx",                  L"\\items\\silencer.object\\", false},
    {"noname","models/items/breather/tris.md2"              ,"models/items/breather/skin.pcx",              "models/items/breather/skin.pcx",                  L"\\items\\breather.object\\", false},
    {"noname","models/items/enviro/tris.md2"                ,"models/items/enviro/skin.pcx",                "models/items/enviro/skin.pcx",                    L"\\items\\enviro.object\\", false},
    {"noname","models/items/band/tris.md2"                  ,"models/items/band/skin.pcx",                  "models/items/band/skin.pcx",                      L"\\items\\band.object\\", false},
    {"noname","models/items/pack/tris.md2"                  ,"models/items/pack/skin.pcx",                  "models/items/pack/skin.pcx",                      L"\\items\\pack.object\\", false},
    {"noname","models/items/adrenal/tris.md2"               ,"models/items/adrenal/skin.pcx",               "models/items/adrenal/skin.pcx",                   L"\\items\\adrenal.object\\", false},
    {"noname","models/ships/viper/tris.md2"                 ,"models/ships/viper/skin.pcx",                 "models/ships/viper/skin.pcx",                     L"\\ships\\viper.object\\", false},
    {"noname","models/ships/bigviper/tris.md2"              ,"models/ships/bigviper/skin.pcx",              "models/ships/bigviper/skin.pcx",                  L"\\ships\\bigviper.object\\", false},
    {"noname","models/ships/strogg1/tris.md2"               ,"models/ships/strogg1/skin.pcx",               "models/ships/strogg1/skin.pcx",                   L"\\ships\\strogg1.object\\", false},
    {"noname","models/items/keys/target/tris.md2"           ,"models/items/keys/target/skin.pcx",           "models/items/keys/target/skin.pcx",               L"\\items\\target.object\\", false},
    {"noname","models/objects/banner/tris.md2"              ,"models/objects/banner/skin.pcx",              "models/objects/banner/skin.pcx",                  L"\\objects\\banner.object\\", false},
    {"noname","models/objects/satellite/tris.md2"           ,"models/objects/satellite/skin.pcx",           "models/objects/satellite/skin.pcx",               L"\\objects\\satellite.object\\", false},
    {"noname","models/objects/black/tris.md2"               ,"models/objects/black/skin.pcx",               "models/objects/black/skin.pcx",                   L"\\objects\\black.object\\", false},
    {"noname","models/items/keys/data_cd/tris.md2"          ,"models/items/keys/data_cd/skin.pcx",          "models/items/keys/data_cd/skin.pcx",              L"\\items\\data_cd.object\\", false},
    {"noname","models/items/keys/power/tris.md2"            ,"models/items/keys/power/skin.pcx",            "models/items/keys/power/skin.pcx",                L"\\items\\power.object\\", false},
    {"noname","models/items/keys/pyramid/tris.md2"          ,"models/items/keys/pyramid/skin.pcx",          "models/items/keys/pyramid/skin.pcx",              L"\\items\\pyramid.object\\", false},
    {"noname","models/items/keys/spinner/tris.md2"          ,"models/items/keys/spinner/skin.pcx",          "models/items/keys/spinner/skin.pcx",              L"\\items\\spinner.object\\", false},
    {"noname","models/items/keys/pass/tris.md2"             ,"models/items/keys/pass/skin.pcx",             "models/items/keys/pass/skin.pcx",                 L"\\items\\pass.object\\", false},
    {"noname","models/items/keys/key/tris.md2"              ,"models/items/keys/key/skin.pcx",              "models/items/keys/key/skin.pcx",                  L"\\items\\key.object\\", false},
    //{"noname","models/items/keys/red_key/tris.md2"          ,"models/items/keys/red_key/skin.pcx",          "models/items/keys/red_key/skin.pcx",              L"\\items\\red_key.object\\", false},
    {"noname","models/monsters/berserk/tris.md2"            ,"models/monsters/berserk/skin.pcx",            "models/monsters/berserk/skin.pcx",                L"\\monsters\\berserk.object\\", false},
    {"noname","models/monsters/gladiatr/tris.md2"           ,"models/monsters/gladiatr/skin.pcx",           "models/monsters/gladiatr/skin.pcx",               L"\\monsters\\gladiatr.object\\", false},
    {"noname","models/monsters/gunner/tris.md2"             ,"models/monsters/gunner/skin.pcx",             "models/monsters/gunner/skin.pcx",                 L"\\monsters\\gunner.object\\", false},
    {"noname","models/monsters/infantry/tris.md2"           ,"models/monsters/infantry/skin.pcx",           "models/monsters/infantry/skin.pcx",               L"\\monsters\\infantry.object\\", false},
    {"noname","models/monsters/soldier/tris.md2"            ,"models/monsters/soldier/skin.pcx",            "models/monsters/soldier/skin.pcx",                L"\\monsters\\soldier.object\\", false},
    {"noname","models/monsters/tank/tris.md2"               ,"models/monsters/tank/skin.pcx",               "models/monsters/tank/skin.pcx",                   L"\\monsters\\tank.object\\", false},
    {"noname","models/monsters/commandr/tris.md2"           ,"models/monsters/commandr/skin.pcx",           "models/monsters/commandr/skin.pcx",               L"\\monsters\\commandr.object\\", false},
    {"noname","models/monsters/commandr/head/tris.md2"      ,"models/monsters/commandr/head/skin.pcx",      "models/monsters/commandr/head/skin.pcx",          L"\\monsters\\commandr_head.object\\", false},
    {"noname","models/monsters/medic/tris.md2"              ,"models/monsters/medic/skin.pcx",              "models/monsters/medic/skin.pcx",                  L"\\monsters\\medic.object\\", false},
    {"noname","models/monsters/flipper/tris.md2"            ,"models/monsters/flipper/skin.pcx",            "models/monsters/flipper/skin.pcx",                L"\\monsters\\flipper.object\\", false},
    {"noname","models/monsters/bitch/tris.md2"              ,"models/monsters/bitch/skin.pcx",              "models/monsters/bitch/skin.pcx",                  L"\\monsters\\bitch.object\\", false},
    {"noname","models/monsters/float/tris.md2"              ,"models/monsters/float/skin.pcx",              "models/monsters/float/skin.pcx",                  L"\\monsters\\float.object\\", false},
    {"noname","models/monsters/parasite/tris.md2"           ,"models/monsters/parasite/skin.pcx",           "models/monsters/parasite/skin.pcx",               L"\\monsters\\parasite.object\\", false},
    {"noname","models/monsters/parasite/segment/tris.md2"   ,"models/monsters/parasite/segment/skin.pcx",   "models/monsters/parasite/segment/skin.pcx",       L"\\monsters\\parasite_segment.object\\", false},
    {"noname","models/monsters/parasite/tip/tris.md2"       ,"models/monsters/parasite/tip/skin.pcx",       "models/monsters/parasite/tip/skin.pcx",           L"\\monsters\\parasite_tip.object\\", false},
    {"noname","models/monsters/flyer/tris.md2"              ,"models/monsters/flyer/skin.pcx",              "models/monsters/flyer/skin.pcx",                  L"\\monsters\\flyer.object\\", false},
    {"noname","models/monsters/brain/tris.md2"              ,"models/monsters/brain/skin.pcx",              "models/monsters/brain/skin.pcx",                  L"\\monsters\\brain.object\\", false},
    {"noname","models/monsters/hover/tris.md2"              ,"models/monsters/hover/skin.pcx",              "models/monsters/hover/skin.pcx",                  L"\\monsters\\hover.object\\", false},
    {"noname","models/monsters/mutant/tris.md2"             ,"models/monsters/mutant/skin.pcx",             "models/monsters/mutant/skin.pcx",                 L"\\monsters\\mutant.object\\", false},
    {"noname","models/monsters/insane/tris.md2"             ,"models/monsters/insane/skin.pcx",             "models/monsters/insane/skin.pcx",                 L"\\monsters\\insane.object\\", false},
    {"noname","models/monsters/boss1/tris.md2"              ,"models/monsters/boss1/skin.pcx",              "models/monsters/boss1/skin.pcx",                  L"\\monsters\\boss1.object\\", false},
    {"noname","models/monsters/boss2/tris.md2"              ,"models/monsters/boss2/skin.pcx",              "models/monsters/boss2/skin.pcx",                  L"\\monsters\\boss2.object\\", false},
    {"noname","models/monsters/boss3/jorg/tris.md2"         ,"models/monsters/boss3/jorg/skin.pcx",         "models/monsters/boss3/jorg/skin.pcx",             L"\\monsters\\boss3_jorg.object\\", false},
    {"noname","models/monsters/boss3/rider/tris.md2"        ,"models/monsters/boss3/rider/skin.pcx",        "models/monsters/boss3/rider/skin.pcx",            L"\\monsters\\boss3_rider.object\\", false},
    {"noname","models/weapons/g_bfg/tris.md2"               ,"models/weapons/g_bfg/skin.pcx",               "models/weapons/g_bfg/skin.pcx",                   L"\\weapons\\bfg.object\\", false},
    {"noname","models/weapons/g_blast/tris.md2"             ,"models/weapons/g_blast/skin.pcx",             "models/weapons/g_blast/skin.pcx",                 L"\\weapons\\blast.object\\", false},
    {"noname","models/weapons/g_chain/tris.md2"             ,"models/weapons/g_chain/skin.pcx",             "models/weapons/g_chain/skin.pcx",                 L"\\weapons\\chain.object\\", false},
    {"noname","models/weapons/g_disint/tris.md2"            ,"models/weapons/g_disint/skin.pcx",            "models/weapons/g_disint/skin.pcx",                L"\\weapons\\disint.object\\", false},
    {"noname","models/weapons/g_flareg/tris.md2"            ,"models/weapons/g_flareg/skin.pcx",            "models/weapons/g_flareg/skin.pcx",                L"\\weapons\\flareg.object\\", false},
    {"noname","models/weapons/g_hyperb/tris.md2"            ,"models/weapons/g_hyperb/skin.pcx",            "models/weapons/g_hyperb/skin.pcx",                L"\\weapons\\hyperb.object\\", false},
    {"noname","models/weapons/g_launch/tris.md2"            ,"models/weapons/g_launch/skin.pcx",            "models/weapons/g_launch/skin.pcx",                L"\\weapons\\launch.object\\", false},
    {"noname","models/weapons/g_machn/tris.md2"             ,"models/weapons/g_machn/skin.pcx",             "models/weapons/g_machn/skin.pcx",                 L"\\weapons\\machn.object\\", false},
    {"noname","models/weapons/g_rail/tris.md2"              ,"models/weapons/g_rail/skin.pcx",              "models/weapons/g_rail/skin.pcx",                  L"\\weapons\\rail.object\\", false},
    {"noname","models/weapons/g_rocket/tris.md2"            ,"models/weapons/g_rocket/skin.pcx",            "models/weapons/g_rocket/skin.pcx",                L"\\weapons\\rocket.object\\", false},
    {"noname","models/weapons/g_shotg/tris.md2"             ,"models/weapons/g_shotg/skin.pcx",             "models/weapons/g_shotg/skin.pcx",                 L"\\weapons\\shotg.object\\", false},
    {"noname","models/weapons/g_shotg2/tris.md2"            ,"models/weapons/g_shotg2/skin.pcx",            "models/weapons/g_shotg2/skin.pcx",                L"\\weapons\\shotg2.object\\", false},
    {"noname","models/weapons/v_bfg/tris.md2"               ,"models/weapons/v_bfg/skin.pcx",               "models/weapons/v_bfg/skin.pcx",                   L"\\weapons\\vbfg.object\\", false},
    {"noname","models/weapons/v_blast/tris.md2"             ,"models/weapons/v_blast/skin.pcx",             "models/weapons/v_blast/skin.pcx",                 L"\\weapons\\vblast.object\\", false},
    {"noname","models/weapons/v_chain/tris.md2"             ,"models/weapons/v_chain/skin.pcx",             "models/weapons/v_chain/skin.pcx",                 L"\\weapons\\vchain.object\\", false},
    {"noname","models/weapons/v_disint/tris.md2"            ,"models/weapons/v_disint/skin.pcx",            "models/weapons/v_disint/skin.pcx",                L"\\weapons\\vdisint.object\\", false},
    {"noname","models/weapons/v_flareg/tris.md2"            ,"models/weapons/v_flareg/base.pcx",            "models/weapons/v_flareg/base.pcx",                L"\\weapons\\vflareg.object\\", false},
    {"noname","models/weapons/v_hyperb/tris.md2"            ,"models/weapons/v_hyperb/skin.pcx",            "models/weapons/v_hyperb/skin.pcx",                L"\\weapons\\vhyperb.object\\", false},
    {"noname","models/weapons/v_launch/tris.md2"            ,"models/weapons/v_launch/skin.pcx",            "models/weapons/v_launch/skin.pcx",                L"\\weapons\\vlaunch.object\\", false},
    {"noname","models/weapons/v_machn/tris.md2"             ,"models/weapons/v_machn/skin.pcx",             "models/weapons/v_machn/skin.pcx",                 L"\\weapons\\vmachn.object\\", false},
    {"noname","models/weapons/v_rail/tris.md2"              ,"models/weapons/v_rail/skin.pcx",              "models/weapons/v_rail/skin.pcx",                  L"\\weapons\\vrail.object\\", false},
    {"noname","models/weapons/v_rocket/tris.md2"            ,"models/weapons/v_rocket/skin.pcx",            "models/weapons/v_rocket/skin.pcx",                L"\\weapons\\vrocket.object\\", false},
    {"noname","models/weapons/v_shotg/tris.md2"             ,"models/weapons/v_shotg/skin.pcx",             "models/weapons/v_shotg/skin.pcx",                 L"\\weapons\\vshotg.object\\", false},
    {"noname","models/weapons/v_shotg2/tris.md2"            ,"models/weapons/v_shotg2/skin.pcx",            "models/weapons/v_shotg2/skin.pcx",                L"\\weapons\\vshotg2.object\\", false},
    
    //{"soldier","models/monsters/soldier/tris.md2", "models/monsters/soldier/skin.pcx", "models/monsters/soldier/pain.pcx", L"\\enemies\\soldier.object\\", false},
    //{"doomguy","C:\\Users\\coopp\\Documents\\Visual Studio 2012\\Projects\\GDK\\Games\\Quake2\\content\\Pak\\downloads\\doom\\tris.md2", "C:\\Users\\coopp\\Documents\\Visual Studio 2012\\Projects\\GDK\\Games\\Quake2\\content\\Pak\\downloads\\doom\\doom.pcx", "C:\\Users\\coopp\\Documents\\Visual Studio 2012\\Projects\\GDK\\Games\\Quake2\\content\\Pak\\downloads\\doom\\doom.pcx", L"\\enemies\\doomguy.object\\", true},
};

#endif

int _tmain(int argc, _TCHAR* argv[])
{
    HRESULT hr = S_OK;
    std::vector<byte_t> data;
    std::wstring contentRoot;
    std::vector<RGBQUAD> bmpal;
    std::vector<byte_t> bmpData;
    BITMAPINFOHEADER bmih = {0};

    if (argc < 3)
    {
        printf("contentextractor.exe <pak file path> <content path>\n");
        return 0;
    }

    contentRoot = argv[2];

    // uncomment to list ALL PAK file chunks
    //CHECKHR(GetPakChunk(argv[1], "", data), L"Error listing all chunks");

#ifndef BUILD_LOOSE_FILES_ONLY

    std::wstring contentDir;
    contentDir = contentRoot;
    contentDir.append(L"\\Resources");

    // Extract all pics .PCX from PAK
    CHECKHR(SavePCXPicsFilesAsTextureContentFromPak(argv[1], contentDir.c_str()), L"Failed to extract all pics .pcx from PAK");

    // Extract all .WAVs from PAK
    CHECKHR(SaveFilesFromPak(argv[1], L".wav", contentDir.c_str()), L"Failed to extract all .wavs from PAK");
    
    // Extract all .WALs as .TEXTURE from PAK
    CHECKHR(GetPakChunk(argv[1], "pics/colormap.pcx", data), L"Error finding colormap texture");
    ConvertPCXToBMP((PCXFILEHEADER*)data.data(), data.data(), (uint32_t)data.size(), bmpData, bmpal, bmih);
    CHECKHR(SaveWALFilesAsTextureContentFromPak(argv[1], contentDir.c_str(), bmpal), L"Failed to extract all .wal from PAK");

    // Extract all .BSPs as folders of .object's from PAK
    CHECKHR(SaveBSPLevelsAsObjectsFromPak(argv[1], contentDir.c_str()), L"Failed to extract all .bsps from PAK");

#endif

    // Extract md2 models and textures as .GEOMETRY and .TEXTURE from PAK
    for (size_t i = 0; i < ARRAYSIZE(quake_content); i++)
    {
        std::wstring contentDir;

        contentDir = contentRoot;
        contentDir.append(quake_content[i].gameContentPath);

        SHCreateDirectory(nullptr, contentDir.c_str());

        contentDir.append(L"mesh.geometry");

        if (quake_content[i].looseFiles)
        {
            SaveMD2FileAsGeometryContent(contentDir.c_str(), AnsiToWide(quake_content[i].md2Model).c_str());
        }
        else
        {
            CHECKHR(GetPakChunk(argv[1], quake_content[i].md2Model, data), L"Error finding md2 model");
            SaveMD2AsGeometryContent(contentDir.c_str(), quake_content[i].name, data.data(), (uint32_t)data.size());
        }

        data.clear();

        contentDir = contentRoot;
        contentDir.append(quake_content[i].gameContentPath);

        SHCreateDirectory(nullptr, contentDir.c_str());

        contentDir.append(L"primary.texture");
        if (quake_content[i].looseFiles)
        {
            SavePCXFileAsTextureContent(contentDir.c_str(), AnsiToWide(quake_content[i].skin1).c_str());
        }
        else
        {
            CHECKHR(GetPakChunk(argv[1], quake_content[i].skin1, data), L"Error finding md2 primary texture");
            SavePCXAsTextureContent(contentDir.c_str(), quake_content[i].name, data.data(), (uint32_t)data.size());
        }
    }

Exit:
    
    return 0;
}
