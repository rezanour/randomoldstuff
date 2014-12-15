#include "common.h"

/* Imported map layer data */
#pragma pack(push,1)

typedef struct _WOLF3D_LAYERDATA 
{
    USHORT layer1;
    USHORT layer2;
    USHORT layer3;
}WOLF3D_LAYERDATA,*PWOLF3D_LAYERDATA;

#pragma pack(pop)

typedef struct
{
    BYTE Texture;
    VEC2 Position;
    VEC2 EndPosition;
} MOVING_WALL, *PMOVING_WALL;

static MOVING_WALL g_Secret = {0};

/* Map data */
HALLOC g_mapAlloc = NULL;
MAPCELL* g_map = NULL;
USHORT g_mapWidth = 0;
USHORT g_mapHeight = 0;

extern BOOL g_gameInProgress;

/* Textures used throughout the level */
HSPRITE g_wallTextures[MAX_WALL_TEXTURES] = { INVALID_HSPRITE_VALUE };
HSPRITE g_doorTextures[MAX_DOOR_TEXTURES] = { INVALID_HSPRITE_VALUE };
HSPRITE g_doorJamTextures[MAX_DOOR_TEXTURES] = { INVALID_HSPRITE_VALUE };
HSPRITE g_decorSprites[MAX_DECOR_SPRITES] = { INVALID_HSPRITE_VALUE };

/* TODO: Remove these for the real thing later */
HSOUND g_openDoorSound = INVALID_HSOUND_VALUE;
HSOUND g_closeDoorSound = INVALID_HSOUND_VALUE;
HSOUND g_openSecretWallSound = INVALID_HSOUND_VALUE;

/* Cache the indices of doors in the map, since we need to update them regularly */
USHORT g_doorCells[MAX_DOORS] = {0};
USHORT g_numDoors = 0;

HSOUND g_backgroundMusic = INVALID_HSOUND_VALUE;

/**************************************************************
    Internal helper method declarations
***************************************************************/

/* General cell queries */
USHORT RawWolfWallValueToId(USHORT i);
USHORT GetCell(SHORT x, SHORT y);
USHORT GetCellX(USHORT cellIndex);
USHORT GetCellY(USHORT cellIndex);
VEC2 GetCellPosition(USHORT cellIndex);

/* Cell contents queries */
BOOL CellContainsEntities(USHORT cellIndex);
BOOL CellContainsEnemies(USHORT cellIndex);
BOOL CellContainsPowerups(USHORT cellIndex);
BOOL CellContainsDecorSprites(USHORT cellIndex);

/* Image related */
VOID LoadMapImages();
VOID FreeMapImages();

/* Collision related */
BOOL IsCellOccupied(SHORT x, SHORT y);
VEC2 CollideWith(VEC2 position, SHORT cellX, SHORT cellY);
BOOL IsWithin(float v, BYTE start, BYTE end);
BYTE RayCastOutwardFromCell(VEC2* position, VEC2 direction, SHORT cellX, SHORT cellY);

/* Building out map cells */
VOID CreateWall(USHORT index, USHORT wallIndex);
VOID CreateDoor(USHORT index, BOOL directionWE, BYTE initialState, BYTE texture, BYTE lockType);
VOID CreateEnemy(USHORT index, BYTE direction, BYTE enemyType, BYTE initialState, BYTE difficulty);
VOID CreatePowerup(USHORT index, BYTE powerupType);
VOID CreateDecorSprite(USHORT index, USHORT sprite);
VOID CreatePatrol(USHORT index, float x, float y);
VOID CreatePushWall(USHORT index);

/* Doors */
VOID UpdateDoors();
VOID UpdateDoor(USHORT doorIndex);

HSPRITE DebugGetWallTexture(USHORT id)
{
    return g_wallTextures[id];
}

/**************************************************************
    Public Level API
***************************************************************/
VOID LevelInit()
{
    SHORT i;
    
    for (i = 0; i < ARRAYSIZE(g_wallTextures); ++i)
    {
        g_wallTextures[i] = INVALID_HSPRITE_VALUE;
    }
    for (i = 0; i < ARRAYSIZE(g_doorTextures); ++i)
    {
        g_doorTextures[i] = INVALID_HSPRITE_VALUE;
    }
    for (i = 0; i < ARRAYSIZE(g_doorJamTextures); ++i)
    {
        g_doorJamTextures[i] = INVALID_HSPRITE_VALUE;
    }
    for (i = 0; i < ARRAYSIZE(g_decorSprites); ++i)
    {
        g_decorSprites[i] = INVALID_HSPRITE_VALUE;
    }
}

