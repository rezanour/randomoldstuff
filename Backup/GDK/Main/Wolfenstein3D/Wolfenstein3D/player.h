#ifndef _PLAYER_H_
#define _PLAYER_H_

typedef struct
{
    VEC2 Position;
    VEC2 Direction;
    BYTE Ammo;
    BYTE Health;
    BYTE CurrentWeapon;
    BYTE Level;
    BYTE Lives;
    BYTE Guns; /* each weapon's presence is determined by the weapon's index being 1 */
    INT  Score;
    BYTE HasGoldKey;
    BYTE HasSilverKey;
    BYTE Difficulty;
    ANIMATION GunAnimation;
} PLAYER;

extern PLAYER g_player;

/*
Player methods
*/

VOID PlayerInit(BOOL notE1M1);
VOID PlayerReset();
VOID PlayerSetGun(BYTE gun);
BOOL PlayerHasGun(BYTE gun);
VOID PlayerSwitchNextGun();
VOID PlayerSwitchPrevGun();
VOID PlayerSwitchToGun(BYTE gun);
BOOL PlayerIsDead();
BOOL PlayerIsAttacking();
VOID PlayerUpdate();
BOOL PlayerTakeDamage(BYTE damage);
/* called when an attack is initiated. If the current weapon is out of ammo, rolls down the list until it finds a suitable weapon */
VOID PlayerEnsureHasWeaponWithAmmo();
/* try to move player in the direction given by the move vector, taking into account collision */
VOID PlayerMove(VEC2 movement);
VOID PlayerSetPosition(float x, float y);

#endif /* _PLAYER_H_ */