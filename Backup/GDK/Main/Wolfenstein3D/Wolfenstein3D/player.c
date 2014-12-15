#include "common.h"

PLAYER g_player;
BYTE g_bulletsUsedPerShot[WEAPON_LAST] = {0};

/* Start a new player (new game) */
VOID PlayerInit(BOOL notE1M1)
{
    /* ensure bullet mapping is initialized */
    g_bulletsUsedPerShot[WEAPON_KNIFE] = 0;
    g_bulletsUsedPerShot[WEAPON_PISTOL] = 1;
    g_bulletsUsedPerShot[WEAPON_MACHINEGUN] = 2; /* TODO: what are the real numbers here? */
    g_bulletsUsedPerShot[WEAPON_CHAINGUN] = 3;

    PlayerReset();

    g_player.Level = notE1M1 ? g_player.Level : 0;
    g_player.Lives = 3;
    g_player.Score = 0;
    g_player.HasGoldKey = 0;
    g_player.HasSilverKey = 0;
}

/* Reset player, but keep level (restart level, died, etc...) */
VOID PlayerReset()
{
    g_player.Guns = 0;
    PlayerSetGun(WEAPON_KNIFE);
    PlayerSetGun(WEAPON_PISTOL);

    g_player.Ammo = 8;
    g_player.CurrentWeapon = WEAPON_PISTOL;
    g_player.Health = 100;

    AnimationInit(&g_player.GunAnimation, 0, 0, FALSE);

    g_player.Position = Vec2Zero();
    g_player.Direction = Vec2Up();
}

VOID PlayerSetPosition(float x, float y)
{
    g_player.Position.x = x;
    g_player.Position.y = y;
}

VOID PlayerSetGun(BYTE gun)
{
    BYTE flag = 0x1 << gun;
    g_player.Guns |= flag;
}

BOOL PlayerTakeDamage(BYTE damage)
{
    g_player.Health = max((SHORT)g_player.Health - damage, 0);
    if (g_player.Health == 0)
    {
        return TRUE;
    }
    return FALSE;
}

/* check to see if player has a particular gun */
BOOL PlayerHasGun(BYTE gun)
{
    BYTE flag = 0x1 << gun;
    return (g_player.Guns & flag) == flag;
}

/* flips through the guns */
VOID PlayerSwitchNextGun()
{
    BYTE nextWeapon = g_player.CurrentWeapon + 1;

    if (nextWeapon == WEAPON_LAST)
        nextWeapon = WEAPON_KNIFE;

    while (!PlayerHasGun(nextWeapon))
    {
        ++nextWeapon;
        if (nextWeapon == WEAPON_LAST)
            nextWeapon = WEAPON_KNIFE;
    }

    g_player.CurrentWeapon = nextWeapon;
}

VOID PlayerSwitchPrevGun()
{
    BYTE nextWeapon = g_player.CurrentWeapon - 1;
    /* catch underflow */
    if (nextWeapon > WEAPON_LAST)
        nextWeapon = WEAPON_LAST - 1;

    while (!PlayerHasGun(nextWeapon))
    {
        --nextWeapon;
        /* catch underflow */
        if (nextWeapon > WEAPON_LAST)
            nextWeapon = WEAPON_LAST - 1;
    }

    g_player.CurrentWeapon = nextWeapon;
}

VOID PlayerSwitchToGun(BYTE gun)
{
    if (PlayerHasGun(gun))
    {
        g_player.CurrentWeapon = gun;
    }
}

/* called when an attack is initiated. If the current weapon is out of ammo, rolls down the list until it finds a suitable weapon */
VOID PlayerEnsureHasWeaponWithAmmo()
{
    while (g_bulletsUsedPerShot[g_player.CurrentWeapon] > g_player.Ammo)
    {
        PlayerSwitchPrevGun();
    }
}

/* try to move player in the direction given by the move vector, taking into account collision */
VOID PlayerMove(VEC2 movement)
{
    g_player.Position = Vec2Add(g_player.Position, movement);
}

/* check for health == 0 */
BOOL PlayerIsDead()
{
    return g_player.Health == 0;
}

BOOL PlayerIsAttacking()
{
    return AnimationIsValid(&g_player.GunAnimation);
}