BOOL LevelCreate(BYTE levelNumber)
{
    /* TODO: Read these values from data */
    USHORT width = 64;
    USHORT height = 64;
    VEC2 direction = {0,0};

    FILE* mapFile = NULL;
    WOLF3D_LAYERDATA layerData = {0};
    USHORT* layer1Data = NULL;
    USHORT wallId = 0;

    USHORT i, j;

    /* Ensure data is cleared */
    LevelDestroy();

    /* Create map resources */
    g_mapWidth = width;
    g_mapHeight = height;

    g_mapAlloc = MemAlloc(RESOURCE_TYPE_MAP, width * height * sizeof(MAPCELL));
    g_map = (MAPCELL*)MemLock(g_mapAlloc);
    if (!g_map)
    {
        return FALSE;
    }

    ZeroMemory(g_map, g_mapWidth * g_mapHeight * sizeof(MAPCELL));

    for (i = 0; i < g_mapWidth * g_mapHeight; ++i)
    {
        for (j = 0; j < ARRAYSIZE(g_map[i].Enemies); ++j)
        {
            g_map[i].Enemies[j] = INVALID_ENTITY;
        }

        for (j = 0; j < ARRAYSIZE(g_map[i].Powerups); ++j)
        {
            g_map[i].Powerups[j] = INVALID_ENTITY;
        }

        for (j = 0; j < ARRAYSIZE(g_map[i].DecorSprites); ++j)
        {
            g_map[i].DecorSprites[j] = INVALID_INDEX;
        }
    }

    /* Load map textures */
    LoadMapImages();

    /* Fill map with data */
    GetLayerData(levelNumber + 1, &layer1Data);
    if (layer1Data)
    {
        for (i = 0; i < g_mapWidth * g_mapHeight; ++i)
        {
            if (i == 842)
            {
                i = i;
            }

            wallId = RawWolfWallValueToId(layer1Data[i]);
            if (wallId > 0 && wallId < MAX_WALL_SPRITES)
            {
                CreateWall(i, wallId);
            }

            if((layer1Data[i] >= WALL_DOOR_NS && layer1Data[i] <= WALL_DOOR_SILVERKEY_WE) || layer1Data[i] == WALL_DOOR_ELEVATOR_NS || layer1Data[i] == WALL_DOOR_ELEVATOR_WE )
            {
                // door
                switch(layer1Data[i])
                {
                    case WALL_DOOR_NS:
                        CreateDoor(i, FALSE, DOOR_STATE_CLOSED, 0, DOOR_LOCKED_NONE);
                        break;
                    case WALL_DOOR_ELEVATOR_NS:
                        CreateDoor(i, FALSE, DOOR_STATE_CLOSED, 1, DOOR_LOCKED_NONE);
                        break;
                    case WALL_DOOR_GOLDKEY_NS:
                        CreateDoor(i, FALSE, DOOR_STATE_CLOSED, 2, DOOR_LOCKED_GOLD);
                        break;
                    case WALL_DOOR_SILVERKEY_NS:
                        CreateDoor(i, FALSE, DOOR_STATE_CLOSED, 3, DOOR_LOCKED_SILVER);
                        break;
                    case WALL_DOOR_WE:
                        CreateDoor(i, TRUE, DOOR_STATE_CLOSED, 0, DOOR_LOCKED_NONE);
                        break;
                    case WALL_DOOR_ELEVATOR_WE:
                        CreateDoor(i, TRUE, DOOR_STATE_CLOSED, 1, DOOR_LOCKED_NONE);
                        break;
                    case WALL_DOOR_GOLDKEY_WE:
                        CreateDoor(i, TRUE, DOOR_STATE_CLOSED, 2, DOOR_LOCKED_GOLD);
                        break;
                    case WALL_DOOR_SILVERKEY_WE:
                        CreateDoor(i, TRUE, DOOR_STATE_CLOSED, 3, DOOR_LOCKED_SILVER);
                        break;
                }
            }
        }

        free(layer1Data);
    }

    GetLayerDataEx(2, levelNumber + 1, &layer1Data);
    if (layer1Data)
    {
        SHORT index;

        for (i = 0; i < g_mapWidth * g_mapHeight; ++i)
        {
            if (i == 842)
            {
                i = i;
            }

            if (layer1Data[i] != 0)
            {
                switch(layer1Data[i])
                {
                case PLAYER_START_N:
                    g_player.Direction.x = 0;
                    g_player.Direction.y = -1;
                    PlayerSetPosition(GetCellX(i), GetCellY(i));
                    break;
                case PLAYER_START_E:
                    g_player.Direction.x = 1;
                    g_player.Direction.y = 0;
                    PlayerSetPosition(GetCellX(i), GetCellY(i));
                    break;
                case PLAYER_START_S:
                    g_player.Direction.x = 0;
                    g_player.Direction.y = 1;
                    PlayerSetPosition(GetCellX(i), GetCellY(i));
                    break;
                case PLAYER_START_W:
                    g_player.Direction.x = -1;
                    g_player.Direction.y = 0;
                    PlayerSetPosition(GetCellX(i), GetCellY(i));
                    break;
                case ENEMY_GUARD_DEAD:
                    CreateEnemy(i, DIR_DONTCARE, ENEMY_TYPE_SOLDIER, ENEMY_STATE_DEAD, DIFFICULTY_EASY);
                    break;
                
                case BOSS_EPISODE_1:
                    CreateEnemy(i, DIR_SOUTH, ENEMY_TYPE_BOSS, ENEMY_STATE_IDLE, DIFFICULTY_EASY);
                    break;

                case PATROL_DIR_CHANGE_EAST:
                    CreatePatrol(i, 1, 0);
                    break;
                case PATROL_DIR_CHANGE_NORTHEAST:
                    CreatePatrol(i, 1, -1);
                    break;
                case PATROL_DIR_CHANGE_NORTH:
                    CreatePatrol(i, 0, -1);
                    break;
                case PATROL_DIR_CHANGE_NORTHWEST:
                    CreatePatrol(i, -1, -1);
                    break;
                case PATROL_DIR_CHANGE_WEST:
                    CreatePatrol(i, -1, 0);
                    break;
                case PATROL_DIR_CHANGE_SOUTHWEST:
                    CreatePatrol(i, -1, 1);
                    break;
                case PATROL_DIR_CHANGE_SOUTH:
                    CreatePatrol(i, 0, 1);
                    break;
                case PATROL_DIR_CHANGE_SOUTHEAST:
                    CreatePatrol(i, 1, 1);
                    break;

                case PUSHWALL_SECRET:
                    // Push wall detected
                    CreatePushWall(i);
                    break;

                case EASY_STANDING_GUARD_N:
                    CreateEnemy(i, DIR_NORTH, ENEMY_TYPE_SOLDIER, ENEMY_STATE_IDLE, DIFFICULTY_EASY);
                    break;
                case MEDIUM_STANDING_GUARD_N:
                    CreateEnemy(i, DIR_NORTH, ENEMY_TYPE_SOLDIER, ENEMY_STATE_IDLE, DIFFICULTY_MEDIUM);
                    break;
                case HARD_STANDING_GUARD_N:
                    CreateEnemy(i, DIR_NORTH, ENEMY_TYPE_SOLDIER, ENEMY_STATE_IDLE, DIFFICULTY_HARD);
                    break;
                case EASY_PATROL_GUARD_N:
                    CreateEnemy(i, DIR_NORTH, ENEMY_TYPE_SOLDIER, ENEMY_STATE_PATROL, DIFFICULTY_EASY);
                    break;
                case MEDIUM_PATROL_GUARD_N:
                    CreateEnemy(i, DIR_NORTH, ENEMY_TYPE_SOLDIER, ENEMY_STATE_PATROL, DIFFICULTY_MEDIUM);
                    break;
                case HARD_PATROL_GUARD_N:
                    CreateEnemy(i, DIR_NORTH, ENEMY_TYPE_SOLDIER, ENEMY_STATE_PATROL, DIFFICULTY_HARD);
                    break;
                case EASY_STANDING_GUARD_E:
                    CreateEnemy(i, DIR_EAST, ENEMY_TYPE_SOLDIER, ENEMY_STATE_IDLE, DIFFICULTY_EASY);
                    break;
                case MEDIUM_STANDING_GUARD_E:
                    CreateEnemy(i, DIR_EAST, ENEMY_TYPE_SOLDIER, ENEMY_STATE_IDLE, DIFFICULTY_MEDIUM);
                    break;
                case HARD_STANDING_GUARD_E:
                    CreateEnemy(i, DIR_EAST, ENEMY_TYPE_SOLDIER, ENEMY_STATE_IDLE, DIFFICULTY_HARD);
                    break;
                case EASY_PATROL_GUARD_E:
                    CreateEnemy(i, DIR_EAST, ENEMY_TYPE_SOLDIER, ENEMY_STATE_PATROL, DIFFICULTY_EASY);
                    break;
                case MEDIUM_PATROL_GUARD_E:
                    CreateEnemy(i, DIR_EAST, ENEMY_TYPE_SOLDIER, ENEMY_STATE_PATROL, DIFFICULTY_MEDIUM);
                    break;
                case HARD_PATROL_GUARD_E:
                    CreateEnemy(i, DIR_EAST, ENEMY_TYPE_SOLDIER, ENEMY_STATE_PATROL, DIFFICULTY_HARD);
                    break;
                case EASY_STANDING_GUARD_S:
                    CreateEnemy(i, DIR_SOUTH, ENEMY_TYPE_SOLDIER, ENEMY_STATE_IDLE, DIFFICULTY_EASY);
                    break;
                case MEDIUM_STANDING_GUARD_S:
                    CreateEnemy(i, DIR_SOUTH, ENEMY_TYPE_SOLDIER, ENEMY_STATE_IDLE, DIFFICULTY_MEDIUM);
                    break;
                case HARD_STANDING_GUARD_S:
                    CreateEnemy(i, DIR_SOUTH, ENEMY_TYPE_SOLDIER, ENEMY_STATE_IDLE, DIFFICULTY_HARD);
                    break;
                case EASY_PATROL_GUARD_S:
                    CreateEnemy(i, DIR_SOUTH, ENEMY_TYPE_SOLDIER, ENEMY_STATE_PATROL, DIFFICULTY_EASY);
                    break;
                case MEDIUM_PATROL_GUARD_S:
                    CreateEnemy(i, DIR_SOUTH, ENEMY_TYPE_SOLDIER, ENEMY_STATE_PATROL, DIFFICULTY_MEDIUM);
                    break;
                case HARD_PATROL_GUARD_S:
                    CreateEnemy(i, DIR_SOUTH, ENEMY_TYPE_SOLDIER, ENEMY_STATE_PATROL, DIFFICULTY_HARD);
                    break;
                case EASY_STANDING_GUARD_W:
                    CreateEnemy(i, DIR_WEST, ENEMY_TYPE_SOLDIER, ENEMY_STATE_IDLE, DIFFICULTY_EASY);
                    break;
                case MEDIUM_STANDING_GUARD_W:
                    CreateEnemy(i, DIR_WEST, ENEMY_TYPE_SOLDIER, ENEMY_STATE_IDLE, DIFFICULTY_MEDIUM);
                    break;
                case HARD_STANDING_GUARD_W:
                    CreateEnemy(i, DIR_WEST, ENEMY_TYPE_SOLDIER, ENEMY_STATE_IDLE, DIFFICULTY_HARD);
                    break;
                case EASY_PATROL_GUARD_W:
                    CreateEnemy(i, DIR_WEST, ENEMY_TYPE_SOLDIER, ENEMY_STATE_PATROL, DIFFICULTY_EASY);
                    break;
                case MEDIUM_PATROL_GUARD_W:
                    CreateEnemy(i, DIR_WEST, ENEMY_TYPE_SOLDIER, ENEMY_STATE_PATROL, DIFFICULTY_MEDIUM);
                    break;
                case HARD_PATROL_GUARD_W:
                    CreateEnemy(i, DIR_WEST, ENEMY_TYPE_SOLDIER, ENEMY_STATE_PATROL, DIFFICULTY_HARD);
                    break;
                
                case EASY_STANDING_SS_N:
                    CreateEnemy(i, DIR_NORTH, ENEMY_TYPE_BLUEDUDE, ENEMY_STATE_IDLE, DIFFICULTY_EASY);
                    break;
                case MEDIUM_STANDING_SS_N:
                    CreateEnemy(i, DIR_NORTH, ENEMY_TYPE_BLUEDUDE, ENEMY_STATE_IDLE, DIFFICULTY_MEDIUM);
                    break;
                case HARD_STANDING_SS_N:
                    CreateEnemy(i, DIR_NORTH, ENEMY_TYPE_BLUEDUDE, ENEMY_STATE_IDLE, DIFFICULTY_HARD);
                    break;
                case EASY_PATROL_SS_N:
                    CreateEnemy(i, DIR_NORTH, ENEMY_TYPE_BLUEDUDE, ENEMY_STATE_PATROL, DIFFICULTY_EASY);
                    break;
                case MEDIUM_PATROL_SS_N:
                    CreateEnemy(i, DIR_NORTH, ENEMY_TYPE_BLUEDUDE, ENEMY_STATE_PATROL, DIFFICULTY_MEDIUM);
                    break;
                case HARD_PATROL_SS_N:
                    CreateEnemy(i, DIR_NORTH, ENEMY_TYPE_BLUEDUDE, ENEMY_STATE_PATROL, DIFFICULTY_HARD);
                    break;
                case EASY_STANDING_SS_E:
                    CreateEnemy(i, DIR_EAST, ENEMY_TYPE_BLUEDUDE, ENEMY_STATE_IDLE, DIFFICULTY_EASY);
                    break;
                case MEDIUM_STANDING_SS_E:
                    CreateEnemy(i, DIR_EAST, ENEMY_TYPE_BLUEDUDE, ENEMY_STATE_IDLE, DIFFICULTY_MEDIUM);
                    break;
                case HARD_STANDING_SS_E:
                    CreateEnemy(i, DIR_EAST, ENEMY_TYPE_BLUEDUDE, ENEMY_STATE_IDLE, DIFFICULTY_HARD);
                    break;
                case EASY_PATROL_SS_E:
                    CreateEnemy(i, DIR_EAST, ENEMY_TYPE_BLUEDUDE, ENEMY_STATE_PATROL, DIFFICULTY_EASY);
                    break;
                case MEDIUM_PATROL_SS_E:
                    CreateEnemy(i, DIR_EAST, ENEMY_TYPE_BLUEDUDE, ENEMY_STATE_PATROL, DIFFICULTY_MEDIUM);
                    break;
                case HARD_PATROL_SS_E:
                    CreateEnemy(i, DIR_EAST, ENEMY_TYPE_BLUEDUDE, ENEMY_STATE_PATROL, DIFFICULTY_HARD);
                    break;
                case EASY_STANDING_SS_S:
                    CreateEnemy(i, DIR_SOUTH, ENEMY_TYPE_BLUEDUDE, ENEMY_STATE_IDLE, DIFFICULTY_EASY);
                    break;
                case MEDIUM_STANDING_SS_S:
                    CreateEnemy(i, DIR_SOUTH, ENEMY_TYPE_BLUEDUDE, ENEMY_STATE_IDLE, DIFFICULTY_MEDIUM);
                    break;
                case HARD_STANDING_SS_S:
                    CreateEnemy(i, DIR_SOUTH, ENEMY_TYPE_BLUEDUDE, ENEMY_STATE_IDLE, DIFFICULTY_HARD);
                    break;
                case EASY_PATROL_SS_S:
                    CreateEnemy(i, DIR_SOUTH, ENEMY_TYPE_BLUEDUDE, ENEMY_STATE_PATROL, DIFFICULTY_EASY);
                    break;
                case MEDIUM_PATROL_SS_S:
                    CreateEnemy(i, DIR_SOUTH, ENEMY_TYPE_BLUEDUDE, ENEMY_STATE_PATROL, DIFFICULTY_MEDIUM);
                    break;
                case HARD_PATROL_SS_S:
                    CreateEnemy(i, DIR_SOUTH, ENEMY_TYPE_BLUEDUDE, ENEMY_STATE_PATROL, DIFFICULTY_HARD);
                    break;
                case EASY_STANDING_SS_W:
                    CreateEnemy(i, DIR_WEST, ENEMY_TYPE_BLUEDUDE, ENEMY_STATE_IDLE, DIFFICULTY_EASY);
                    break;
                case MEDIUM_STANDING_SS_W:
                    CreateEnemy(i, DIR_WEST, ENEMY_TYPE_BLUEDUDE, ENEMY_STATE_IDLE, DIFFICULTY_MEDIUM);
                    break;
                case HARD_STANDING_SS_W:
                    CreateEnemy(i, DIR_WEST, ENEMY_TYPE_BLUEDUDE, ENEMY_STATE_IDLE, DIFFICULTY_HARD);
                    break;
                case EASY_PATROL_SS_W:
                    CreateEnemy(i, DIR_WEST, ENEMY_TYPE_BLUEDUDE, ENEMY_STATE_PATROL, DIFFICULTY_EASY);
                    break;
                case MEDIUM_PATROL_SS_W:
                    CreateEnemy(i, DIR_WEST, ENEMY_TYPE_BLUEDUDE, ENEMY_STATE_PATROL, DIFFICULTY_MEDIUM);
                    break;
                case HARD_PATROL_SS_W:
                    CreateEnemy(i, DIR_WEST, ENEMY_TYPE_BLUEDUDE, ENEMY_STATE_PATROL, DIFFICULTY_HARD);
                    break;

                case EASY_STANDING_DOG_N:
                    CreateEnemy(i, DIR_NORTH, ENEMY_TYPE_DOG, ENEMY_STATE_IDLE, DIFFICULTY_EASY);
                    break;
                case MEDIUM_STANDING_DOG_N:
                    CreateEnemy(i, DIR_NORTH, ENEMY_TYPE_DOG, ENEMY_STATE_IDLE, DIFFICULTY_MEDIUM);
                    break;
                case HARD_STANDING_DOG_N:
                    CreateEnemy(i, DIR_NORTH, ENEMY_TYPE_DOG, ENEMY_STATE_IDLE, DIFFICULTY_HARD);
                    break;
                case EASY_PATROL_DOG_N:
                    CreateEnemy(i, DIR_NORTH, ENEMY_TYPE_DOG, ENEMY_STATE_PATROL, DIFFICULTY_EASY);
                    break;
                case MEDIUM_PATROL_DOG_N:
                    CreateEnemy(i, DIR_NORTH, ENEMY_TYPE_DOG, ENEMY_STATE_PATROL, DIFFICULTY_MEDIUM);
                    break;
                case HARD_PATROL_DOG_N:
                    CreateEnemy(i, DIR_NORTH, ENEMY_TYPE_DOG, ENEMY_STATE_PATROL, DIFFICULTY_HARD);
                    break;
                case EASY_STANDING_DOG_E:
                    CreateEnemy(i, DIR_EAST, ENEMY_TYPE_DOG, ENEMY_STATE_IDLE, DIFFICULTY_EASY);
                    break;
                case MEDIUM_STANDING_DOG_E:
                    CreateEnemy(i, DIR_EAST, ENEMY_TYPE_DOG, ENEMY_STATE_IDLE, DIFFICULTY_MEDIUM);
                    break;
                case HARD_STANDING_DOG_E:
                    CreateEnemy(i, DIR_EAST, ENEMY_TYPE_DOG, ENEMY_STATE_IDLE, DIFFICULTY_HARD);
                    break;
                case EASY_PATROL_DOG_E:
                    CreateEnemy(i, DIR_EAST, ENEMY_TYPE_DOG, ENEMY_STATE_PATROL, DIFFICULTY_EASY);
                    break;
                case MEDIUM_PATROL_DOG_E:
                    CreateEnemy(i, DIR_EAST, ENEMY_TYPE_DOG, ENEMY_STATE_PATROL, DIFFICULTY_MEDIUM);
                    break;
                case HARD_PATROL_DOG_E:
                    CreateEnemy(i, DIR_EAST, ENEMY_TYPE_DOG, ENEMY_STATE_PATROL, DIFFICULTY_HARD);
                    break;
                case EASY_STANDING_DOG_S:
                    CreateEnemy(i, DIR_SOUTH, ENEMY_TYPE_DOG, ENEMY_STATE_IDLE, DIFFICULTY_EASY);
                    break;
                case MEDIUM_STANDING_DOG_S:
                    CreateEnemy(i, DIR_SOUTH, ENEMY_TYPE_DOG, ENEMY_STATE_IDLE, DIFFICULTY_MEDIUM);
                    break;
                case HARD_STANDING_DOG_S:
                    CreateEnemy(i, DIR_SOUTH, ENEMY_TYPE_DOG, ENEMY_STATE_IDLE, DIFFICULTY_HARD);
                    break;
                case EASY_PATROL_DOG_S:
                    CreateEnemy(i, DIR_SOUTH, ENEMY_TYPE_DOG, ENEMY_STATE_PATROL, DIFFICULTY_EASY);
                    break;
                case MEDIUM_PATROL_DOG_S:
                    CreateEnemy(i, DIR_SOUTH, ENEMY_TYPE_DOG, ENEMY_STATE_PATROL, DIFFICULTY_MEDIUM);
                    break;
                case HARD_PATROL_DOG_S:
                    CreateEnemy(i, DIR_SOUTH, ENEMY_TYPE_DOG, ENEMY_STATE_PATROL, DIFFICULTY_HARD);
                    break;
                case EASY_STANDING_DOG_W:
                    CreateEnemy(i, DIR_WEST, ENEMY_TYPE_DOG, ENEMY_STATE_IDLE, DIFFICULTY_EASY);
                    break;
                case MEDIUM_STANDING_DOG_W:
                    CreateEnemy(i, DIR_WEST, ENEMY_TYPE_DOG, ENEMY_STATE_IDLE, DIFFICULTY_MEDIUM);
                    break;
                case HARD_STANDING_DOG_W:
                    CreateEnemy(i, DIR_WEST, ENEMY_TYPE_DOG, ENEMY_STATE_IDLE, DIFFICULTY_HARD);
                    break;
                case EASY_PATROL_DOG_W:
                    CreateEnemy(i, DIR_WEST, ENEMY_TYPE_DOG, ENEMY_STATE_PATROL, DIFFICULTY_EASY);
                    break;
                case MEDIUM_PATROL_DOG_W:
                    CreateEnemy(i, DIR_WEST, ENEMY_TYPE_DOG, ENEMY_STATE_PATROL, DIFFICULTY_MEDIUM);
                    break;
                case HARD_PATROL_DOG_W:
                    CreateEnemy(i, DIR_WEST, ENEMY_TYPE_DOG, ENEMY_STATE_PATROL, DIFFICULTY_HARD);
                    break;


                case EASY_STANDING_ZOMBIE_N:
                    CreateEnemy(i, DIR_NORTH, ENEMY_TYPE_ZOMBIE, ENEMY_STATE_IDLE, DIFFICULTY_EASY);
                    break;
                case MEDIUM_STANDING_ZOMBIE_N:
                    CreateEnemy(i, DIR_NORTH, ENEMY_TYPE_ZOMBIE, ENEMY_STATE_IDLE, DIFFICULTY_MEDIUM);
                    break;
                case HARD_STANDING_ZOMBIE_N:
                    CreateEnemy(i, DIR_NORTH, ENEMY_TYPE_ZOMBIE, ENEMY_STATE_IDLE, DIFFICULTY_HARD);
                    break;
                case EASY_PATROL_ZOMBIE_N:
                    CreateEnemy(i, DIR_NORTH, ENEMY_TYPE_ZOMBIE, ENEMY_STATE_PATROL, DIFFICULTY_EASY);
                    break;
                case MEDIUM_PATROL_ZOMBIE_N:
                    CreateEnemy(i, DIR_NORTH, ENEMY_TYPE_ZOMBIE, ENEMY_STATE_PATROL, DIFFICULTY_MEDIUM);
                    break;
                case HARD_PATROL_ZOMBIE_N:
                    CreateEnemy(i, DIR_NORTH, ENEMY_TYPE_ZOMBIE, ENEMY_STATE_PATROL, DIFFICULTY_HARD);
                    break;
                case EASY_STANDING_ZOMBIE_E:
                    CreateEnemy(i, DIR_EAST, ENEMY_TYPE_ZOMBIE, ENEMY_STATE_IDLE, DIFFICULTY_EASY);
                    break;
                case MEDIUM_STANDING_ZOMBIE_E:
                    CreateEnemy(i, DIR_EAST, ENEMY_TYPE_ZOMBIE, ENEMY_STATE_IDLE, DIFFICULTY_MEDIUM);
                    break;
                case HARD_STANDING_ZOMBIE_E:
                    CreateEnemy(i, DIR_EAST, ENEMY_TYPE_ZOMBIE, ENEMY_STATE_IDLE, DIFFICULTY_HARD);
                    break;
                case EASY_PATROL_ZOMBIE_E:
                    CreateEnemy(i, DIR_EAST, ENEMY_TYPE_ZOMBIE, ENEMY_STATE_PATROL, DIFFICULTY_EASY);
                    break;
                case MEDIUM_PATROL_ZOMBIE_E:
                    CreateEnemy(i, DIR_EAST, ENEMY_TYPE_ZOMBIE, ENEMY_STATE_PATROL, DIFFICULTY_MEDIUM);
                    break;
                case HARD_PATROL_ZOMBIE_E:
                    CreateEnemy(i, DIR_EAST, ENEMY_TYPE_ZOMBIE, ENEMY_STATE_PATROL, DIFFICULTY_HARD);
                    break;
                case EASY_STANDING_ZOMBIE_S:
                    CreateEnemy(i, DIR_SOUTH, ENEMY_TYPE_ZOMBIE, ENEMY_STATE_IDLE, DIFFICULTY_EASY);
                    break;
                case MEDIUM_STANDING_ZOMBIE_S:
                    CreateEnemy(i, DIR_SOUTH, ENEMY_TYPE_ZOMBIE, ENEMY_STATE_IDLE, DIFFICULTY_MEDIUM);
                    break;
                case HARD_STANDING_ZOMBIE_S:
                    CreateEnemy(i, DIR_SOUTH, ENEMY_TYPE_ZOMBIE, ENEMY_STATE_IDLE, DIFFICULTY_HARD);
                    break;
                case EASY_PATROL_ZOMBIE_S:
                    CreateEnemy(i, DIR_SOUTH, ENEMY_TYPE_ZOMBIE, ENEMY_STATE_PATROL, DIFFICULTY_EASY);
                    break;
                case MEDIUM_PATROL_ZOMBIE_S:
                    CreateEnemy(i, DIR_SOUTH, ENEMY_TYPE_ZOMBIE, ENEMY_STATE_PATROL, DIFFICULTY_MEDIUM);
                    break;
                case HARD_PATROL_ZOMBIE_S:
                    CreateEnemy(i, DIR_SOUTH, ENEMY_TYPE_ZOMBIE, ENEMY_STATE_PATROL, DIFFICULTY_HARD);
                    break;
                case EASY_STANDING_ZOMBIE_W:
                    CreateEnemy(i, DIR_WEST, ENEMY_TYPE_ZOMBIE, ENEMY_STATE_IDLE, DIFFICULTY_EASY);
                    break;
                case MEDIUM_STANDING_ZOMBIE_W:
                    CreateEnemy(i, DIR_WEST, ENEMY_TYPE_ZOMBIE, ENEMY_STATE_IDLE, DIFFICULTY_MEDIUM);
                    break;
                case HARD_STANDING_ZOMBIE_W:
                    CreateEnemy(i, DIR_WEST, ENEMY_TYPE_ZOMBIE, ENEMY_STATE_IDLE, DIFFICULTY_HARD);
                    break;
                case EASY_PATROL_ZOMBIE_W:
                    CreateEnemy(i, DIR_WEST, ENEMY_TYPE_ZOMBIE, ENEMY_STATE_PATROL, DIFFICULTY_EASY);
                    break;
                case MEDIUM_PATROL_ZOMBIE_W:
                    CreateEnemy(i, DIR_WEST, ENEMY_TYPE_ZOMBIE, ENEMY_STATE_PATROL, DIFFICULTY_MEDIUM);
                    break;
                case HARD_PATROL_ZOMBIE_W:
                    CreateEnemy(i, DIR_WEST, ENEMY_TYPE_ZOMBIE, ENEMY_STATE_PATROL, DIFFICULTY_HARD);
                    break;


                case EASY_STANDING_OFFICER_N:
                    CreateEnemy(i, DIR_NORTH, ENEMY_TYPE_OFFICER, ENEMY_STATE_IDLE, DIFFICULTY_EASY);
                    break;
                case MEDIUM_STANDING_OFFICER_N:
                    CreateEnemy(i, DIR_NORTH, ENEMY_TYPE_OFFICER, ENEMY_STATE_IDLE, DIFFICULTY_MEDIUM);
                    break;
                case HARD_STANDING_OFFICER_N:
                    CreateEnemy(i, DIR_NORTH, ENEMY_TYPE_OFFICER, ENEMY_STATE_IDLE, DIFFICULTY_HARD);
                    break;
                case EASY_PATROL_OFFICER_N:
                    CreateEnemy(i, DIR_NORTH, ENEMY_TYPE_OFFICER, ENEMY_STATE_PATROL, DIFFICULTY_EASY);
                    break;
                case MEDIUM_PATROL_OFFICER_N:
                    CreateEnemy(i, DIR_NORTH, ENEMY_TYPE_OFFICER, ENEMY_STATE_PATROL, DIFFICULTY_MEDIUM);
                    break;
                case HARD_PATROL_OFFICER_N:
                    CreateEnemy(i, DIR_NORTH, ENEMY_TYPE_OFFICER, ENEMY_STATE_PATROL, DIFFICULTY_HARD);
                    break;
                case EASY_STANDING_OFFICER_E:
                    CreateEnemy(i, DIR_EAST, ENEMY_TYPE_OFFICER, ENEMY_STATE_IDLE, DIFFICULTY_EASY);
                    break;
                case MEDIUM_STANDING_OFFICER_E:
                    CreateEnemy(i, DIR_EAST, ENEMY_TYPE_OFFICER, ENEMY_STATE_IDLE, DIFFICULTY_MEDIUM);
                    break;
                case HARD_STANDING_OFFICER_E:
                    CreateEnemy(i, DIR_EAST, ENEMY_TYPE_OFFICER, ENEMY_STATE_IDLE, DIFFICULTY_HARD);
                    break;
                case EASY_PATROL_OFFICER_E:
                    CreateEnemy(i, DIR_EAST, ENEMY_TYPE_OFFICER, ENEMY_STATE_PATROL, DIFFICULTY_EASY);
                    break;
                case MEDIUM_PATROL_OFFICER_E:
                    CreateEnemy(i, DIR_EAST, ENEMY_TYPE_OFFICER, ENEMY_STATE_PATROL, DIFFICULTY_MEDIUM);
                    break;
                case HARD_PATROL_OFFICER_E:
                    CreateEnemy(i, DIR_EAST, ENEMY_TYPE_OFFICER, ENEMY_STATE_PATROL, DIFFICULTY_HARD);
                    break;
                case EASY_STANDING_OFFICER_S:
                    CreateEnemy(i, DIR_SOUTH, ENEMY_TYPE_OFFICER, ENEMY_STATE_IDLE, DIFFICULTY_EASY);
                    break;
                case MEDIUM_STANDING_OFFICER_S:
                    CreateEnemy(i, DIR_SOUTH, ENEMY_TYPE_OFFICER, ENEMY_STATE_IDLE, DIFFICULTY_MEDIUM);
                    break;
                case HARD_STANDING_OFFICER_S:
                    CreateEnemy(i, DIR_SOUTH, ENEMY_TYPE_OFFICER, ENEMY_STATE_IDLE, DIFFICULTY_HARD);
                    break;
                case EASY_PATROL_OFFICER_S:
                    CreateEnemy(i, DIR_SOUTH, ENEMY_TYPE_OFFICER, ENEMY_STATE_PATROL, DIFFICULTY_EASY);
                    break;
                case MEDIUM_PATROL_OFFICER_S:
                    CreateEnemy(i, DIR_SOUTH, ENEMY_TYPE_OFFICER, ENEMY_STATE_PATROL, DIFFICULTY_MEDIUM);
                    break;
                case HARD_PATROL_OFFICER_S:
                    CreateEnemy(i, DIR_SOUTH, ENEMY_TYPE_OFFICER, ENEMY_STATE_PATROL, DIFFICULTY_HARD);
                    break;
                case EASY_STANDING_OFFICER_W:
                    CreateEnemy(i, DIR_WEST, ENEMY_TYPE_OFFICER, ENEMY_STATE_IDLE, DIFFICULTY_EASY);
                    break;
                case MEDIUM_STANDING_OFFICER_W:
                    CreateEnemy(i, DIR_WEST, ENEMY_TYPE_OFFICER, ENEMY_STATE_IDLE, DIFFICULTY_MEDIUM);
                    break;
                case HARD_STANDING_OFFICER_W:
                    CreateEnemy(i, DIR_WEST, ENEMY_TYPE_OFFICER, ENEMY_STATE_IDLE, DIFFICULTY_HARD);
                    break;
                case EASY_PATROL_OFFICER_W:
                    CreateEnemy(i, DIR_WEST, ENEMY_TYPE_OFFICER, ENEMY_STATE_PATROL, DIFFICULTY_EASY);
                    break;
                case MEDIUM_PATROL_OFFICER_W:
                    CreateEnemy(i, DIR_WEST, ENEMY_TYPE_OFFICER, ENEMY_STATE_PATROL, DIFFICULTY_MEDIUM);
                    break;
                case HARD_PATROL_OFFICER_W:
                    CreateEnemy(i, DIR_WEST, ENEMY_TYPE_OFFICER, ENEMY_STATE_PATROL, DIFFICULTY_HARD);
                    break;

                default:
                    index = (85 + layer1Data[i]);
                    if (index - OBJECT_DEMO >= 0 && index < MAX_OBJECT_ID)
                    {
                        if (index == OBJECT_GOLDKEY || index == OBJECT_SILVERKEY)
                        {
                            CreatePowerup(i, index == OBJECT_GOLDKEY ? POWERUP_TYPE_GOLD_KEY:POWERUP_TYPE_SILVER_KEY);
                        }
                        else if (index >= OBJECT_TVDINNER && index <= OBJECT_LIFEPOWERUP)
                        {
                            CreatePowerup(i, index - OBJECT_TVDINNER + 1);
                        }
                        else
                        {
                            CreateDecorSprite(i, index);
                        }
                    }
                    break;
                }

            }
        }

        free(layer1Data);
    }

    if (g_backgroundMusic == INVALID_HSOUND_VALUE)
    {
        g_backgroundMusic = AudioCreateMusic(WAV_ROOT"GetThem.wav");
        AudioPlay(g_backgroundMusic, g_player.Position);
    }

    return TRUE;
}

