#include "common.h"

#define MAX_BULLETS         200
#define MAX_SHELLS           50
#define MAX_ROCKETS          50
#define MAX_CELLS           300

typedef struct
{
    int MaxAmmo;
    weapon_type_t Type;
    weapon_ammo_type_t AmmoType;
    char* Sound;
} weapon_t;

weapon_t g_WeaponTable[] = 
{
    { 0,           weapon_type_fist,         weapon_ammo_none    ,"DSPUNCH"},
    { MAX_BULLETS, weapon_type_pistol,       weapon_ammo_bullets ,"DSPISTOL"},
    { MAX_SHELLS,  weapon_type_shotgun,      weapon_ammo_shells  ,"DSSHOTGN"},
    { MAX_BULLETS, weapon_type_chaingun,     weapon_ammo_bullets ,"DSPISTOL"},
    { MAX_ROCKETS, weapon_type_missile,      weapon_ammo_rockets ,"DSRLAUNC"},
    { MAX_CELLS,   weapon_type_plasma,       weapon_ammo_cells   ,"DSPLASMA"},
    { MAX_CELLS,   weapon_type_bfg,          weapon_ammo_cells   ,"DSBFG"},
    { 0,           weapon_type_chainsaw,     weapon_ammo_none    ,"DSSAWFUL"},
    { MAX_SHELLS,  weapon_type_supershotgun, weapon_ammo_shells  ,"DSDSHTGN"},
};

typedef struct
{
    vector2_t Position;
    vector2_t Direction;
    int       Health;
    int       Armor;
    weapon_type_t Weapon;
    int           Ammo[NUMAMMOTYPES];
    BOOL          Weapons[NUMWEAPONS]; // TRUE = have, FALSE = not have
    BOOL      GodMode;
    BOOL      Keys[NUMKEYS];
    BOOL      Shooting;
} player_t;

static player_t g_player;

//
// public player methods
//

BOOL PlayerHasKey(key_type_t keyType)
{
    return g_player.Keys[keyType];
}

void PlayerGiveKey(key_type_t keyType)
{
    g_player.Keys[keyType] = TRUE;
}

BOOL PlayerInGodMode()
{
    return g_player.GodMode;
}

void PlayerSetGodMode(BOOL godMode)
{
    g_player.GodMode = godMode;
}

void PlayerUseAmmo()
{
    int i = 0;
    for (i = 0; i < ARRAYSIZE(g_WeaponTable); i++)
    {
        if (g_WeaponTable[i].Type == g_player.Weapon)
        {
            if ((g_player.Ammo[g_WeaponTable[i].AmmoType] > 0)|| g_player.Weapon == weapon_type_fist || g_player.Weapon == weapon_type_chainsaw)
            {
                AudioPlaySound(g_WeaponTable[i].Sound);
                g_player.Ammo[g_WeaponTable[i].AmmoType] -= 1;
                if (g_player.Ammo[g_WeaponTable[i].AmmoType] < 0)
                {
                    g_player.Ammo[g_WeaponTable[i].AmmoType] = 0;
                }
            }
            break;
        }
    }
}

void PlayerShootWeapon()
{
    int i = 0;

    if (PlayerIsShooting())
        return;

    for (i = 0; i < ARRAYSIZE(g_WeaponTable); i++)
    {
        if (g_WeaponTable[i].Type == g_player.Weapon)
        {
            if ((g_player.Ammo[g_WeaponTable[i].AmmoType] > 0) || g_player.Weapon == weapon_type_fist || g_player.Weapon == weapon_type_chainsaw)
            {
                PlayerSetShooting(TRUE); // player took actual shot
            }
            break;
        }
    }
}

int PlayerGetWeaponMaxAmmo(weapon_type_t weaponType)
{
    int i = 0;
    for (i = 0; i < ARRAYSIZE(g_WeaponTable); i++)
    {
        if (g_WeaponTable[i].Type == weaponType)
        {
            return g_WeaponTable[i].MaxAmmo;
        }
    }

    return 0;
}

weapon_ammo_type_t PlayerGetAmmoType(weapon_type_t weaponType)
{
    int i = 0;
    for (i = 0; i < ARRAYSIZE(g_WeaponTable); i++)
    {
        if (g_WeaponTable[i].Type == weaponType)
        {
            return g_WeaponTable[i].AmmoType;
        }
    }

    return weapon_ammo_none;
}

int PlayerGetAmmoByWeapon(weapon_type_t weaponType)
{
    return g_player.Ammo[PlayerGetAmmoType(weaponType)];
}

int PlayerGetAmmo(weapon_ammo_type_t ammoType)
{
    return g_player.Ammo[ammoType];
}

void PlayerAddAmmoByWeapon(weapon_type_t weaponType, int ammo)
{
    PlayerAddAmmo(PlayerGetAmmoType(weaponType), ammo);
}

int PlayerGetMaxAmmo(weapon_ammo_type_t ammoType)
{
    int i = 0;
    for (i = 0; i < ARRAYSIZE(g_WeaponTable); i++)
    {
        if (g_WeaponTable[i].AmmoType == ammoType)
        {
            return g_WeaponTable[i].MaxAmmo;
        }
    }

    return 0;
}

void PlayerAddAmmo(weapon_ammo_type_t ammoType, int ammo)
{
    g_player.Ammo[ammoType] += ammo;
    if (g_player.Ammo[ammoType] > PlayerGetMaxAmmo(ammoType))
    {
        g_player.Ammo[ammoType] = PlayerGetMaxAmmo(ammoType);
    }
}

void PlayerAddWeapon(weapon_type_t weaponType)
{
    g_player.Weapons[weaponType] = TRUE;
}

vector2_t PlayerGetPosition()
{
    return g_player.Position;
}