VOID DoTheActualAttack()
{
    UINT enemy = INVALID_ENTITY;
    USHORT weaponDamage = 3;
    g_player.Ammo = max(g_player.Ammo - g_bulletsUsedPerShot[g_player.CurrentWeapon], 0);

    PlayWeaponSound(g_player.CurrentWeapon, TRUE);

    switch(g_player.CurrentWeapon)
    {
    case WEAPON_PISTOL:
        weaponDamage = 15;
        break;
    case WEAPON_MACHINEGUN:
        weaponDamage = 20;
        break;
    case WEAPON_CHAINGUN:
        weaponDamage = 35;
        break;
    case WEAPON_KNIFE:
    default:
        weaponDamage = 10;
        break;
    }

    if (g_player.CurrentWeapon != WEAPON_KNIFE)
    {
        enemy = LevelRaycastEnemy(g_player.Position, g_player.Direction);
        if (enemy != INVALID_ENTITY)
        {
            EnemyTakeDamage(enemy, weaponDamage);
        }
    }
    else
    {
        enemy = LevelGetEnemyInFront(g_player.Position, g_player.Direction);
        if (enemy != INVALID_ENTITY)
        {
            VEC2 vec = Vec2Add(EnemyGetPosition(enemy), Vec2Neg(g_player.Position));
            if (Vec2Len(vec) < 4.f)
            {
                EnemyTakeDamage(enemy, weaponDamage);
            }
        }
    }
}

VOID StartAttack()
{
    PlayerEnsureHasWeaponWithAmmo();

    if (g_player.Ammo >= g_bulletsUsedPerShot[g_player.CurrentWeapon])
    {
        switch (g_player.CurrentWeapon)
        {
        case WEAPON_KNIFE:
            AnimationInit(&g_player.GunAnimation, 0.5f, 4, FALSE);
            break;

        case WEAPON_PISTOL:
            AnimationInit(&g_player.GunAnimation, 0.5f, 4, FALSE);
            break;

        case WEAPON_MACHINEGUN:
            AnimationInit(&g_player.GunAnimation, 0.25f, 4, FALSE);
            break;

        case WEAPON_CHAINGUN:
            AnimationInit(&g_player.GunAnimation, 0.125f, 4, FALSE);
            break;
        }
    }
}

VOID UpdateAttack(BOOL attackPressed)
{
    UINT enemy = INVALID_ENTITY;
    static BOOL justFired = FALSE;
    BOOL animationStillGood = FALSE;

    animationStillGood = AnimationUpdate(&g_player.GunAnimation);

    if (attackPressed && 
        (g_player.CurrentWeapon == WEAPON_MACHINEGUN || g_player.CurrentWeapon == WEAPON_CHAINGUN) &&
        g_player.Ammo >= g_bulletsUsedPerShot[g_player.CurrentWeapon] &&
        !animationStillGood)
    {
        AnimationSetFrame(&g_player.GunAnimation, 1);
        animationStillGood = TRUE;
    }

    if (animationStillGood)
    {
        if (AnimationGetFrame(&g_player.GunAnimation) == 1)
        {
            if (!justFired)
            {
                DoTheActualAttack();
                justFired = TRUE;
            }
        }
        else
        {
            justFired = FALSE;
        }
    }
    else
    {
        AnimationInit(&g_player.GunAnimation, 0, 0, FALSE);
    }
}

VOID AwardPowerup(USHORT index)
{
    BYTE type;
    USHORT value;

    PowerupGetInfo(index, &type, &value);

    if (PowerupIsAmmo(type))
    {
        value = min(value, 255 - g_player.Ammo);
        if (value > 0)
        {
            g_player.Ammo += value;
            LevelRemovePowerup(g_player.Position.x, g_player.Position.y, index);
        }
    }
    else if (PowerupIsHealth(type))
    {
        value = min(value, 100 - g_player.Health);
        if (value > 0)
        {
            g_player.Health += value;
            LevelRemovePowerup(g_player.Position.x, g_player.Position.y, index);
        }
    }
    else if (PowerupIsGun(type))
    {
        PlayerSetGun(value);
        PlayerSwitchToGun(value);
        
        value = min(8, 255 - g_player.Ammo);
        
        g_player.Ammo += value; // guns come with ammo
        LevelRemovePowerup(g_player.Position.x, g_player.Position.y, index);
    }
    else if (PowerupIsTreasure(type))
    {
        g_player.Score += value;

        if (type == POWERUP_TYPE_TREASURE_1UP)
        {
            // 1up power up gives extra life and ammo in addition to score
            g_player.Lives += 1;

            g_player.Ammo += min(25, 255 - g_player.Ammo);
        }

        LevelRemovePowerup(g_player.Position.x, g_player.Position.y, index);
    }
    else if (PowerupIsKey(type))
    {
        if (type == POWERUP_TYPE_GOLD_KEY)
        {
            g_player.HasGoldKey = 1;
        }
        
        if (type == POWERUP_TYPE_SILVER_KEY)
        {
            g_player.HasSilverKey = 1;
        }

        LevelRemovePowerup(g_player.Position.x, g_player.Position.y, index);
    }
}