VOID LevelDestroy()
{
    FreeMapImages();
    EnemyClearAll();

    if (g_mapAlloc != INVALID_HALLOC_VALUE)
    {
        MemUnlock(g_mapAlloc);
        MemFree(g_mapAlloc);
        g_mapAlloc = INVALID_HALLOC_VALUE;
        g_map = NULL;
    }

    if (g_backgroundMusic != INVALID_HSOUND_VALUE)
    {
        AudioDestroy(g_backgroundMusic);
        g_backgroundMusic = INVALID_HSOUND_VALUE;
    }

    g_mapWidth = g_mapHeight = 0;
}

VOID LevelShutdown()
{
    LevelDestroy();
}

USHORT LevelGetCellSize()
{
    return 64;
}

USHORT LevelGetWidth()
{
    return g_mapWidth;
}

USHORT LevelGetHeight()
{
    return g_mapHeight;
}

BOOL LevelIsCellEmpty(SHORT cellX, SHORT cellY)
{
    return g_map[GetCell(cellX, cellY)].Type == CELL_TYPE_EMPTY;
}

BOOL LevelIsCellDoor(SHORT cellX, SHORT cellY)
{
    return  g_map[GetCell(cellX, cellY)].Type >= CELL_TYPE_WE_DOOR &&
            g_map[GetCell(cellX, cellY)].Type <= CELL_TYPE_NS_DOOR;
}