vector2_t PlayerGetDirection()
{
    return g_player.Direction;
}

void PlayerSetPosition(vector2_t position)
{
    g_player.Position = position;
}

void PlayerSetDirection(vector2_t direction)
{
    g_player.Direction = direction;
}

void PlayerMove(vector2_t movement)
{
    g_player.Position = Vector2Add(g_player.Position, movement);
}

void PlayerRotate(float amount)
{
    double ang = atan2(g_player.Direction.Y, g_player.Direction.X);
    ang += amount;
    g_player.Direction.X = (float)cos(ang);
    g_player.Direction.Y = (float)sin(ang);
}

int PlayerGetHealth()
{
    return g_player.Health;
}

void PlayerTakeDamage(int damage)
{
    if (PlayerInGodMode())
    {
        return;
    }

    g_player.Health -= damage;
    if (g_player.Health < 0)
    {
        g_player.Health = 0;
    }
}

weapon_type_t PlayerGetWeapon()
{
    return g_player.Weapon;
}

void PlayerSwitchWeaponById(int id)
{
    weapon_type_t weaponType = weapon_type_fist;
    switch(id)
    {
    case 1:
        weaponType = weapon_type_fist;
        break;
    case 2:
        weaponType = weapon_type_pistol;
        break;
    case 3:
        weaponType = weapon_type_shotgun;
        break;
    case 4:
        weaponType = weapon_type_chaingun;
        break;
    case 5:
        weaponType = weapon_type_missile;
        break;
    case 6:
        weaponType = weapon_type_plasma;
        break;
    case 7:
        weaponType = weapon_type_bfg;
        break;
    default:
        break;
    }

    PlayerSwitchWeapon(weaponType);
}

void PlayerSwitchWeapon(weapon_type_t weaponType)
{
    if (g_player.Weapons[weaponType])
    {
        // The chainsaw and fist share location 1, the original gameplay completely replaced the fist with the
        // chainsaw if the player has one.
        if ((weaponType == weapon_type_fist) && (g_player.Weapons[weapon_type_chainsaw]))
        {
            weaponType = weapon_type_chainsaw;
        }

        // If the player is switching to shotgun, and the player is already holding the shotgun...and they have a super shotgun
        // upgrade their shotgun to super shotgun.
        if ((weaponType == weapon_type_shotgun) && (g_player.Weapons[weapon_type_supershotgun]) && (g_player.Weapon == weapon_type_shotgun))
        {
            weaponType = weapon_type_supershotgun;
        }

        // If the player is switching to shotgun and the player is already holding a super shotgun, down grade them to regular shotgun
        if ((weaponType == weapon_type_shotgun) && (g_player.Weapon == weapon_type_shotgun))
        {
            weaponType = weapon_type_shotgun;
        }

        g_player.Weapon = weaponType;
    }
}

BOOL PlayerHasWeapon(weapon_type_t weaponType)
{
    return g_player.Weapons[weaponType];
}

int PlayerGetArmor()
{
    return g_player.Armor;
}

void PlayerAddArmor(int armor)
{
    g_player.Armor += armor;
}

void PlayerReset()
{
    int i = 0;

    g_player.Health = 100;
    g_player.GodMode = FALSE;

    g_player.Weapons[weapon_type_fist]         = TRUE;
    g_player.Weapons[weapon_type_pistol]       = TRUE;
    g_player.Weapons[weapon_type_shotgun]      = FALSE;
    g_player.Weapons[weapon_type_chaingun]     = FALSE;
    g_player.Weapons[weapon_type_missile]      = FALSE;
    g_player.Weapons[weapon_type_plasma]       = FALSE;
    g_player.Weapons[weapon_type_bfg]          = FALSE;
    g_player.Weapons[weapon_type_chainsaw]     = FALSE;
    g_player.Weapons[weapon_type_supershotgun] = FALSE;
    
    g_player.Ammo[weapon_ammo_bullets] = 50;
    g_player.Ammo[weapon_ammo_shells]  = 0;
    g_player.Ammo[weapon_ammo_cells]   = 0;
    g_player.Ammo[weapon_ammo_rockets] = 0;

    for (i = 0; i < ARRAYSIZE(g_player.Keys);i++)
    {
        g_player.Keys[i] = FALSE;
    }

    g_player.Weapon = weapon_type_pistol;
}

void PlayerGiveAll()
{
    int i = 0;

    g_player.Health = 200;
    g_player.Armor = 200;

    g_player.Weapons[weapon_type_fist]         = TRUE;
    g_player.Weapons[weapon_type_pistol]       = TRUE;
    g_player.Weapons[weapon_type_shotgun]      = TRUE;
    g_player.Weapons[weapon_type_chaingun]     = TRUE;
    g_player.Weapons[weapon_type_missile]      = TRUE;
    g_player.Weapons[weapon_type_plasma]       = TRUE;
    g_player.Weapons[weapon_type_bfg]          = TRUE;
    g_player.Weapons[weapon_type_chainsaw]     = TRUE;
    g_player.Weapons[weapon_type_supershotgun] = TRUE;
    
    g_player.Ammo[weapon_ammo_bullets] = MAX_BULLETS;
    g_player.Ammo[weapon_ammo_shells]  = MAX_SHELLS;
    g_player.Ammo[weapon_ammo_cells]   = MAX_CELLS;
    g_player.Ammo[weapon_ammo_rockets] = MAX_ROCKETS;

    for (i = 0; i < ARRAYSIZE(g_player.Keys);i++)
    {
        g_player.Keys[i] = TRUE;
    }
}

void PlayerSetShooting(BOOL shooting)
{
    g_player.Shooting = shooting;
}

BOOL PlayerIsShooting()
{
    return g_player.Shooting;
}

