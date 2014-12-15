#ifndef _LEVEL_H_
#define _LEVEL_H_

/* Door states */
#define DOOR_STATE_CLOSED   0
#define DOOR_STATE_OPENING  1
#define DOOR_STATE_OPEN     2
#define DOOR_STATE_CLOSING  3
#define DOOR_STATE_INVALID  4

/* Cell types */
#define CELL_TYPE_EMPTY             0
#define CELL_TYPE_FULLSIZE_DECOR    1 /* full-size, collidable decor entity such as table */
#define CELL_TYPE_WE_DOOR           2
#define CELL_TYPE_NS_DOOR           3
#define CELL_TYPE_WALL              4 /* Walls start from here. To get index of wall type: (cell.Type - CELL_TYPE_WALL). */
#define CELL_TYPE_LEVEL_EXIT        (CELL_TYPE_WALL + WALL_ELEVATOR_SWITCH_DOWN - 1)

/* Entity types */
#define ENTITY_ENEMY        0
#define ENTITY_DECOR        1

#define MAX_ENEMIES_PER_CELL    5
#define MAX_POWERUPS_PER_CELL   5

/* Max number of doors in a map */
#define MAX_DOORS           300

#define DOOR_LOCKED_NONE    0
#define DOOR_LOCKED_SILVER  1
#define DOOR_LOCKED_GOLD    2

#define DOOR_OPENING_FRAMES 128

/* Maximum number of images */
#define MAX_WALL_TEXTURES   1000
#define MAX_DOOR_TEXTURES   10

#define MAX_DECOR_SPRITES   1000
#define MAX_DECOR_SPRITES_PER_CELL 5

/* Sides of a cell */
#define WALL_NONE   0
#define WALL_NORTH  1
#define WALL_SOUTH  2
#define WALL_WEST   3
#define WALL_EAST   4

#define DIR_DONTCARE 0
#define DIR_NORTH    1
#define DIR_EAST     2
#define DIR_SOUTH    3
#define DIR_WEST     4

#define INVALID_INDEX   0xff

/* Cell data */
/* 

    NOTE: Enemies, powerups, and decor sprites behave slightly differently.

    Enemies & powerups are things that have instances, and so there may be quite a few of them
    throughout the level. The arrays below hold indicies into their respective collections.
    You can use Enemy*() and Powerup*() APIs to get information about the entity index in the array.
    INVALID_ENTITY is the value for representing to enemy/powerup.

    DecorSprites are things that don't have instances. They are just a sprite, with no state.
    Therefore, all copies of a sprite throughout the level are just that: copies. Therefore, 
    we use the indices in the array for decor sprites to represent which unique sprite to 
    draw here. Every cell that has the same sprite should use the same index. This is why this is 
    just a BYTE, and not a USHORT (255 unique sprites is plenty I think).

    This allows us to nicely pack in the DecorSprite array into the union with the door's
    4 bytes of data, since a cell cannot have decor sprite(s) AND be a door at the same time.

    NOTE 2: CELL_TYPE_FULLSIZE_DECOR means that there are decor pieces in this cell
    that should block the user's movement, such as a table. Any CELL_TYPE_EMPTY cell
    may also contain decor sprites. These are for decor only, and will not block the user.

*/

typedef struct
{
    USHORT Type;

    USHORT Enemies[MAX_ENEMIES_PER_CELL];  /* Indicies of any enemies that overlap the grid cell */
    USHORT Powerups[MAX_POWERUPS_PER_CELL]; /* Indicies of any powerups that overlap with the cell */
    BYTE HasPatrolDir;
    VEC2 PatrolDir;
    BYTE IsPushWall;
    BYTE IsPushWallOverlappingMe;
    union
    {
        struct
        {
            BYTE        Lock;
            BYTE        State;
            BYTE        Texture;
            ANIMATION   Animation;
        }  DoorInfo;

        USHORT DecorSprites[MAX_DECOR_SPRITES_PER_CELL]; /* Can have up to 4 different decor sprites: tables, blood stains, chandaliers, etc... */
    };

} MAPCELL;

/* Level API */
VOID LevelInit();
BOOL LevelCreate(BYTE levelNumber);
VOID LevelDestroy();
VOID LevelShutdown();

USHORT LevelGetCellSize();
USHORT LevelGetWidth();
USHORT LevelGetHeight();

BOOL LevelIsCellEmpty(SHORT cellX, SHORT cellY);
BOOL LevelIsCellDoor(SHORT cellX, SHORT cellY);
BOOL LevelIsCellWEDoor(SHORT cellX, SHORT cellY);
BOOL LevelIsCellNSDoor(SHORT cellX, SHORT cellY);
BOOL LevelIsCellWall(SHORT cellX, SHORT cellY);
BOOL LevelIsCellPushWall(SHORT cellX, SHORT cellY);
BYTE LevelGetDoorState(SHORT cellX, SHORT cellY);
float LevelGetDoorCoverage(SHORT cellX, SHORT cellY);

VOID LevelActivatePushWall(SHORT cellX, SHORT cellY);
BOOL LevelContainsSecret(SHORT cellX, SHORT cellY);
VEC2 LevelGetSecret();
HSPRITE LevelGetSecretTexture();

/*  retrieve texture for the cell. If the cell is a door, use the door specific API to get door related textures */
HSPRITE LevelGetWallTexture(SHORT cellX, SHORT cellY);
HSPRITE LevelGetDoorTexture(SHORT cellX, SHORT cellY);
HSPRITE LevelGetDoorJamTexture(SHORT cellX, SHORT cellY);
BOOL LevelWallHasDarkToneAvailable(SHORT cellX, SHORT cellY);

/* Be sure to call Remove BEFORE moving, and Add AFTER moving an enemy */
VOID LevelRemoveEnemyFromGrid(USHORT enemyIndex);
VOID LevelAddEnemyToGrid(USHORT enemyIndex);

VOID LevelApplyPatrolDirection(ENEMY* enemy);
VOID LevelApplyDirectionChangeTowardPlayer(ENEMY* enemy);

VOID LevelGetEnemies(SHORT cellX, SHORT cellY, USHORT* enemyIndices, USHORT maxEnemies, USHORT* numEnemies);
VOID LevelGetPowerups(SHORT cellX, SHORT cellY, USHORT* powerupIndices, USHORT maxPowerups, USHORT* numPowerups);
VOID LevelGetDecorSprites(SHORT cellX, SHORT cellY, HSPRITE* decorSprites, USHORT maxSprites, USHORT* numSprites);

VOID LevelAddPowerup(SHORT cellX, SHORT cellY, USHORT powerupIndex);
VOID LevelRemovePowerup(SHORT cellX, SHORT cellY, UINT powerupIndex);

VEC2 LevelMoveWithCollision(VEC2 currentPosition, VEC2 movement);
USHORT LevelRaycastEnemy(VEC2 position, VEC2 direction);
BOOL LevelRaycastPlayer(VEC2 position, VEC2 direction, float* distance);
BOOL LevelRaycastPlayerScan(VEC2 position, VEC2 direction, float* distance);
USHORT LevelGetEnemyInFront(VEC2 position, VEC2 direction);

VOID LevelClickInFront(VEC2 position, VEC2 direction);

VOID LevelUpdate();
VOID LevelDraw();

#endif /* _LEVEL_H_ */