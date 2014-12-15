#ifndef _PLAYER_H_
#define _PLAYER_H_

typedef enum
{
    weapon_ammo_none = 0,
    weapon_ammo_bullets,
    weapon_ammo_shells,
    weapon_ammo_rockets,
    weapon_ammo_cells,

    NUMAMMOTYPES,
} weapon_ammo_type_t;

typedef enum
{
    weapon_type_fist,
    weapon_type_pistol,
    weapon_type_shotgun,
    weapon_type_chaingun,
    weapon_type_missile,
    weapon_type_plasma,
    weapon_type_bfg,
    weapon_type_chainsaw,
    weapon_type_supershotgun,

    NUMWEAPONS,

} weapon_type_t;

typedef enum
{
    key_type_red = 0,
    key_type_blue,
    key_type_yellow,
    key_type_red_skull,
    key_type_blue_skull,
    key_type_yellow_skull,

    NUMKEYS,
} key_type_t;

vector2_t PlayerGetPosition();
vector2_t PlayerGetDirection();

void PlayerSetPosition(vector2_t position);
void PlayerSetDirection(vector2_t direction);

void PlayerMove(vector2_t movement);
void PlayerRotate(float amount); // positive turns left

int PlayerGetHealth();
void PlayerTakeDamage(int damage);

BOOL PlayerHasKey(key_type_t keyType);
void PlayerGiveKey(key_type_t keyType);

void PlayerReset();

int PlayerGetWeaponMaxAmmo(weapon_type_t weaponType);
weapon_type_t PlayerGetWeapon();
void PlayerSwitchWeapon(weapon_type_t weaponType);
void PlayerSwitchWeaponById(int id);
int PlayerGetAmmoByWeapon(weapon_type_t weaponType);
int PlayerGetAmmo(weapon_ammo_type_t ammoType);
int PlayerGetMaxAmmo(weapon_ammo_type_t ammoType);
void PlayerAddAmmoByWeapon(weapon_type_t weaponType, int ammo);
void PlayerAddAmmo(weapon_ammo_type_t ammoType, int ammo);
void PlayerAddWeapon(weapon_type_t weaponType);
BOOL PlayerHasWeapon(weapon_type_t weaponType);
int PlayerGetArmor();
void PlayerAddArmor(int armor);

void PlayerGiveAll();
void PlayerShootWeapon();
void PlayerStopShooting();
void PlayerUseAmmo();

BOOL PlayerInGodMode();
void PlayerSetGodMode(BOOL godMode);

BOOL PlayerIsShooting();
void PlayerSetShooting(BOOL shooting);

#endif // _PLAYER_H_