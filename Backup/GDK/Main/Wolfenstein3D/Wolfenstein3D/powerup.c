#include "common.h"

typedef struct
{
    VEC2 position;
    USHORT value;
    BYTE type;
} POWERUP;

#define MAX_POWERUPS    1000
static POWERUP g_powerups[MAX_POWERUPS] = {0};
static USHORT g_numPowerups = 0;

static HSPRITE g_powerupSprites[22] = {INVALID_HSPRITE_VALUE};
static HSOUND g_powerupSounds[22] = { INVALID_HSOUND_VALUE };

VOID PowerupInit()
{
    USHORT index = POWERUP_TYPE_FOOD;

    for (; index < POWERUP_TYPE_TREASURE_1UP + 1; ++index)
    {
        g_powerupSprites[index - POWERUP_TYPE_FOOD] = W3DCreateSprite(OBJECT_TVDINNER + index - 1, FALSE);
        g_powerupSounds[index - POWERUP_TYPE_FOOD] = INVALID_HSOUND_VALUE;
    }

    g_powerupSounds[POWERUP_TYPE_AMMO] = AudioCreateSFX(WAV_ROOT"PickupAmmo.wav");
    g_powerupSounds[POWERUP_TYPE_CHAINGUN] = AudioCreateSFX(WAV_ROOT"PickupChaingun.wav");
    g_powerupSounds[POWERUP_TYPE_MACHINEGUN] = AudioCreateSFX(WAV_ROOT"PickupMachinegun.wav");
    g_powerupSounds[POWERUP_TYPE_FOOD] = AudioCreateSFX(WAV_ROOT"PickupFood.wav");
    g_powerupSounds[POWERUP_TYPE_HEALTHKIT] = AudioCreateSFX(WAV_ROOT"PickupHealth.wav");
    g_powerupSounds[POWERUP_TYPE_TREASURE_CUP] = AudioCreateSFX(WAV_ROOT"TreasureCup.wav");
    g_powerupSounds[POWERUP_TYPE_TREASURE_CROSS] = AudioCreateSFX(WAV_ROOT"TreasureCross.wav");
    g_powerupSounds[POWERUP_TYPE_TREASURE_CHEST] = AudioCreateSFX(WAV_ROOT"TreasureChest.wav");

    // Adding keys separately because they are not in the powerup range (special case)
    g_powerupSprites[POWERUP_TYPE_GOLD_KEY - 1] = W3DCreateSprite(OBJECT_GOLDKEY, FALSE);
    g_powerupSprites[POWERUP_TYPE_SILVER_KEY - 1] = W3DCreateSprite(OBJECT_SILVERKEY, FALSE);
}

VOID PowerupShutdown()
{
    USHORT i = 0;
    for (; i < 20; ++i)
    {
        if (W3DIsValidSprite(g_powerupSprites[i]))
        {
            W3DDestroySprite(g_powerupSprites[i]);
            g_powerupSprites[i] = INVALID_HSPRITE_VALUE;
        }
    }
}

USHORT PowerupCreate(VEC2 position, BYTE type)
{
    USHORT index = g_numPowerups;

    if (g_numPowerups >= MAX_POWERUPS)
    {
        return INVALID_ENTITY;
    }

    g_powerups[index].type = type;
    switch (type)
    {
    case POWERUP_TYPE_GOLD_KEY:
        g_powerups[index].position = position;
        g_powerups[index].value = 0;
        break;
    case POWERUP_TYPE_SILVER_KEY:
        g_powerups[index].position = position;
        g_powerups[index].value = 0;
        break;
    case POWERUP_TYPE_FOOD:
        g_powerups[index].position = position;
        g_powerups[index].value = 10;
        break;
    case POWERUP_TYPE_HEALTHKIT:
        g_powerups[index].position = position;
        g_powerups[index].value = 25;
        break;
    case POWERUP_TYPE_AMMO:
        g_powerups[index].position = position;
        g_powerups[index].value = 8;
        break;
    case POWERUP_TYPE_MACHINEGUN:
        g_powerups[index].position = position;
        g_powerups[index].value = WEAPON_MACHINEGUN;
        break;
    case POWERUP_TYPE_CHAINGUN:
        g_powerups[index].position = position;
        g_powerups[index].value = WEAPON_CHAINGUN;
        break;
    case POWERUP_TYPE_TREASURE_CROSS:
        g_powerups[index].position = position;
        g_powerups[index].value = 100;
        break;
    case POWERUP_TYPE_TREASURE_CUP:
        g_powerups[index].position = position;
        g_powerups[index].value = 500;
        break;
    case POWERUP_TYPE_TREASURE_CHEST:
        g_powerups[index].position = position;
        g_powerups[index].value = 1000;
        break;
    case POWERUP_TYPE_TREASURE_CROWN:
        g_powerups[index].position = position;
        g_powerups[index].value = 5000;
        break;
    case POWERUP_TYPE_TREASURE_1UP:
        g_powerups[index].position = position;
        g_powerups[index].value = 99;
        break;

    default:
        return INVALID_ENTITY;
    }

    ++g_numPowerups;
    return index;
}

HSOUND PowerupGetSound(USHORT powerupIndex)
{
    if (powerupIndex == INVALID_ENTITY || powerupIndex >= g_numPowerups || g_powerups[powerupIndex].type == POWERUP_TYPE_NONE)
    {
        return INVALID_HSPRITE_VALUE;
    }

    return g_powerupSounds[g_powerups[powerupIndex].type];
}

HSPRITE PowerupGetSprite(USHORT powerupIndex)
{
    BYTE i  = 0;
    if (powerupIndex == INVALID_ENTITY || powerupIndex >= g_numPowerups || g_powerups[powerupIndex].type == POWERUP_TYPE_NONE)
    {
        return INVALID_HSPRITE_VALUE;
    }

    return g_powerupSprites[g_powerups[powerupIndex].type - 1];
}

VEC2 PowerupGetPosition(USHORT powerupIndex)
{
    VEC2 position = { 0, 0 };

    if (powerupIndex != INVALID_ENTITY && powerupIndex < g_numPowerups)
    {
        position = g_powerups[powerupIndex].position;
    }

    return position;
}

VOID PowerupGetInfo(USHORT powerupIndex, BYTE* type, USHORT* value)
{
    if (powerupIndex == INVALID_ENTITY || powerupIndex >= g_numPowerups)
    {
        *type = POWERUP_TYPE_NONE;
        *value = 0;
    }

    *type = g_powerups[powerupIndex].type;
    *value = g_powerups[powerupIndex].value;
}

BOOL PowerupIsAmmo(BYTE type)
{
    return type == POWERUP_TYPE_AMMO;
}

BOOL PowerupIsHealth(BYTE type)
{
    return type == POWERUP_TYPE_FOOD ||
           type == POWERUP_TYPE_HEALTHKIT;
}

BOOL PowerupIsGun(BYTE type)
{
    return type == POWERUP_TYPE_MACHINEGUN ||
           type == POWERUP_TYPE_CHAINGUN;
}

BOOL PowerupIsTreasure(BYTE type)
{
    return type >= POWERUP_TYPE_TREASURE_CROSS &&
            type <= POWERUP_TYPE_TREASURE_1UP;
}

BOOL PowerupIsKey(BYTE type)
{
    return type == POWERUP_TYPE_GOLD_KEY ||
           type == POWERUP_TYPE_SILVER_KEY;
}
