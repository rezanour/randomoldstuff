#ifndef _POWERUP_H_
#define _POWERUP_H_

/* Powerup types */
#define POWERUP_TYPE_NONE           0
#define POWERUP_TYPE_FOOD           1
#define POWERUP_TYPE_HEALTHKIT      2
#define POWERUP_TYPE_AMMO           3
#define POWERUP_TYPE_MACHINEGUN     4
#define POWERUP_TYPE_CHAINGUN       5
#define POWERUP_TYPE_TREASURE_CROSS 6
#define POWERUP_TYPE_TREASURE_CUP   7
#define POWERUP_TYPE_TREASURE_CHEST 8
#define POWERUP_TYPE_TREASURE_CROWN 9
#define POWERUP_TYPE_TREASURE_1UP   10
#define POWERUP_TYPE_GOLD_KEY       11
#define POWERUP_TYPE_SILVER_KEY     12
#define MAX_POWERUP_TYPES           13

VOID PowerupInit();
VOID PowerupShutdown();

USHORT PowerupCreate(VEC2 position, BYTE type);

HSOUND PowerupGetSound(USHORT powerupIndex);
HSPRITE PowerupGetSprite(USHORT powerupIndex);
VEC2 PowerupGetPosition(USHORT powerupIndex);

/* for ammo, returns # bullets in clip. For health, returns amount health restored. For gun, returns the gun id. For treasure, returns the score */
VOID PowerupGetInfo(USHORT powerupIndex, BYTE* type, USHORT* value);

BOOL PowerupIsAmmo(BYTE type);
BOOL PowerupIsHealth(BYTE type);
BOOL PowerupIsGun(BYTE type);
BOOL PowerupIsTreasure(BYTE type);
BOOL PowerupIsKey(BYTE type);

#endif // _POWERUP_H_