BOOL LevelIsElevatorExit(SHORT cellX, SHORT cellY)
{
    return g_map[GetCell(cellX, cellY)].Type == CELL_TYPE_LEVEL_EXIT;
}

BOOL LevelIsCellWEDoor(SHORT cellX, SHORT cellY)
{
    return g_map[GetCell(cellX, cellY)].Type == CELL_TYPE_WE_DOOR;
}

BOOL LevelIsCellNSDoor(SHORT cellX, SHORT cellY)
{
    return g_map[GetCell(cellX, cellY)].Type == CELL_TYPE_NS_DOOR;
}

BOOL LevelIsCellWall(SHORT cellX, SHORT cellY)
{
    return g_map[GetCell(cellX, cellY)].Type >= CELL_TYPE_WALL;
}

BOOL LevelIsCellPushWall(SHORT cellX, SHORT cellY)
{
    return g_map[GetCell(cellX, cellY)].IsPushWall;
}

VOID LevelActivatePushWall(SHORT cellX, SHORT cellY)
{
    if (LevelIsCellPushWall(cellX, cellY))
    {
        int i = 0;
        int numCells = 2;

        VEC2 c = GetCellPosition(GetCell(cellX, cellY));
        VEC2 dir = Vec2Add(c, Vec2Neg(GetCellPosition(GetCell(g_player.Position.x, g_player.Position.y))));

        g_map[GetCell(cellX, cellY)].IsPushWall = 0;
        g_Secret.Texture = g_map[GetCell(cellX, cellY)].Type;
        g_Secret.Position = GetCellPosition(GetCell(cellX, cellY));
        g_Secret.EndPosition = Vec2Add(g_Secret.Position, Vec2Scale(dir, numCells));
        g_map[GetCell(cellX, cellY)].Type = CELL_TYPE_EMPTY;

        for (; i < numCells; ++i)
        {
            g_map[GetCell(c.x, c.y)].IsPushWallOverlappingMe = 1;
            c = Vec2Add(c, dir);
        }
    }
}