VOID PlayerUpdate()
{
    VEC2 movement = {0};
    VEC2 perp = {0};
    SHORT turn = 0;
    float angle = 0;
    BOOL attackPressed = FALSE;
    BOOL strafing = FALSE;
    USHORT powerups[5] = {0};
    USHORT numPowerups, i;

    if (IsKeyJustPressed(VK_SPACE))
    {
        LevelClickInFront(g_player.Position, g_player.Direction);
    }

    /* switch weapons */
    if (IsKeyJustPressed(VkKeyScan('1')))
    {
        PlayerSwitchToGun(WEAPON_KNIFE);
    }
    else if (IsKeyJustPressed(VkKeyScan('2')))
    {
        PlayerSwitchToGun(WEAPON_PISTOL);
    }
    else if (IsKeyJustPressed(VkKeyScan('3')))
    {
        PlayerSwitchToGun(WEAPON_MACHINEGUN);
    }
    else if (IsKeyJustPressed(VkKeyScan('4')))
    {
        PlayerSwitchToGun(WEAPON_CHAINGUN);
    }

    /* movement */
    perp = Vec2Perp(g_player.Direction);

    if (IsKeyDown(VkKeyScanA('z')))
    {
        strafing = TRUE;
    }

    if (IsKeyDown(VK_UP))
    {
        movement = Vec2Add(movement, Vec2Scale(g_player.Direction, PLAYER_SPEED * g_elapsedTime));
    }
    if (strafing && IsKeyDown(VK_LEFT))
    {
        movement = Vec2Add(movement, Vec2Scale(Vec2Neg(perp), PLAYER_SPEED * g_elapsedTime));
    }
    if (IsKeyDown(VK_DOWN))
    {
        movement = Vec2Add(movement, Vec2Scale(Vec2Neg(g_player.Direction), PLAYER_SPEED * g_elapsedTime));
    }
    if (strafing && IsKeyDown(VK_RIGHT))
    {
        movement = Vec2Add(movement, Vec2Scale(perp, PLAYER_SPEED * g_elapsedTime));
    }

    if (Vec2LenSquared(movement) > 0.000001f)
    {
        movement = Vec2Scale(Vec2Normalize(movement), PLAYER_SPEED * g_elapsedTime);
        g_player.Position = LevelMoveWithCollision(g_player.Position, movement);
    }

    /* turning */
    if (!strafing && IsKeyDown(VK_LEFT))
    {
        turn -= 1;
    }
    if (!strafing && IsKeyDown(VK_RIGHT))
    {
        turn += 1;
    }

    if (turn != 0)
    {
        angle = atan2(g_player.Direction.y, g_player.Direction.x);
        angle += turn * PLAYER_TURN_SPEED * g_elapsedTime;
        g_player.Direction.x = cos(angle);
        g_player.Direction.y = sin(angle);
        g_player.Direction = Vec2Normalize(g_player.Direction);
    }

    /* attack */
    attackPressed = IsKeyDown(VkKeyScanA('x')) || IsKeyDown(VK_CONTROL);
    if (attackPressed && !PlayerIsAttacking())
    {
        StartAttack();
    }
    
    if (PlayerIsAttacking())
    {
        UpdateAttack(attackPressed);
    }

    /* did you pick up anything? */
    LevelGetPowerups(g_player.Position.x, g_player.Position.y, powerups, ARRAYSIZE(powerups), &numPowerups);
    for (i = 0; i < numPowerups; ++i)
    {
        AwardPowerup(powerups[i]);
    }
}