BOOL LevelContainsSecret(SHORT cellX, SHORT cellY)
{
    return g_map[GetCell(cellX, cellY)].IsPushWallOverlappingMe;
}

VEC2 LevelGetSecret()
{
    return g_Secret.Position;
}

HSPRITE LevelGetSecretTexture()
{
    return g_Secret.Texture - CELL_TYPE_WALL;
}

BYTE LevelGetDoorState(SHORT cellX, SHORT cellY)
{
    if (!LevelIsCellDoor(cellX, cellY))
    {
        return DOOR_STATE_INVALID;
    }

    return g_map[GetCell(cellX, cellY)].DoorInfo.State;
}

float LevelGetDoorCoverage(SHORT cellX, SHORT cellY)
{
    MAPCELL* cell = &g_map[GetCell(cellX, cellY)];

    if (LevelIsCellDoor(cellX, cellY))
    {
        switch (cell->DoorInfo.State)
        {
        case DOOR_STATE_OPEN:       return 0.0f;
        case DOOR_STATE_CLOSED:     return 1.0f;
        case DOOR_STATE_OPENING:    return 1.0f - (AnimationGetFrame(&cell->DoorInfo.Animation) / (float)DOOR_OPENING_FRAMES);
        case DOOR_STATE_CLOSING:    return (AnimationGetFrame(&cell->DoorInfo.Animation) / (float)DOOR_OPENING_FRAMES);
        }
    }

    return 0;
}

HSPRITE LevelGetWallTexture(SHORT cellX, SHORT cellY)
{
    USHORT type = g_map[GetCell(cellX, cellY)].Type;

    if (type < CELL_TYPE_WALL || type > MAX_WALL_SPRITES + CELL_TYPE_WALL)
    {
        assert(FALSE && "You have some Poopy walls");
        return INVALID_HSPRITE_VALUE;
    }

    return g_wallTextures[type];
}

HSPRITE LevelGetDoorTexture(SHORT cellX, SHORT cellY)
{
    if (!LevelIsCellDoor(cellX, cellY))
    {
        return INVALID_HSPRITE_VALUE;
    }

    return g_doorTextures[g_map[GetCell(cellX, cellY)].DoorInfo.Texture];
}

HSPRITE LevelGetDoorJamTexture(SHORT cellX, SHORT cellY)
{
    if (!LevelIsCellDoor(cellX, cellY))
    {
        return INVALID_HSPRITE_VALUE;
    }

    return g_doorJamTextures[g_map[GetCell(cellX, cellY)].DoorInfo.Texture];
}

BOOL LevelWallHasDarkToneAvailable(SHORT cellX, SHORT cellY)
{
    /* All walls except #1 have light tones at id - 1 */
    SHORT index = g_map[GetCell(cellX, cellY)].Type - CELL_TYPE_WALL;
    return (index % 2 == 0);
}

VOID LevelApplyDirectionChangeTowardPlayer(ENEMY* enemy)
{
    VEC2 toPlayer;
    toPlayer = Vec2Normalize(Vec2Add(g_player.Position, Vec2Neg(enemy->Position)));
    enemy->Direction = toPlayer;
}

VOID LevelApplyPatrolDirection(ENEMY* enemy)
{
    float distance = 0.0f;
    MAPCELL* cell = &g_map[GetCell(enemy->Position.x, enemy->Position.y)];
    VEC2 cellPos = GetCellPosition(GetCell(enemy->Position.x, enemy->Position.y));
    if (cell->HasPatrolDir)
    {
        distance = Vec2Len(Vec2Add(enemy->Position, Vec2Neg(cellPos)));
        if (distance < .1)
        {
            enemy->Direction = cell->PatrolDir;
        }
    }
}

VOID LevelRemoveEnemyFromGrid(USHORT enemyIndex)
{
    VEC2 position = EnemyGetPosition(enemyIndex);
    SHORT cellStart, cellEnd;
    SHORT x, y;

    cellStart = GetCell(position.x - 1, position.y - 1);
    cellEnd = GetCell(position.x + 1, position.y + 1);

    for (y = GetCellY(cellStart); y <= GetCellY(cellEnd); ++y)
    {
        for (x = GetCellX(cellStart); x <= GetCellX(cellEnd); ++x)
        {
            MAPCELL* cell = &g_map[GetCell(x, y)];
            SHORT i;
            for (i = 0; i < ARRAYSIZE(cell->Enemies); ++i)
            {
                if (cell->Enemies[i] == enemyIndex)
                {
                    cell->Enemies[i] = INVALID_ENTITY;
                }
            }
        }
    }
}

VOID LevelAddEnemyToGrid(USHORT enemyIndex)
{
    VEC2 position = EnemyGetPosition(enemyIndex);
    SHORT cell = GetCell(position.x, position.y);
    SHORT startX, startY, endX, endY, x, y;

    startX = endX = GetCellX(cell);
    startY = endY = GetCellY(cell);

    if (position.x - ENTITY_RADIUS < GetCellX(cell))
    {
        --startX;
    }
    else if (position.x + ENTITY_RADIUS > GetCellX(GetCell(position.x + 1, position.y)))
    {
        ++endX;
    }

    if (position.y - ENTITY_RADIUS < GetCellY(cell))
    {
        --startY;
    }
    else if (position.y + ENTITY_RADIUS > GetCellY(GetCell(position.x, position.y + 1)))
    {
        ++endY;
    }

    for (y = startY; y <= endY; ++y)
    {
        for (x = startX; x <= endX; ++x)
        {
            MAPCELL* cell = &g_map[GetCell(x, y)];
            SHORT i;
            for (i = 0; i < ARRAYSIZE(cell->Enemies); ++i)
            {
                if (cell->Enemies[i] == INVALID_ENTITY)
                {
                    cell->Enemies[i] = enemyIndex;
                    break;
                }
            }
        }
    }
}

VOID LevelGetEnemies(SHORT cellX, SHORT cellY, USHORT* enemyIndices, USHORT maxEnemies, USHORT* numEnemies)
{
    MAPCELL* cell = &g_map[GetCell(cellX, cellY)];
    SHORT i, j;

    *numEnemies = 0;

    if (cell->Type >= CELL_TYPE_WALL)
    {
        return;
    }

    for (i = 0, j = 0; i < ARRAYSIZE(cell->Enemies) && j < maxEnemies; ++i)
    {
        if (cell->Enemies[i] != INVALID_ENTITY)
        {
            enemyIndices[j++] = cell->Enemies[i];
        }
    }

    *numEnemies = j;
}

VOID LevelGetPowerups(SHORT cellX, SHORT cellY, USHORT* powerupIndices, USHORT maxPowerups, USHORT* numPowerups)
{
    MAPCELL* cell = &g_map[GetCell(cellX, cellY)];
    SHORT i, j;

    *numPowerups = 0;

    if (cell->Type >= CELL_TYPE_WALL)
    {
        return;
    }

    for (i = 0, j = 0; i < ARRAYSIZE(cell->Powerups) && j < maxPowerups; ++i)
    {
        if (cell->Powerups[i] != INVALID_ENTITY)
        {
            powerupIndices[j++] = cell->Powerups[i];
        }
    }

    *numPowerups = j;
}

VOID LevelGetDecorSprites(SHORT cellX, SHORT cellY, HSPRITE* decorSprites, USHORT maxSprites, USHORT* numSprites)
{
    MAPCELL* cell = &g_map[GetCell(cellX, cellY)];
    SHORT i, j;

    *numSprites = 0;

    if (cell->Type > CELL_TYPE_FULLSIZE_DECOR)
    {
        return;
    }

    for (i = 0, j = 0; i < ARRAYSIZE(cell->DecorSprites) && j < maxSprites; ++i)
    {
        if (cell->DecorSprites[i] != INVALID_INDEX)
        {
            decorSprites[j++] = g_decorSprites[cell->DecorSprites[i]];
        }
    }

#ifndef NDEBUG
    for (i = 0; i < j; ++i)
    {
        assert(W3DIsValidSprite(decorSprites[i]));
    }

#endif

    *numSprites = j;
}

VOID LevelAddPowerup(SHORT cellX, SHORT cellY, USHORT powerupIndex)
{
    USHORT i = 0;
    MAPCELL* cell = &g_map[GetCell(cellX, cellY)];

    for (; i < ARRAYSIZE(cell->Powerups); ++i)
    {
        if (cell->Powerups[i] == INVALID_ENTITY)
        {
            cell->Powerups[i] = powerupIndex;
            break;
        }
    }
}

VOID LevelRemovePowerup(SHORT cellX, SHORT cellY, UINT powerupIndex)
{
    USHORT i = 0;
    MAPCELL* cell = &g_map[GetCell(cellX, cellY)];
    HSOUND sound = PowerupGetSound(powerupIndex);

    for (; i < ARRAYSIZE(cell->Powerups); ++i)
    {
        if (cell->Powerups[i] == powerupIndex)
        {
            cell->Powerups[i] = INVALID_ENTITY;
            AudioPlay(sound, g_player.Position);
        }
    }
}

VEC2 LevelMoveWithCollision(VEC2 currentPosition, VEC2 movement)
{
    VEC2 newPosition = Vec2Add(currentPosition, movement);
    SHORT cellX, cellY;
    SHORT minY = (SHORT)currentPosition.y - 1;
    SHORT maxY = (SHORT)currentPosition.y + 1;
    SHORT minX = (SHORT)currentPosition.x - 1;
    SHORT maxX = (SHORT)currentPosition.x + 1;

    /* clamp bounds */
    if (minY < 0) minY = 0;
    if (maxY >= g_mapHeight) maxY = g_mapHeight - 1;

    if (minX < 0) minX = 0;
    if (maxX >= g_mapWidth) maxX = g_mapWidth - 1;

    for (cellY = minY; cellY <= maxY; ++cellY)
    {
        for (cellX = minX; cellX <= maxX; ++cellX)
        {
            if (IsCellOccupied(cellX, cellY))
            {
                newPosition = CollideWith(newPosition, cellX, cellY);
            }
        }
    }

    return newPosition;
}

USHORT LevelRaycastEnemy(VEC2 position, VEC2 direction)
{
    /* start from where we are, and work our way out until we hit the first enemy */
    SHORT cellX = position.x;
    SHORT cellY = position.y;
    BYTE side = WALL_NORTH;
    USHORT i, index;
    direction = Vec2Normalize(direction);

    while (side != WALL_NONE)
    {
        index = GetCell(cellX, cellY);

        /* wall or closed door? then no hit */
        if (g_map[index].Type >= CELL_TYPE_WALL)
        {
            return INVALID_ENTITY;
        }
        else if (LevelIsCellDoor(cellX, cellY) &&
                 g_map[index].DoorInfo.State != DOOR_STATE_OPEN)
        {
            return INVALID_ENTITY;
        }

        /* found an enemy? */
        for (i = 0; i < ARRAYSIZE(g_map[index].Enemies); ++i)
        {
            if (EnemyIsAlive(g_map[index].Enemies[i]))
            {
                return g_map[index].Enemies[i];
            }
        }

        side = RayCastOutwardFromCell(&position, direction, cellX, cellY);
        switch (side)
        {
        case WALL_NORTH:    --cellY;    break;
        case WALL_SOUTH:    ++cellY;    break;
        case WALL_WEST:     --cellX;    break;
        case WALL_EAST:     ++cellX;    break;
        }

        if (cellX < 0 || cellX >= g_mapWidth ||
            cellY < 0 || cellY >= g_mapHeight)
        {
            return INVALID_ENTITY;
        }
    }

    return INVALID_ENTITY;
}

BOOL LevelRaycastPlayerScan(VEC2 position, VEC2 direction, float* distance)
{
    BOOL hitPlayer = FALSE;
    VEC2 dir = direction;
    VEC2 scanDir;
    FLOAT scanIncrement = 0;

    hitPlayer = LevelRaycastPlayer(position, dir, distance);
    if (!hitPlayer)
    {
        for (scanIncrement = .1; scanIncrement < 1; scanIncrement += .1)
        {
            scanDir.x = dir.x + scanIncrement;
            scanDir.y = dir.y + scanIncrement;
            scanDir = Vec2Normalize(scanDir);
            hitPlayer = LevelRaycastPlayer(position, scanDir, distance);
            if (!hitPlayer)
            {
                scanDir.x = dir.x - scanIncrement;
                scanDir.y = dir.y - scanIncrement;
                scanDir = Vec2Normalize(scanDir);
                hitPlayer = LevelRaycastPlayer(position, scanDir, distance);
            }

            if (hitPlayer)
            {
                break;
            }
        }
    }

    return hitPlayer;
}

BOOL LevelRaycastPlayer(VEC2 position, VEC2 direction, float* distance)
{
    /* start from where we are, and work our way out until we hit the first enemy */
    SHORT cellX = position.x;
    SHORT cellY = position.y;
    BYTE side = WALL_NORTH;
    USHORT i, index;
    SHORT playerIndex = GetCell(g_player.Position.x, g_player.Position.y);
    direction = Vec2Normalize(direction);

    while (side != WALL_NONE)
    {
        index = GetCell(cellX, cellY);

        /* wall or closed door? then no hit */
        if (g_map[index].Type >= CELL_TYPE_WALL)
        {
            return FALSE;
        }
        else if (LevelIsCellDoor(cellX, cellY) &&
                 g_map[index].DoorInfo.State != DOOR_STATE_OPEN)
        {
            return FALSE;
        }

        /* found an player? */
        if (index == playerIndex)
        {
            *distance = Vec2Len(Vec2Add(position, Vec2Neg(g_player.Position)));
            return TRUE;
        }

        side = RayCastOutwardFromCell(&position, direction, cellX, cellY);
        switch (side)
        {
        case WALL_NORTH:    --cellY;    break;
        case WALL_SOUTH:    ++cellY;    break;
        case WALL_WEST:     --cellX;    break;
        case WALL_EAST:     ++cellX;    break;
        }

        if (cellX < 0 || cellX >= g_mapWidth ||
            cellY < 0 || cellY >= g_mapHeight)
        {
            return FALSE;
        }
    }

    return FALSE;
}

USHORT LevelGetEnemyInFront(VEC2 position, VEC2 direction)
{
    SHORT cellX = position.x;
    SHORT cellY = position.y;
    USHORT i;
    BYTE side = RayCastOutwardFromCell(&position, Vec2Normalize(direction), cellX, cellY);
    MAPCELL* cell;

    switch (side)
    {
    case WALL_NORTH:    --cellY;    break;
    case WALL_SOUTH:    ++cellY;    break;
    case WALL_WEST:     --cellX;    break;
    case WALL_EAST:     ++cellX;    break;
    }

    if (cellX < 0 || cellX >= g_mapWidth ||
        cellY < 0 || cellY >= g_mapHeight)
    {
        return INVALID_ENTITY;
    }

    cell = &g_map[GetCell(cellX, cellY)];

    for (i = 0; i < ARRAYSIZE(cell->Enemies); ++i)
    {
        if (EnemyIsAlive(cell->Enemies[i]))
        {
            return cell->Enemies[i];
        }
    }

    return INVALID_ENTITY;
}

VOID LevelClickInFront(VEC2 position, VEC2 direction)
{
    /* get the cell in front of us */
    SHORT cellX = position.x;
    SHORT cellY = position.y;
    BYTE side = RayCastOutwardFromCell(&position, Vec2Normalize(direction), cellX, cellY);
    USHORT index;
    MAPCELL* cell;

    switch (side)
    {
    case WALL_NORTH:    --cellY;    break;
    case WALL_SOUTH:    ++cellY;    break;
    case WALL_WEST:     --cellX;    break;
    case WALL_EAST:     ++cellX;    break;
    }

    if (cellX < 0 || cellX >= g_mapWidth ||
        cellY < 0 || cellY >= g_mapHeight)
    {
        return;
    }

    index = GetCell(cellX, cellY);
    cell = &g_map[index];

    /* check the distance */
    if (Vec2Len(Vec2Add(position, Vec2Neg(GetCellPosition(index)))) < 1.25f)
    {
        if (LevelIsElevatorExit(cellX, cellY))
        {
            StartNextLevel();
        }
        else if (LevelIsCellPushWall(cellX, cellY))
        {
            /* open secret wall */
            AudioPlay(g_openSecretWallSound, GetCellPosition(index));
            LevelActivatePushWall(cellX, cellY);
        }
        else if (LevelIsCellDoor(cellX, cellY))
        {
            if (cell->DoorInfo.State == DOOR_STATE_CLOSED)
            {
                /* open it if the player has the proper key or the door is not locked */
                /* NOTE: Enemies are currently using this mechanism to open/close doors, so if the player
                         doesn't have the proper keys, enemies will not be able to open locked doors.  We could
                         change this, but I don't think it really effects gameplay.*/
                if ((cell->DoorInfo.Lock == DOOR_LOCKED_NONE) ||
                    (cell->DoorInfo.Lock == DOOR_LOCKED_GOLD && g_player.HasGoldKey) ||
                    (cell->DoorInfo.Lock == DOOR_LOCKED_SILVER && g_player.HasSilverKey))
                {
                    cell->DoorInfo.State = DOOR_STATE_OPENING;
                    AnimationInit(&cell->DoorInfo.Animation, 1.0f, DOOR_OPENING_FRAMES, FALSE);
                    AudioPlay(g_openDoorSound, GetCellPosition(index));
                }
            }
        }
    }
}

VOID LevelUpdate()
{
    PlayerUpdate();
    UpdateDoors();
    EnemyUpdateAll();

    if (g_Secret.Texture != 0)
    {
        VEC2 dir = Vec2Add(g_Secret.EndPosition, Vec2Neg(g_Secret.Position));
        float dist = Vec2Len(dir);
        int i = 0;

        dir = Vec2Normalize(dir);
        g_Secret.Position = Vec2Add(g_Secret.Position, Vec2Scale(dir, 0.01f));
        if (dist < 0.05f)
        {
            g_map[GetCell(g_Secret.Position.x, g_Secret.Position.y)].Type = g_Secret.Texture;
            g_Secret.Texture = 0;
        }

        /* SUPER HACK SINCE WE'RE TRYING TO FINISH TONIGHT */
        for (; i < LevelGetWidth() * LevelGetHeight(); ++i)
        {
            dist = Vec2Len(Vec2Add(GetCellPosition(i), Vec2Neg(g_Secret.Position)));
            if (dist > 0.7f)
            {
                g_map[i].IsPushWallOverlappingMe = 0;
            }
            else
            {
                g_map[i].IsPushWallOverlappingMe = 1;
            }
        }
    }
}

/**************************************************************
    Internal helper methods
***************************************************************/

USHORT RawWolfWallValueToId(USHORT i)
{
    return (i - 1) * 2 + 1;
}

USHORT GetCell(SHORT x, SHORT y)
{
    /* force any out of bounds coordinates into bounds */
    if (x < 0) x = 0;
    if (x >= g_mapWidth) x = g_mapWidth - 1;

    if (y < 0) y = 0;
    if (y >= g_mapHeight) y = g_mapHeight - 1;

    return y * g_mapWidth + x;
}

VOID GetCells(VEC2* position, USHORT* cellIds)
{
    BYTE i = 0;
    // max cells 4
    cellIds[0] = GetCell(position->x + .5, position->y + .5); // initial containing cell
    cellIds[1] = GetCell(position->x - .5, position->y - .5); // initial containing cell
    cellIds[2] = GetCell(position->x - .5, position->y + .5); // initial containing cell
    cellIds[3] = GetCell(position->x + .5, position->y - .5); // initial containing cell
}
USHORT GetCellX(USHORT cellIndex)
{
    return cellIndex % g_mapWidth;
}

USHORT GetCellY(USHORT cellIndex)
{
    return cellIndex / g_mapHeight;
}

VEC2 GetCellPosition(USHORT cellIndex)
{
    VEC2 v = { GetCellX(cellIndex) + 0.5f, GetCellY(cellIndex) + 0.5f };
    return v;
}

BOOL CellContainsEntities(USHORT cellIndex)
{
    return  CellContainsEnemies(cellIndex) ||
            CellContainsPowerups(cellIndex);
}

BOOL CellContainsEnemies(USHORT cellIndex)
{
    USHORT i;
    MAPCELL* cell = &g_map[cellIndex];

    if (cell->Type >= CELL_TYPE_WALL)
    {
        return FALSE;
    }

    for (i = 0; i < ARRAYSIZE(g_map[0].Enemies); ++i)
    {
        if (cell->Enemies[i] != INVALID_ENTITY)
            return TRUE;
    }

    return FALSE;
}

BOOL CellContainsPowerups(USHORT cellIndex)
{
    USHORT i;
    MAPCELL* cell = &g_map[cellIndex];

    if (cell->Type >= CELL_TYPE_WALL)
    {
        return FALSE;
    }

    for (i = 0; i < ARRAYSIZE(g_map[0].Powerups); ++i)
    {
        if (cell->Powerups[i] != INVALID_ENTITY)
            return TRUE;
    }

    return FALSE;
}

BOOL CellContainsDecorSprites(USHORT cellIndex)
{
    USHORT i;
    MAPCELL* cell = &g_map[cellIndex];

    if (cell->Type > CELL_TYPE_FULLSIZE_DECOR)
    {
        return FALSE;
    }

    for (i = 0; i < ARRAYSIZE(g_map[0].DecorSprites); ++i)
    {
        if (cell->DecorSprites[i] != INVALID_INDEX)
            return TRUE;
    }

    return FALSE;
}

VOID LoadMapImages()
{
    SHORT index = CELL_TYPE_WALL;
    for(index = CELL_TYPE_WALL; index < CELL_TYPE_WALL + (MAX_WALL_SPRITES - 1); index++)
    {
        g_wallTextures[index] = W3DCreateSprite(index - CELL_TYPE_WALL, FALSE);
    }

    /* TODO: FIX THIS */
    g_doorTextures[0] = W3DCreateSprite(WALL_DOOR_LIGHT, FALSE);
    g_doorTextures[1] = W3DCreateSprite(WALL_ELEVATOR_DOOR_DARK_SIDE, FALSE);
    g_doorTextures[2] = W3DCreateSprite(WALL_DOOR_LOCKED_LIGHT_SIDE, FALSE);
    g_doorTextures[3] = W3DCreateSprite(WALL_DOOR_LOCKED_DARK_SIDE, FALSE);
    g_doorJamTextures[0] = W3DCreateSprite(WALL_DOOR_LIGHT_SIDE, FALSE);
    g_doorJamTextures[1] = W3DCreateSprite(WALL_DOOR_LIGHT_SIDE, FALSE);
    g_doorJamTextures[2] = W3DCreateSprite(WALL_DOOR_LIGHT_SIDE, FALSE);
    g_doorJamTextures[3] = W3DCreateSprite(WALL_DOOR_LIGHT_SIDE, FALSE);

    for(index = 0; index <= (OBJECT_VINES - OBJECT_DEMO); index++)
    {
        g_decorSprites[index] = W3DCreateSprite(index + OBJECT_DEMO, FALSE);
    }
    for (; index < ARRAYSIZE(g_decorSprites); ++index)
    {
        g_decorSprites[index] = INVALID_HSPRITE_VALUE;
    }

    g_openDoorSound = AudioCreateSFX(CONTENT_ROOT "wavs\\DoorOpen.wav");
    g_closeDoorSound = AudioCreateSFX(CONTENT_ROOT "wavs\\DoorClose.wav");
    g_openSecretWallSound = AudioCreateSFX(CONTENT_ROOT "wavs\\PushwallOpen.wav");
}

VOID FreeMapImages()
{
    USHORT i;

    for (i = 0; i < ARRAYSIZE(g_wallTextures); ++i)
    {
        if (W3DIsValidSprite(g_wallTextures[i]))
        {
            W3DDestroySprite(g_wallTextures[i]);
        }
        g_wallTextures[i] = INVALID_HSPRITE_VALUE;
    }

    for (i = 0; i < ARRAYSIZE(g_doorTextures); ++i)
    {
        if (W3DIsValidSprite(g_doorTextures[i]))
        {
            W3DDestroySprite(g_doorTextures[i]);
        }
        g_doorTextures[i] = INVALID_HSPRITE_VALUE;
    }

    for (i = 0; i < ARRAYSIZE(g_doorJamTextures); ++i)
    {
        if (W3DIsValidSprite(g_doorJamTextures[i]))
        {
            W3DDestroySprite(g_doorJamTextures[i]);
        }
        g_doorJamTextures[i] = INVALID_HSPRITE_VALUE;
    }

    for (i = 0; i < ARRAYSIZE(g_decorSprites); ++i)
    {
        if (W3DIsValidSprite(g_decorSprites[i]))
        {
            W3DDestroySprite(g_decorSprites[i]);
        }
        g_decorSprites[i] = INVALID_HSPRITE_VALUE;
    }

    if (g_openDoorSound != INVALID_HSOUND_VALUE)
    {
        AudioDestroy(g_openDoorSound);
        g_openDoorSound = INVALID_HSOUND_VALUE;
    }

    if (g_closeDoorSound != INVALID_HSOUND_VALUE)
    {
        AudioDestroy(g_closeDoorSound);
        g_closeDoorSound = INVALID_HSOUND_VALUE;
    }

    if (g_openSecretWallSound != INVALID_HSOUND_VALUE)
    {
        AudioDestroy(g_openSecretWallSound);
        g_openSecretWallSound = INVALID_HSOUND_VALUE;
    }
}

BOOL IsCellOccupied(SHORT x, SHORT y)
{
    MAPCELL* cell = &g_map[GetCell(x, y)];
    USHORT i;

    for (i = 0; i < ARRAYSIZE(cell->Enemies); ++i)
    {
        if (EnemyIsAlive(cell->Enemies[i]))
            return TRUE;
    }

    return (cell->Type >= CELL_TYPE_WALL ||                 /* A wall */
            cell->Type == CELL_TYPE_FULLSIZE_DECOR ||       /* full size decor */
            (LevelIsCellDoor(x, y) &&                       /* closed door blocking the way */
            cell->DoorInfo.State != DOOR_STATE_OPEN) ||
            cell->IsPushWallOverlappingMe);
}

VEC2 CollideWith(VEC2 position, SHORT cellX, SHORT cellY)
{
    float x1, x2;
    float y1, y2;
    VEC2 d = { position.x - (cellX + 0.5f), position.y - (cellY + 0.5f) };

    x1 = cellX + 1 - (position.x - ENTITY_RADIUS);
    x2 = (position.x + ENTITY_RADIUS) - cellX;
    if (x1 < 0 || x2 < 0)
        return position;

    y1 = cellY + 1 - (position.y - ENTITY_RADIUS);
    y2 = (position.y + ENTITY_RADIUS) - cellY;
    if (y1 < 0 || y2 < 0)
        return position;

    x1 = min(x1, x2);
    y1 = min(y1, y2);

    if (x1 < y1)
    {
        position.x += x1 * (d.x >= 0 ? 1 : -1);
    }
    else
    {
        position.y += y1 * (d.y >= 0 ? 1 : -1);
    }

    return position;
}

BOOL IsWithin(float v, BYTE start, BYTE end)
{
    return (v >= start && v <= end);
}

BYTE RayCastOutwardFromCell(VEC2* position, VEC2 direction, SHORT cellX, SHORT cellY)
{
    if (direction.y < 0)
    {
        float d = position->y - cellY;
        float h = d / Vec2Dot(Vec2Up(), direction);
        VEC2 intersection = Vec2Add(*position, Vec2Scale(direction, h));
        if (IsWithin(intersection.x, cellX, cellX + 1))
        {
            *position = intersection;
            return WALL_NORTH;
        }
    }
    else if (direction.y > 0)
    {
        float d = cellY + 1 - position->y;
        float h = d / Vec2Dot(Vec2Down(), direction);
        VEC2 intersection = Vec2Add(*position, Vec2Scale(direction, h));
        if (IsWithin(intersection.x, cellX, cellX + 1))
        {
            *position = intersection;
            return WALL_SOUTH;
        }
    }

    if (direction.x < 0)
    {
        float d = position->x - cellX;
        float h = d / Vec2Dot(Vec2Left(), direction);
        VEC2 intersection = Vec2Add(*position, Vec2Scale(direction, h));
        if (IsWithin(intersection.y, cellY, cellY + 1))
        {
            *position = intersection;
            return WALL_WEST;
        }
    }
    else if (direction.x > 0)
    {
        float d = cellX + 1 - position->x;
        float h = d / Vec2Dot(Vec2Right(), direction);
        VEC2 intersection = Vec2Add(*position, Vec2Scale(direction, h));
        if (IsWithin(intersection.y, cellY, cellY + 1))
        {
            *position = intersection;
            return WALL_EAST;
        }
    }

    return WALL_NONE;
}

VOID CreateWall(USHORT index, USHORT wallIndex)
{
    MAPCELL* cell = &g_map[index];
    cell->Type = CELL_TYPE_WALL + wallIndex - 1;
    cell->IsPushWall = 0;
}

VOID CreateDoor(USHORT index, BOOL directionWE, BYTE initialState, BYTE texture, BYTE lockType)
{
    MAPCELL* cell = &g_map[index];
    cell->Type = directionWE ? CELL_TYPE_WE_DOOR : CELL_TYPE_NS_DOOR;
    AnimationInit(&cell->DoorInfo.Animation, 0, 0, FALSE);
    cell->DoorInfo.State = initialState;
    cell->DoorInfo.Texture = texture;
    cell->DoorInfo.Lock = lockType;

    g_doorCells[g_numDoors++] = index;
}

VOID CreatePushWall(USHORT index)
{
    MAPCELL* cell = &g_map[index];
    cell->IsPushWall = 1;
}

VOID CreatePatrol(USHORT index, float x, float y)
{
    MAPCELL* cell = &g_map[index];
    cell->HasPatrolDir = 1;
    cell->PatrolDir.x = x;
    cell->PatrolDir.y = y;
    cell->PatrolDir = Vec2Normalize(cell->PatrolDir);
}

VOID CreateEnemy(USHORT index, BYTE direction, BYTE enemyType, BYTE initialState, BYTE difficulty)
{
    VEC2 dir = {0,0};
    MAPCELL* cell = &g_map[index];
    cell->Type = CELL_TYPE_EMPTY;

    switch(direction)
    {
    case DIR_EAST:
        dir.x = 0; dir.y = -1;
        break;
    case DIR_NORTH: 
        dir.x = 1; dir.y = 0;
        break;
    case DIR_WEST:
        dir.x = 0; dir.y = 1;
        break;
    case DIR_SOUTH:
        dir.x = -1; dir.y = 0;
        break;
    case DIR_DONTCARE:
        default:
            break;
    }

    // Only create enemies if the difficulty is less than or equal to the player's
    // desired difficulty setting
    if (difficulty <= g_player.Difficulty)
    {
        cell->Enemies[0] = EnemyCreate(GetCellPosition(index), dir, enemyType, initialState);
    }
}

VOID CreatePowerup(USHORT index, BYTE powerupType)
{
    MAPCELL* cell = &g_map[index];
    cell->Type = CELL_TYPE_EMPTY;
    cell->Powerups[0] = PowerupCreate(GetCellPosition(index), powerupType);
}

VOID CreateDecorSprite(USHORT index, USHORT sprite)
{
    VEC2 pos = GetCellPosition(index);
    MAPCELL* cell = &g_map[index];
    SHORT id = sprite - OBJECT_DEMO;

    switch (sprite)
    {
    case OBJECT_GREENBARREL:
    case OBJECT_TABLE_AND_CHAIRS:
    case OBJECT_GREENLAMP:
    case OBJECT_HANGING_SKELETON:
    case OBJECT_PILLER:
    case OBJECT_TREEPLANT:
    case OBJECT_SINK:
    case OBJECT_POTTEDPLANT:
    case OBJECT_BLUEVACE:
    case OBJECT_TABLE:
    case OBJECT_KNIGHT:
    case OBJECT_CAGEEMPTY:
    case OBJECT_CAGESKELETON:
    case OBJECT_BED:
    case OBJECT_BROWNBARREL:
    case OBJECT_WELLFULL: 
    case OBJECT_WELLEMPTY:
    case OBJECT_FLAG:
    case OBJECT_WOODSTOVE:
    case OBJECT_SPEARS: 
        cell->Type = CELL_TYPE_FULLSIZE_DECOR;
        break;

    default:
        cell->Type = CELL_TYPE_EMPTY; // CELL_TYPE_FULLSIZE_DECOR;
        break;
    }
    cell->DecorSprites[0] = id; /* TODO: Do something here */
}

VOID UpdateDoors()
{
    USHORT i = 0;
    for (; i < g_numDoors; ++i)
    {
        UpdateDoor(i);
    }
}

VOID UpdateDoor(USHORT doorIndex)
{
    USHORT cellIndex = g_doorCells[doorIndex];
    USHORT i;
    MAPCELL* cell = &g_map[cellIndex];
    BOOL animationDone = FALSE;

    animationDone = !AnimationUpdate(&cell->DoorInfo.Animation);

    switch (cell->DoorInfo.State)
    {
    case DOOR_STATE_OPENING:
        if (animationDone)
        {
            AnimationInit(&cell->DoorInfo.Animation, 2.5f, 1, FALSE);
            cell->DoorInfo.State = DOOR_STATE_OPEN;
            return;
        }
        break;

    case DOOR_STATE_OPEN:
        if (animationDone)
        {
            /* are we blocked for this frame? */
            if (CellContainsEnemies(cellIndex) ||
                CellContainsEntities(cellIndex) ||
                GetCell(g_player.Position.x, g_player.Position.y) == cellIndex)
            {
                AnimationSetFrame(&cell->DoorInfo.Animation, DOOR_OPENING_FRAMES);
                return; /* so we try again next frame */
            }
            else
            {
                AnimationInit(&cell->DoorInfo.Animation, 1.0f, DOOR_OPENING_FRAMES, FALSE);
                cell->DoorInfo.State = DOOR_STATE_CLOSING;
                AudioPlay(g_closeDoorSound, GetCellPosition(cellIndex));
                return;
            }
        }
        break;

    case DOOR_STATE_CLOSING:
        if (animationDone)
        {
            AnimationInit(&cell->DoorInfo.Animation, 0, 1, FALSE);
            cell->DoorInfo.State = DOOR_STATE_CLOSED;
            return;
        }
        break;
    }
}
