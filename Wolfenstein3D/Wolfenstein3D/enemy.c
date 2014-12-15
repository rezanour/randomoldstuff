#include "common.h"

#define FACING_FORWARD      0
#define FACING_FORWARD_LEFT 1
#define FACING_LEFT         2
#define FACING_BACK_LEFT    3
#define FACING_BACK         4
#define FACING_BACK_RIGHT   5
#define FACING_RIGHT        6
#define FACING_FORWARD_RIGHT 7

ENEMY g_enemies[MAX_ENEMIES];
static USHORT g_numEnemies;
extern MAPCELL* g_map;

/* attacking and dying animations don't count here */
#define ENEMY_FRAMES        5
#define ENEMY_DIRECTIONS    8
static HSPRITE g_enemySprites[MAX_ENEMY_TYPES][MAX_ENEMY_STATES][ENEMY_FRAMES][ENEMY_DIRECTIONS];
static HSOUND g_enemyDyingSounds[MAX_ENEMY_TYPES];

/* Internal method prototypes */
VOID EnemySwitchToState(USHORT enemyIndex, BYTE state);
VOID LoadSoldierResources();
VOID LoadDogResources();
VOID LoadBlueDudeResources();
VOID LoadZombieResources();
VOID LoadOfficerResources();
VOID LoadBoss1Resources();

VOID GetCells(VEC2* position, USHORT* cellIds);

/* Public Enemy API */
VOID EnemyInit()
{
    SHORT i = 0;

    for (i = 0; i < ARRAYSIZE(g_enemyDyingSounds); ++i)
    {
        g_enemyDyingSounds[i] = INVALID_HSOUND_VALUE;
    }

    LoadSoldierResources();
    LoadDogResources();
    LoadBlueDudeResources();
    LoadZombieResources();
    LoadOfficerResources();
    LoadBoss1Resources();
}

VOID EnemyShutdown()
{
    SHORT i = 0;

    for (i = 0; i < ARRAYSIZE(g_enemyDyingSounds); ++i)
    {
        AudioDestroy(g_enemyDyingSounds[i]);
        g_enemyDyingSounds[i] = INVALID_HSOUND_VALUE;
    }
}

VOID EnemyClearAll()
{
    USHORT i = 0;
    ZeroMemory(g_enemies, sizeof(g_enemies[0]) * MAX_ENEMIES);
    for (i = 0; i < ARRAYSIZE(g_enemies); i++)
    {
        g_enemies[i].Type = ENEMY_TYPE_INVALID;
    }
    g_numEnemies = 0;
}

USHORT EnemyCreate(VEC2 position, VEC2 direction, BYTE type, BYTE initialState)
{
    USHORT index = INVALID_ENTITY;

    if (g_numEnemies < MAX_ENEMIES)
    {
        g_enemies[g_numEnemies].Type = type;
        g_enemies[g_numEnemies].Direction = direction;

        switch (type)
        {
        case ENEMY_SOLDIER:
            g_enemies[g_numEnemies].Health = 25;
            g_enemies[g_numEnemies].Position = position;
            g_enemies[g_numEnemies].CurrentState = initialState;
            break;

        case ENEMY_DOG:
            g_enemies[g_numEnemies].Health = 1;
            g_enemies[g_numEnemies].Position = position;
            g_enemies[g_numEnemies].CurrentState = initialState;
            break;

        case ENEMY_BLUEDUDE:
            g_enemies[g_numEnemies].Health = 100;
            g_enemies[g_numEnemies].Position = position;
            g_enemies[g_numEnemies].CurrentState = initialState;
            break;

        case ENEMY_ZOMBIE:
            g_enemies[g_numEnemies].Health = 100;
            g_enemies[g_numEnemies].Position = position;
            g_enemies[g_numEnemies].CurrentState = initialState;
            break;

        case ENEMY_OFFICER:
            g_enemies[g_numEnemies].Health = 100;
            g_enemies[g_numEnemies].Position = position;
            g_enemies[g_numEnemies].CurrentState = initialState;
            break;

         case ENEMY_BOSS1:
            g_enemies[g_numEnemies].Health = 100;
            g_enemies[g_numEnemies].Position = position;
            g_enemies[g_numEnemies].CurrentState = initialState;
            break;

        default:
            return INVALID_ENTITY;
        }

        index = g_numEnemies++;
        
        // Ensure animations are properly started for initial state?
        EnemySwitchToState(index, initialState);
    }

    return index;
}

VEC2 EnemyGetPosition(USHORT enemyIndex)
{
    VEC2 position = { 0, 0 };

    if (enemyIndex != INVALID_ENTITY)
    {
        position = g_enemies[enemyIndex].Position;
    }

    return position;
}

BOOL EnemyIsAlive(USHORT enemyIndex)
{
    if (enemyIndex != INVALID_ENTITY && enemyIndex < g_numEnemies)
    {
        ENEMY* e = &g_enemies[enemyIndex];
        return e->CurrentState != ENEMY_STATE_DYING && e->CurrentState != ENEMY_STATE_DEAD;
    }

    return FALSE;
}

VOID EnemyTakeDamage(USHORT enemyIndex, USHORT damage)
{
    if (enemyIndex != INVALID_ENTITY && enemyIndex < g_numEnemies)
    {
        ENEMY* e = &g_enemies[enemyIndex];

        if (e->CurrentState == ENEMY_STATE_IDLE)
        {
            damage *= 2; // double damage occurs if the enemy is standing idle and not activated
        }

        e->Health = max(0, e->Health - damage);
        if (e->Health > 0)
        {
            EnemySwitchToState(enemyIndex,  ENEMY_STATE_DAMAGE);
        }
        else
        {
            VEC2 position = e->Position;
            EnemySwitchToState(enemyIndex,  ENEMY_STATE_DYING);
            if (g_enemies[enemyIndex].Type == ENEMY_TYPE_BOSS)
            {
                LevelAddPowerup(position.x, position.y, PowerupCreate(position, POWERUP_TYPE_GOLD_KEY));
            }
            LevelAddPowerup(position.x, position.y, PowerupCreate(position, POWERUP_TYPE_AMMO));
        }
    }
}

VOID EnemyUpdateAll()
{
    USHORT i = 0;
    SHORT turn = 0;
    float angle = 0;
    BYTE randomAction = 0;
    USHORT cellIds[4] = {0};

    for (; i < g_numEnemies; ++i)
    {
        ENEMY* enemy = &g_enemies[i];
        BOOL animationEnded = FALSE;
        VEC2 movement = {0};
        float distance = 0;
        BOOL playerVisible = FALSE;

        if (!AnimationUpdate(&enemy->Animation))
        {
            AnimationInit(&enemy->Animation, 0, 0, FALSE);
            animationEnded = TRUE;
        }

        playerVisible = LevelRaycastPlayerScan(enemy->Position, enemy->Direction, &distance);
        if (playerVisible)
        {
            EnemyYellAtPlayer(i);
            animationEnded = animationEnded;
        }

        switch (enemy->CurrentState)
        {
        case ENEMY_STATE_IDLE:
            // If the enemy sees the player,  record player's direction and
            // transition to attack state.  Enemy will fire at player's last
            // direction the player was sighted. Otherwise, just hang out and
            // enjoy the dark smelly dungeon full of dead soldiers, skeletons
            // and stale TV dinners.
            if (playerVisible)
            {
                g_enemies[i].AttackDirection = g_enemies[i].Direction;
                if (g_enemies[i].Type != ENEMY_TYPE_DOG)
                {
                    EnemySwitchToState(i, ENEMY_STATE_ATTACK);
                }
            }
            break;

        case ENEMY_STATE_PATROL:
            {
                // Walk around constantly checking in front for doors to open.
                LevelClickInFront(g_enemies[i].Position, g_enemies[i].Direction);

                // If the enemy sees the player while on patrol,  record player's direction and
                // transition to attack state.  Enemy will fire at player's last
                // direction the player was sighted. Otherwise, enjoy roaming around
                // the dark smelly dungeon looking for dog food or some skeleton bones
                // to chew on.
                if (playerVisible)
                {
                    g_enemies[i].AttackDirection = g_enemies[i].Direction;
                    if (g_enemies[i].Type != ENEMY_TYPE_DOG)
                    {
                        EnemySwitchToState(i, ENEMY_STATE_ATTACK);
                    }
                }

                // If the enemy does not see the player, continue pursuing the player, or just patrolling around.
                else
                {
                    // Pursue the player
                    if (g_enemies[i].PersueMode)
                    {
                        LevelApplyDirectionChangeTowardPlayer(&g_enemies[i]);
                    }
                    // Follow patrolling level markers
                    else
                    {
                        LevelApplyPatrolDirection(&g_enemies[i]);
                    }

                    movement = Vec2Add(movement, Vec2Scale(g_enemies[i].Direction, PLAYER_SPEED/4 * g_elapsedTime));

                    if (Vec2LenSquared(movement) > 0.000001f)
                    {
                        movement = Vec2Scale(Vec2Normalize(movement), PLAYER_SPEED/4 * g_elapsedTime);
                        LevelRemoveEnemyFromGrid(i);
                        g_enemies[i].Position = LevelMoveWithCollision(g_enemies[i].Position, movement);
                        LevelAddEnemyToGrid(i);

                        // If an enemy has "patrolled" into a wall  and they were in persue mode, take them out of pursue mode
                        // and have them take a guard position with their back to the wall they just collided with.
                        if ((g_enemies[i].PersueMode) && g_enemies[i].LastPosition.x == g_enemies[i].Position.x && g_enemies[i].LastPosition.y == g_enemies[i].Position.y)
                        {
                            // stopped?, exit pursue mode
                            g_enemies[i].PersueMode = 0;
                            EnemySwitchToState(i, ENEMY_STATE_IDLE);
                            // Flip enemy to opposite facing
                            g_enemies[i].Direction.x = -g_enemies[i].Direction.x;
                            g_enemies[i].Direction.y = -g_enemies[i].Direction.y;
                        }

                        g_enemies[i].LastPosition = g_enemies[i].Position;
                    }
                }
            }
            break;

        case ENEMY_STATE_ATTACK:
            if (g_enemies[i].Type == ENEMY_TYPE_BLUEDUDE || g_enemies[i].Type == ENEMY_TYPE_ZOMBIE)
            {
                PlayWeaponSound(WEAPON_MACHINEGUN, FALSE);
            }

            if (g_enemies[i].Type == ENEMY_TYPE_BOSS)
            {
                PlayWeaponSound(WEAPON_CHAINGUN, TRUE);
            }

            if (animationEnded)
            {
                switch(g_enemies[i].Type)
                {
                case ENEMY_TYPE_OFFICER:
                case ENEMY_TYPE_SOLDIER:
                    PlayWeaponSound(WEAPON_PISTOL, TRUE);
                    break;
                case ENEMY_TYPE_ZOMBIE:
                    PlayWeaponSound(WEAPON_MACHINEGUN, TRUE);
                    break;
                case ENEMY_TYPE_DOG:
                    // Dog attack sound?
                    break;
                }

                EnemyAttackPlayer(i);
                EnemySwitchToState(i, ENEMY_STATE_PATROL);
                g_enemies[i].PersueMode = 1; // PursueMode enabled
            }
            break;

        case ENEMY_STATE_DAMAGE:
            if (animationEnded)
            {
                // If enemy has been shot, assume the enemy wants to pursue his attacker
                EnemySwitchToState(i, ENEMY_STATE_PATROL);
                g_enemies[i].PersueMode = 1; // PursueMode enabled
            }
            break;

        case ENEMY_STATE_DYING:
            if (animationEnded)
            {
                EnemySwitchToState(i, ENEMY_STATE_DEAD);
            }
            break;
        }
    }
}

HSPRITE EnemyGetSprite(USHORT enemyIndex, VEC2 playerPosition, VEC2 playerDirection)
{
    HSPRITE sprite;
    BYTE direction = 0;
    ENEMY* enemy;
    VEC2 toPlayer, perp;
    float dot, dot2, ang;

    if (enemyIndex == INVALID_ENTITY || enemyIndex > g_numEnemies)
        return INVALID_HSPRITE_VALUE;

    enemy = &g_enemies[enemyIndex];

    /* determine the side we should show the enemy from */
    enemy->Direction = Vec2Normalize(enemy->Direction);
    toPlayer = Vec2Normalize(Vec2Add(playerPosition, Vec2Neg(enemy->Position)));
    perp = Vec2Perp(toPlayer);
    dot = Vec2Dot(toPlayer, enemy->Direction);
    dot2 = Vec2Dot(perp, enemy->Direction);
    ang = acos(dot);

    ang *= 180.0f / 3.1415f; /* convert to degrees, approx. */
    if (dot2 > 0) ang = (360 - ang);
    ang /= 45.0f;
    direction = min(FACING_FORWARD + (SHORT)ang, 7);

    sprite = g_enemySprites[enemy->Type][enemy->CurrentState][AnimationGetFrame(&enemy->Animation)][direction];
    return sprite;
}

VOID EnemyYellAtPlayer(USHORT enemyIndex)
{
    ENEMY* enemy = &g_enemies[enemyIndex];
    if (!enemy->SaidHelloToPlayer && enemy->CurrentState != ENEMY_STATE_DEAD)
    {
        enemy->SaidHelloToPlayer = 1;
        PlayEnemyAlertSound(enemy->Type, TRUE);
    }
}

VOID EnemyAttackPlayer(USHORT enemyIndex)
{
    ENEMY* enemy = &g_enemies[enemyIndex];
    switch(enemy->Type)
    {
    case ENEMY_TYPE_SOLDIER:
        PlayerTakeDamage(3);
        break;
    case ENEMY_TYPE_BLUEDUDE:
        PlayerTakeDamage(8);
        break;
    case ENEMY_TYPE_DOG:
        PlayerTakeDamage(1);
        break;
    case ENEMY_TYPE_OFFICER:
        PlayerTakeDamage(6);
        break;
    case ENEMY_TYPE_ZOMBIE:
        PlayerTakeDamage(7);
        break;
    case ENEMY_TYPE_BOSS:
        PlayerTakeDamage(10);
        break;
    default:
        break;
    }
}

/* Helper methods */
VOID EnemySwitchToState(USHORT enemyIndex, BYTE state)
{
    ENEMY* enemy = &g_enemies[enemyIndex];

    // Let's add some randomness to attacks just to quickly
    // hack in some behaviors
    if (state == ENEMY_STATE_ATTACK)
    {
        if (((rand() % 10) != 5))
        {
            // hang roll back to a previous state
            state = enemy->CurrentState;
        }
    }

    switch (state)
    {
    case ENEMY_STATE_IDLE:
        AnimationInit(&enemy->Animation, 1, 1, TRUE);
        break;

    case ENEMY_STATE_PATROL:
        AnimationInit(&enemy->Animation, 1.0f, 4, TRUE);
        break;

    case ENEMY_STATE_ATTACK:
        AnimationInit(&enemy->Animation, 0.5f, 3, FALSE);
        break;

    case ENEMY_STATE_DAMAGE:
        AnimationInit(&enemy->Animation, 0.25f, 1, FALSE);
        break;

    case ENEMY_STATE_DYING:
        AnimationInit(&enemy->Animation, 0.5f, 4, FALSE);
        if (g_enemyDyingSounds[enemy->Type] != INVALID_HSOUND_VALUE)
        {
            AudioPlay(g_enemyDyingSounds[enemy->Type], enemy->Position);
        }
        break;

    case ENEMY_STATE_DEAD:
        AnimationInit(&enemy->Animation, 1, 1, TRUE);
        break;
    }

    enemy->CurrentState = state;
}

VOID LoadBoss1Resources()
{
    HSPRITE sprite;

    g_enemyDyingSounds[ENEMY_BOSS1] = AudioCreateSFX(CONTENT_ROOT"wavs\\BOSS1Dying.wav");

    /* IDLE */
    sprite = W3DCreateSprite(BOSS1_WALK_FRAME_0, FALSE);
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_IDLE][0][0] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_IDLE][0][1] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_IDLE][0][2] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_IDLE][0][3] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_IDLE][0][4] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_IDLE][0][5] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_IDLE][0][6] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_IDLE][0][7] = sprite;

    /* PATROL */
    sprite = W3DCreateSprite(BOSS1_WALK_FRAME_0, FALSE);
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_PATROL][0][0] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_PATROL][0][1] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_PATROL][0][2] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_PATROL][0][3] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_PATROL][0][4] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_PATROL][0][5] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_PATROL][0][6] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_PATROL][0][7] = sprite;

    sprite = W3DCreateSprite(BOSS1_WALK_FRAME_1, FALSE);
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_PATROL][1][0] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_PATROL][1][1] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_PATROL][1][2] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_PATROL][1][3] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_PATROL][1][4] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_PATROL][1][5] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_PATROL][1][6] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_PATROL][1][7] = sprite;

    sprite = W3DCreateSprite(BOSS1_WALK_FRAME_2, FALSE);
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_PATROL][2][0] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_PATROL][2][1] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_PATROL][2][2] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_PATROL][2][3] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_PATROL][2][4] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_PATROL][2][5] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_PATROL][2][6] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_PATROL][2][7] = sprite;

    sprite = W3DCreateSprite(BOSS1_WALK_FRAME_3, FALSE);
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_PATROL][3][0] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_PATROL][3][1] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_PATROL][3][2] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_PATROL][3][3] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_PATROL][3][4] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_PATROL][3][5] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_PATROL][3][6] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_PATROL][3][7] = sprite;

    /* ATTACK */
    sprite = W3DCreateSprite(BOSS1_ATTACK_FRAME_0, FALSE);
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_ATTACK][0][0] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_ATTACK][0][1] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_ATTACK][0][2] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_ATTACK][0][3] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_ATTACK][0][4] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_ATTACK][0][5] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_ATTACK][0][6] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_ATTACK][0][7] = sprite;

    sprite = W3DCreateSprite(BOSS1_ATTACK_FRAME_1, FALSE);
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_ATTACK][1][0] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_ATTACK][1][1] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_ATTACK][1][2] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_ATTACK][1][3] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_ATTACK][1][4] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_ATTACK][1][5] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_ATTACK][1][6] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_ATTACK][1][7] = sprite;

    sprite = W3DCreateSprite(BOSS1_ATTACK_FRAME_2, FALSE);
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_ATTACK][2][0] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_ATTACK][2][1] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_ATTACK][2][2] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_ATTACK][2][3] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_ATTACK][2][4] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_ATTACK][2][5] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_ATTACK][2][6] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_ATTACK][2][7] = sprite;

    /* DAMAGE */
    sprite = W3DCreateSprite(BOSS1_WALK_FRAME_0, FALSE);
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_DAMAGE][0][0] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_DAMAGE][0][1] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_DAMAGE][0][2] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_DAMAGE][0][3] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_DAMAGE][0][4] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_DAMAGE][0][5] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_DAMAGE][0][6] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_DAMAGE][0][7] = sprite;

    /* DYING */
    sprite = W3DCreateSprite(BOSS1_DEATH_FRAME_1, FALSE);
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_DYING][0][0] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_DYING][0][1] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_DYING][0][2] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_DYING][0][3] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_DYING][0][4] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_DYING][0][5] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_DYING][0][6] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_DYING][0][7] = sprite;

    sprite = W3DCreateSprite(BOSS1_DEATH_FRAME_2, FALSE);
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_DYING][1][0] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_DYING][1][1] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_DYING][1][2] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_DYING][1][3] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_DYING][1][4] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_DYING][1][5] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_DYING][1][6] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_DYING][1][7] = sprite;

    sprite = W3DCreateSprite(BOSS1_DEATH_FRAME_2, FALSE);
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_DYING][2][0] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_DYING][2][1] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_DYING][2][2] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_DYING][2][3] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_DYING][2][4] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_DYING][2][5] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_DYING][2][6] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_DYING][2][7] = sprite;

    sprite = W3DCreateSprite(BOSS1_DEAD, FALSE);
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_DYING][3][0] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_DYING][3][1] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_DYING][3][2] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_DYING][3][3] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_DYING][3][4] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_DYING][3][5] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_DYING][3][6] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_DYING][3][7] = sprite;

    /* DEAD */
    sprite = W3DCreateSprite(BOSS1_DEAD, FALSE);
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_DEAD][0][0] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_DEAD][0][1] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_DEAD][0][2] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_DEAD][0][3] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_DEAD][0][4] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_DEAD][0][5] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_DEAD][0][6] = sprite;
    g_enemySprites[ENEMY_BOSS1][ENEMY_STATE_DEAD][0][7] = sprite;
}

VOID LoadSoldierResources()
{
    HSPRITE sprite;

    g_enemyDyingSounds[ENEMY_SOLDIER] = AudioCreateSFX(CONTENT_ROOT"wavs\\SoldierDying.wav");

    /* Soldier IDLE */
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_IDLE][0][0] = W3DCreateSprite(SOLDIER_STAND_FRAME_0  , FALSE);
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_IDLE][0][1] = W3DCreateSprite(SOLDIER_STAND_FRAME_45 , FALSE);
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_IDLE][0][2] = W3DCreateSprite(SOLDIER_STAND_FRAME_90 , FALSE);
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_IDLE][0][3] = W3DCreateSprite(SOLDIER_STAND_FRAME_135, FALSE);
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_IDLE][0][4] = W3DCreateSprite(SOLDIER_STAND_FRAME_180, FALSE);
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_IDLE][0][5] = W3DCreateSprite(SOLDIER_STAND_FRAME_225, FALSE);
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_IDLE][0][6] = W3DCreateSprite(SOLDIER_STAND_FRAME_270, FALSE);
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_IDLE][0][7] = W3DCreateSprite(SOLDIER_STAND_FRAME_315, FALSE);

    /* Soldier PATROL */
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_PATROL][0][0] = W3DCreateSprite(SOLDIER_PATROL_LFOOT_FRAME_0  , FALSE);
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_PATROL][0][1] = W3DCreateSprite(SOLDIER_PATROL_LFOOT_FRAME_45 , FALSE);
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_PATROL][0][2] = W3DCreateSprite(SOLDIER_PATROL_LFOOT_FRAME_90 , FALSE);
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_PATROL][0][3] = W3DCreateSprite(SOLDIER_PATROL_LFOOT_FRAME_135, FALSE);
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_PATROL][0][4] = W3DCreateSprite(SOLDIER_PATROL_LFOOT_FRAME_180, FALSE);
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_PATROL][0][5] = W3DCreateSprite(SOLDIER_PATROL_LFOOT_FRAME_225, FALSE);
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_PATROL][0][6] = W3DCreateSprite(SOLDIER_PATROL_LFOOT_FRAME_270, FALSE);
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_PATROL][0][7] = W3DCreateSprite(SOLDIER_PATROL_LFOOT_FRAME_315, FALSE);

    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_PATROL][1][0] = W3DCreateSprite(SOLDIER_PATROL_RFOOT_FRAME_0  , FALSE);
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_PATROL][1][1] = W3DCreateSprite(SOLDIER_PATROL_RFOOT_FRAME_45 , FALSE);
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_PATROL][1][2] = W3DCreateSprite(SOLDIER_PATROL_RFOOT_FRAME_90 , FALSE);
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_PATROL][1][3] = W3DCreateSprite(SOLDIER_PATROL_RFOOT_FRAME_135, FALSE);
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_PATROL][1][4] = W3DCreateSprite(SOLDIER_PATROL_RFOOT_FRAME_180, FALSE);
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_PATROL][1][5] = W3DCreateSprite(SOLDIER_PATROL_RFOOT_FRAME_225, FALSE);
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_PATROL][1][6] = W3DCreateSprite(SOLDIER_PATROL_RFOOT_FRAME_270, FALSE);
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_PATROL][1][7] = W3DCreateSprite(SOLDIER_PATROL_RFOOT_FRAME_315, FALSE);

    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_PATROL][2][0] = W3DCreateSprite(SOLDIER_RUN_RFOOT_FRAME_0  , FALSE);
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_PATROL][2][1] = W3DCreateSprite(SOLDIER_RUN_RFOOT_FRAME_45 , FALSE);
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_PATROL][2][2] = W3DCreateSprite(SOLDIER_RUN_RFOOT_FRAME_90 , FALSE);
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_PATROL][2][3] = W3DCreateSprite(SOLDIER_RUN_RFOOT_FRAME_135, FALSE);
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_PATROL][2][4] = W3DCreateSprite(SOLDIER_RUN_RFOOT_FRAME_180, FALSE);
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_PATROL][2][5] = W3DCreateSprite(SOLDIER_RUN_RFOOT_FRAME_225, FALSE);
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_PATROL][2][6] = W3DCreateSprite(SOLDIER_RUN_RFOOT_FRAME_270, FALSE);
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_PATROL][2][7] = W3DCreateSprite(SOLDIER_RUN_RFOOT_FRAME_315, FALSE);

    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_PATROL][3][0] = W3DCreateSprite(SOLDIER_RUN_LFOOT_FRAME_0  , FALSE);
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_PATROL][3][1] = W3DCreateSprite(SOLDIER_RUN_LFOOT_FRAME_45 , FALSE);
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_PATROL][3][2] = W3DCreateSprite(SOLDIER_RUN_LFOOT_FRAME_90 , FALSE);
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_PATROL][3][3] = W3DCreateSprite(SOLDIER_RUN_LFOOT_FRAME_135, FALSE);
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_PATROL][3][4] = W3DCreateSprite(SOLDIER_RUN_LFOOT_FRAME_180, FALSE);
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_PATROL][3][5] = W3DCreateSprite(SOLDIER_RUN_LFOOT_FRAME_225, FALSE);
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_PATROL][3][6] = W3DCreateSprite(SOLDIER_RUN_LFOOT_FRAME_270, FALSE);
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_PATROL][3][7] = W3DCreateSprite(SOLDIER_RUN_LFOOT_FRAME_315, FALSE);

    /* Soldier ATTACK */
    sprite = W3DCreateSprite(SOLDIER_ATTACK_FRAME_0, FALSE);
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_ATTACK][0][0] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_ATTACK][0][1] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_ATTACK][0][2] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_ATTACK][0][3] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_ATTACK][0][4] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_ATTACK][0][5] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_ATTACK][0][6] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_ATTACK][0][7] = sprite;

    sprite = W3DCreateSprite(SOLDIER_ATTACK_FRAME_1, FALSE);
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_ATTACK][1][0] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_ATTACK][1][1] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_ATTACK][1][2] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_ATTACK][1][3] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_ATTACK][1][4] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_ATTACK][1][5] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_ATTACK][1][6] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_ATTACK][1][7] = sprite;

    sprite = W3DCreateSprite(SOLDIER_ATTACK_FRAME_2, FALSE);
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_ATTACK][2][0] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_ATTACK][2][1] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_ATTACK][2][2] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_ATTACK][2][3] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_ATTACK][2][4] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_ATTACK][2][5] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_ATTACK][2][6] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_ATTACK][2][7] = sprite;

    /* Soldier DAMAGE */
    sprite = W3DCreateSprite(SOLDIER_DAMAGE_TAKEN, FALSE);
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_DAMAGE][0][0] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_DAMAGE][0][1] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_DAMAGE][0][2] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_DAMAGE][0][3] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_DAMAGE][0][4] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_DAMAGE][0][5] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_DAMAGE][0][6] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_DAMAGE][0][7] = sprite;

    /* Soldier DYING */
    sprite = W3DCreateSprite(SOLDIER_DEATH_FRAME_0, FALSE);
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_DYING][0][0] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_DYING][0][1] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_DYING][0][2] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_DYING][0][3] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_DYING][0][4] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_DYING][0][5] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_DYING][0][6] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_DYING][0][7] = sprite;

    sprite = W3DCreateSprite(SOLDIER_DEATH_FRAME_1, FALSE);
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_DYING][1][0] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_DYING][1][1] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_DYING][1][2] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_DYING][1][3] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_DYING][1][4] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_DYING][1][5] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_DYING][1][6] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_DYING][1][7] = sprite;

    sprite = W3DCreateSprite(SOLDIER_DEATH_FRAME_2, FALSE);
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_DYING][2][0] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_DYING][2][1] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_DYING][2][2] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_DYING][2][3] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_DYING][2][4] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_DYING][2][5] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_DYING][2][6] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_DYING][2][7] = sprite;

    sprite = W3DCreateSprite(SOLDIER_DEATH_FRAME_3, FALSE);
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_DYING][3][0] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_DYING][3][1] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_DYING][3][2] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_DYING][3][3] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_DYING][3][4] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_DYING][3][5] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_DYING][3][6] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_DYING][3][7] = sprite;

    /* Soldier DEAD */
    sprite = W3DCreateSprite(SOLDIER_DEAD, FALSE);
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_DEAD][0][0] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_DEAD][0][1] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_DEAD][0][2] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_DEAD][0][3] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_DEAD][0][4] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_DEAD][0][5] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_DEAD][0][6] = sprite;
    g_enemySprites[ENEMY_SOLDIER][ENEMY_STATE_DEAD][0][7] = sprite;
}

VOID LoadDogResources()
{
    HSPRITE sprite;

    g_enemyDyingSounds[ENEMY_DOG] = AudioCreateSFX(CONTENT_ROOT"wavs\\DogDying.wav");

    /* IDLE */
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_IDLE][0][0] = W3DCreateSprite(DOG_STAND_FRAME_0  , FALSE);
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_IDLE][0][1] = W3DCreateSprite(DOG_STAND_FRAME_45 , FALSE);
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_IDLE][0][2] = W3DCreateSprite(DOG_STAND_FRAME_90 , FALSE);
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_IDLE][0][3] = W3DCreateSprite(DOG_STAND_FRAME_135, FALSE);
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_IDLE][0][4] = W3DCreateSprite(DOG_STAND_FRAME_180, FALSE);
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_IDLE][0][5] = W3DCreateSprite(DOG_STAND_FRAME_225, FALSE);
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_IDLE][0][6] = W3DCreateSprite(DOG_STAND_FRAME_270, FALSE);
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_IDLE][0][7] = W3DCreateSprite(DOG_STAND_FRAME_315, FALSE);

    /* PATROL */
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_PATROL][0][0] = W3DCreateSprite(DOG_STAND_FRAME_0  , FALSE);
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_PATROL][0][1] = W3DCreateSprite(DOG_STAND_FRAME_45 , FALSE);
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_PATROL][0][2] = W3DCreateSprite(DOG_STAND_FRAME_90 , FALSE);
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_PATROL][0][3] = W3DCreateSprite(DOG_STAND_FRAME_135, FALSE);
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_PATROL][0][4] = W3DCreateSprite(DOG_STAND_FRAME_180, FALSE);
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_PATROL][0][5] = W3DCreateSprite(DOG_STAND_FRAME_225, FALSE);
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_PATROL][0][6] = W3DCreateSprite(DOG_STAND_FRAME_270, FALSE);
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_PATROL][0][7] = W3DCreateSprite(DOG_STAND_FRAME_315, FALSE);

    g_enemySprites[ENEMY_DOG][ENEMY_STATE_PATROL][1][0] = W3DCreateSprite(DOG_PATROL_LFOOT_FRAME_0  , FALSE);
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_PATROL][1][1] = W3DCreateSprite(DOG_PATROL_LFOOT_FRAME_45 , FALSE);
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_PATROL][1][2] = W3DCreateSprite(DOG_PATROL_LFOOT_FRAME_90 , FALSE);
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_PATROL][1][3] = W3DCreateSprite(DOG_PATROL_LFOOT_FRAME_135, FALSE);
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_PATROL][1][4] = W3DCreateSprite(DOG_PATROL_LFOOT_FRAME_180, FALSE);
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_PATROL][1][5] = W3DCreateSprite(DOG_PATROL_LFOOT_FRAME_225, FALSE);
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_PATROL][1][6] = W3DCreateSprite(DOG_PATROL_LFOOT_FRAME_270, FALSE);
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_PATROL][1][7] = W3DCreateSprite(DOG_PATROL_LFOOT_FRAME_315, FALSE);

    g_enemySprites[ENEMY_DOG][ENEMY_STATE_PATROL][2][0] = W3DCreateSprite(DOG_PATROL_RFOOT_FRAME_0  , FALSE);
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_PATROL][2][1] = W3DCreateSprite(DOG_PATROL_RFOOT_FRAME_45 , FALSE);
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_PATROL][2][2] = W3DCreateSprite(DOG_PATROL_RFOOT_FRAME_90 , FALSE);
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_PATROL][2][3] = W3DCreateSprite(DOG_PATROL_RFOOT_FRAME_135, FALSE);
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_PATROL][2][4] = W3DCreateSprite(DOG_PATROL_RFOOT_FRAME_180, FALSE);
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_PATROL][2][5] = W3DCreateSprite(DOG_PATROL_RFOOT_FRAME_225, FALSE);
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_PATROL][2][6] = W3DCreateSprite(DOG_PATROL_RFOOT_FRAME_270, FALSE);
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_PATROL][2][7] = W3DCreateSprite(DOG_PATROL_RFOOT_FRAME_315, FALSE);

    g_enemySprites[ENEMY_DOG][ENEMY_STATE_PATROL][3][0] = W3DCreateSprite(DOG_RUN_RFOOT_FRAME_0  , FALSE);
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_PATROL][3][1] = W3DCreateSprite(DOG_RUN_RFOOT_FRAME_45 , FALSE);
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_PATROL][3][2] = W3DCreateSprite(DOG_RUN_RFOOT_FRAME_90 , FALSE);
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_PATROL][3][3] = W3DCreateSprite(DOG_RUN_RFOOT_FRAME_135, FALSE);
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_PATROL][3][4] = W3DCreateSprite(DOG_RUN_RFOOT_FRAME_180, FALSE);
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_PATROL][3][5] = W3DCreateSprite(DOG_RUN_RFOOT_FRAME_225, FALSE);
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_PATROL][3][6] = W3DCreateSprite(DOG_RUN_RFOOT_FRAME_270, FALSE);
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_PATROL][3][7] = W3DCreateSprite(DOG_RUN_RFOOT_FRAME_315, FALSE);

    /* ATTACK */
    sprite = W3DCreateSprite(DOG_ATTACK_FRAME_0, FALSE);
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_ATTACK][0][0] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_ATTACK][0][1] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_ATTACK][0][2] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_ATTACK][0][3] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_ATTACK][0][4] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_ATTACK][0][5] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_ATTACK][0][6] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_ATTACK][0][7] = sprite;

    sprite = W3DCreateSprite(DOG_ATTACK_FRAME_1, FALSE);
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_ATTACK][1][0] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_ATTACK][1][1] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_ATTACK][1][2] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_ATTACK][1][3] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_ATTACK][1][4] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_ATTACK][1][5] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_ATTACK][1][6] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_ATTACK][1][7] = sprite;

    sprite = W3DCreateSprite(DOG_ATTACK_FRAME_2, FALSE);
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_ATTACK][2][0] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_ATTACK][2][1] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_ATTACK][2][2] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_ATTACK][2][3] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_ATTACK][2][4] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_ATTACK][2][5] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_ATTACK][2][6] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_ATTACK][2][7] = sprite;

    /* DAMAGE */
    sprite = W3DCreateSprite(DOG_DEATH_FRAME_0, FALSE);
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_DAMAGE][0][0] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_DAMAGE][0][1] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_DAMAGE][0][2] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_DAMAGE][0][3] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_DAMAGE][0][4] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_DAMAGE][0][5] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_DAMAGE][0][6] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_DAMAGE][0][7] = sprite;

    /* DYING */
    sprite = W3DCreateSprite(DOG_DEATH_FRAME_0, FALSE);
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_DYING][0][0] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_DYING][0][1] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_DYING][0][2] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_DYING][0][3] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_DYING][0][4] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_DYING][0][5] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_DYING][0][6] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_DYING][0][7] = sprite;

    sprite = W3DCreateSprite(DOG_DEATH_FRAME_1, FALSE);
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_DYING][1][0] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_DYING][1][1] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_DYING][1][2] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_DYING][1][3] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_DYING][1][4] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_DYING][1][5] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_DYING][1][6] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_DYING][1][7] = sprite;

    sprite = W3DCreateSprite(DOG_DEATH_FRAME_2, FALSE);
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_DYING][2][0] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_DYING][2][1] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_DYING][2][2] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_DYING][2][3] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_DYING][2][4] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_DYING][2][5] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_DYING][2][6] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_DYING][2][7] = sprite;

    sprite = W3DCreateSprite(DOG_DEAD, FALSE);
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_DYING][3][0] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_DYING][3][1] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_DYING][3][2] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_DYING][3][3] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_DYING][3][4] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_DYING][3][5] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_DYING][3][6] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_DYING][3][7] = sprite;

    /* DEAD */
    sprite = W3DCreateSprite(DOG_DEAD, FALSE);
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_DEAD][0][0] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_DEAD][0][1] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_DEAD][0][2] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_DEAD][0][3] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_DEAD][0][4] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_DEAD][0][5] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_DEAD][0][6] = sprite;
    g_enemySprites[ENEMY_DOG][ENEMY_STATE_DEAD][0][7] = sprite;
}

VOID LoadBlueDudeResources()
{
    HSPRITE sprite;

    g_enemyDyingSounds[ENEMY_BLUEDUDE] = AudioCreateSFX(CONTENT_ROOT"wavs\\SSDying.wav");

    /* IDLE */
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_IDLE][0][0] = W3DCreateSprite(SS_STAND_FRAME_0  , FALSE);
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_IDLE][0][1] = W3DCreateSprite(SS_STAND_FRAME_45 , FALSE);
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_IDLE][0][2] = W3DCreateSprite(SS_STAND_FRAME_90 , FALSE);
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_IDLE][0][3] = W3DCreateSprite(SS_STAND_FRAME_135, FALSE);
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_IDLE][0][4] = W3DCreateSprite(SS_STAND_FRAME_180, FALSE);
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_IDLE][0][5] = W3DCreateSprite(SS_STAND_FRAME_225, FALSE);
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_IDLE][0][6] = W3DCreateSprite(SS_STAND_FRAME_270, FALSE);
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_IDLE][0][7] = W3DCreateSprite(SS_STAND_FRAME_315, FALSE);

    /* PATROL */
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_PATROL][0][0] = W3DCreateSprite(SS_PATROL_LFOOT_FRAME_0  , FALSE);
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_PATROL][0][1] = W3DCreateSprite(SS_PATROL_LFOOT_FRAME_45 , FALSE);
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_PATROL][0][2] = W3DCreateSprite(SS_PATROL_LFOOT_FRAME_90 , FALSE);
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_PATROL][0][3] = W3DCreateSprite(SS_PATROL_LFOOT_FRAME_135, FALSE);
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_PATROL][0][4] = W3DCreateSprite(SS_PATROL_LFOOT_FRAME_180, FALSE);
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_PATROL][0][5] = W3DCreateSprite(SS_PATROL_LFOOT_FRAME_225, FALSE);
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_PATROL][0][6] = W3DCreateSprite(SS_PATROL_LFOOT_FRAME_270, FALSE);
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_PATROL][0][7] = W3DCreateSprite(SS_PATROL_LFOOT_FRAME_315, FALSE);

    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_PATROL][1][0] = W3DCreateSprite(SS_PATROL_RFOOT_FRAME_0  , FALSE);
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_PATROL][1][1] = W3DCreateSprite(SS_PATROL_RFOOT_FRAME_45 , FALSE);
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_PATROL][1][2] = W3DCreateSprite(SS_PATROL_RFOOT_FRAME_90 , FALSE);
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_PATROL][1][3] = W3DCreateSprite(SS_PATROL_RFOOT_FRAME_135, FALSE);
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_PATROL][1][4] = W3DCreateSprite(SS_PATROL_RFOOT_FRAME_180, FALSE);
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_PATROL][1][5] = W3DCreateSprite(SS_PATROL_RFOOT_FRAME_225, FALSE);
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_PATROL][1][6] = W3DCreateSprite(SS_PATROL_RFOOT_FRAME_270, FALSE);
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_PATROL][1][7] = W3DCreateSprite(SS_PATROL_RFOOT_FRAME_315, FALSE);

    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_PATROL][2][0] = W3DCreateSprite(SS_RUN_RFOOT_FRAME_0  , FALSE);
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_PATROL][2][1] = W3DCreateSprite(SS_RUN_RFOOT_FRAME_45 , FALSE);
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_PATROL][2][2] = W3DCreateSprite(SS_RUN_RFOOT_FRAME_90 , FALSE);
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_PATROL][2][3] = W3DCreateSprite(SS_RUN_RFOOT_FRAME_135, FALSE);
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_PATROL][2][4] = W3DCreateSprite(SS_RUN_RFOOT_FRAME_180, FALSE);
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_PATROL][2][5] = W3DCreateSprite(SS_RUN_RFOOT_FRAME_225, FALSE);
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_PATROL][2][6] = W3DCreateSprite(SS_RUN_RFOOT_FRAME_270, FALSE);
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_PATROL][2][7] = W3DCreateSprite(SS_RUN_RFOOT_FRAME_315, FALSE);

    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_PATROL][3][0] = W3DCreateSprite(SS_RUN_LFOOT_FRAME_0  , FALSE);
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_PATROL][3][1] = W3DCreateSprite(SS_RUN_LFOOT_FRAME_45 , FALSE);
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_PATROL][3][2] = W3DCreateSprite(SS_RUN_LFOOT_FRAME_90 , FALSE);
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_PATROL][3][3] = W3DCreateSprite(SS_RUN_LFOOT_FRAME_135, FALSE);
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_PATROL][3][4] = W3DCreateSprite(SS_RUN_LFOOT_FRAME_180, FALSE);
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_PATROL][3][5] = W3DCreateSprite(SS_RUN_LFOOT_FRAME_225, FALSE);
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_PATROL][3][6] = W3DCreateSprite(SS_RUN_LFOOT_FRAME_270, FALSE);
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_PATROL][3][7] = W3DCreateSprite(SS_RUN_LFOOT_FRAME_315, FALSE);

    /* ATTACK */
    sprite = W3DCreateSprite(SS_ATTACK_FRAME_0, FALSE);
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_ATTACK][0][0] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_ATTACK][0][1] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_ATTACK][0][2] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_ATTACK][0][3] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_ATTACK][0][4] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_ATTACK][0][5] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_ATTACK][0][6] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_ATTACK][0][7] = sprite;

    sprite = W3DCreateSprite(SS_ATTACK_FRAME_1, FALSE);
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_ATTACK][1][0] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_ATTACK][1][1] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_ATTACK][1][2] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_ATTACK][1][3] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_ATTACK][1][4] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_ATTACK][1][5] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_ATTACK][1][6] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_ATTACK][1][7] = sprite;

    sprite = W3DCreateSprite(SS_ATTACK_FRAME_2, FALSE);
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_ATTACK][2][0] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_ATTACK][2][1] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_ATTACK][2][2] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_ATTACK][2][3] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_ATTACK][2][4] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_ATTACK][2][5] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_ATTACK][2][6] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_ATTACK][2][7] = sprite;

    /* DAMAGE */
    sprite = W3DCreateSprite(SS_DAMAGE_TAKEN, FALSE);
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_DAMAGE][0][0] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_DAMAGE][0][1] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_DAMAGE][0][2] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_DAMAGE][0][3] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_DAMAGE][0][4] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_DAMAGE][0][5] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_DAMAGE][0][6] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_DAMAGE][0][7] = sprite;

    /* DYING */
    sprite = W3DCreateSprite(SS_DEATH_FRAME_1, FALSE);
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_DYING][0][0] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_DYING][0][1] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_DYING][0][2] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_DYING][0][3] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_DYING][0][4] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_DYING][0][5] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_DYING][0][6] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_DYING][0][7] = sprite;

    sprite = W3DCreateSprite(SS_DEATH_FRAME_2, FALSE);
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_DYING][1][0] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_DYING][1][1] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_DYING][1][2] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_DYING][1][3] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_DYING][1][4] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_DYING][1][5] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_DYING][1][6] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_DYING][1][7] = sprite;

    sprite = W3DCreateSprite(SS_DEATH_FRAME_3, FALSE);
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_DYING][2][0] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_DYING][2][1] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_DYING][2][2] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_DYING][2][3] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_DYING][2][4] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_DYING][2][5] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_DYING][2][6] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_DYING][2][7] = sprite;

    sprite = W3DCreateSprite(SS_DEAD, FALSE);
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_DYING][3][0] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_DYING][3][1] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_DYING][3][2] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_DYING][3][3] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_DYING][3][4] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_DYING][3][5] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_DYING][3][6] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_DYING][3][7] = sprite;

    /* DEAD */
    sprite = W3DCreateSprite(SS_DEAD, FALSE);
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_DEAD][0][0] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_DEAD][0][1] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_DEAD][0][2] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_DEAD][0][3] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_DEAD][0][4] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_DEAD][0][5] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_DEAD][0][6] = sprite;
    g_enemySprites[ENEMY_BLUEDUDE][ENEMY_STATE_DEAD][0][7] = sprite;
}

VOID LoadZombieResources()
{
    HSPRITE sprite;

    g_enemyDyingSounds[ENEMY_ZOMBIE] = AudioCreateSFX(CONTENT_ROOT"wavs\\ZombieDying.wav");

    /* ZOMBIE IDLE */
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_IDLE][0][0] = W3DCreateSprite(ZOMBIE_STAND_FRAME_0  , FALSE);
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_IDLE][0][1] = W3DCreateSprite(ZOMBIE_STAND_FRAME_45 , FALSE);
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_IDLE][0][2] = W3DCreateSprite(ZOMBIE_STAND_FRAME_90 , FALSE);
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_IDLE][0][3] = W3DCreateSprite(ZOMBIE_STAND_FRAME_135, FALSE);
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_IDLE][0][4] = W3DCreateSprite(ZOMBIE_STAND_FRAME_180, FALSE);
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_IDLE][0][5] = W3DCreateSprite(ZOMBIE_STAND_FRAME_225, FALSE);
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_IDLE][0][6] = W3DCreateSprite(ZOMBIE_STAND_FRAME_270, FALSE);
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_IDLE][0][7] = W3DCreateSprite(ZOMBIE_STAND_FRAME_315, FALSE);

    /* ZOMBIE PATROL */
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_PATROL][0][0] = W3DCreateSprite(ZOMBIE_PATROL_LFOOT_FRAME_0  , FALSE);
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_PATROL][0][1] = W3DCreateSprite(ZOMBIE_PATROL_LFOOT_FRAME_45 , FALSE);
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_PATROL][0][2] = W3DCreateSprite(ZOMBIE_PATROL_LFOOT_FRAME_90 , FALSE);
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_PATROL][0][3] = W3DCreateSprite(ZOMBIE_PATROL_LFOOT_FRAME_135, FALSE);
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_PATROL][0][4] = W3DCreateSprite(ZOMBIE_PATROL_LFOOT_FRAME_180, FALSE);
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_PATROL][0][5] = W3DCreateSprite(ZOMBIE_PATROL_LFOOT_FRAME_225, FALSE);
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_PATROL][0][6] = W3DCreateSprite(ZOMBIE_PATROL_LFOOT_FRAME_270, FALSE);
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_PATROL][0][7] = W3DCreateSprite(ZOMBIE_PATROL_LFOOT_FRAME_315, FALSE);

    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_PATROL][1][0] = W3DCreateSprite(ZOMBIE_PATROL_RFOOT_FRAME_0  , FALSE);
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_PATROL][1][1] = W3DCreateSprite(ZOMBIE_PATROL_RFOOT_FRAME_45 , FALSE);
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_PATROL][1][2] = W3DCreateSprite(ZOMBIE_PATROL_RFOOT_FRAME_90 , FALSE);
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_PATROL][1][3] = W3DCreateSprite(ZOMBIE_PATROL_RFOOT_FRAME_135, FALSE);
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_PATROL][1][4] = W3DCreateSprite(ZOMBIE_PATROL_RFOOT_FRAME_180, FALSE);
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_PATROL][1][5] = W3DCreateSprite(ZOMBIE_PATROL_RFOOT_FRAME_225, FALSE);
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_PATROL][1][6] = W3DCreateSprite(ZOMBIE_PATROL_RFOOT_FRAME_270, FALSE);
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_PATROL][1][7] = W3DCreateSprite(ZOMBIE_PATROL_RFOOT_FRAME_315, FALSE);

    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_PATROL][2][0] = W3DCreateSprite(ZOMBIE_RUN_RFOOT_FRAME_0  , FALSE);
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_PATROL][2][1] = W3DCreateSprite(ZOMBIE_RUN_RFOOT_FRAME_45 , FALSE);
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_PATROL][2][2] = W3DCreateSprite(ZOMBIE_RUN_RFOOT_FRAME_90 , FALSE);
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_PATROL][2][3] = W3DCreateSprite(ZOMBIE_RUN_RFOOT_FRAME_135, FALSE);
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_PATROL][2][4] = W3DCreateSprite(ZOMBIE_RUN_RFOOT_FRAME_180, FALSE);
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_PATROL][2][5] = W3DCreateSprite(ZOMBIE_RUN_RFOOT_FRAME_225, FALSE);
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_PATROL][2][6] = W3DCreateSprite(ZOMBIE_RUN_RFOOT_FRAME_270, FALSE);
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_PATROL][2][7] = W3DCreateSprite(ZOMBIE_RUN_RFOOT_FRAME_315, FALSE);

    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_PATROL][3][0] = W3DCreateSprite(ZOMBIE_RUN_LFOOT_FRAME_0  , FALSE);
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_PATROL][3][1] = W3DCreateSprite(ZOMBIE_RUN_LFOOT_FRAME_45 , FALSE);
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_PATROL][3][2] = W3DCreateSprite(ZOMBIE_RUN_LFOOT_FRAME_90 , FALSE);
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_PATROL][3][3] = W3DCreateSprite(ZOMBIE_RUN_LFOOT_FRAME_135, FALSE);
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_PATROL][3][4] = W3DCreateSprite(ZOMBIE_RUN_LFOOT_FRAME_180, FALSE);
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_PATROL][3][5] = W3DCreateSprite(ZOMBIE_RUN_LFOOT_FRAME_225, FALSE);
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_PATROL][3][6] = W3DCreateSprite(ZOMBIE_RUN_LFOOT_FRAME_270, FALSE);
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_PATROL][3][7] = W3DCreateSprite(ZOMBIE_RUN_LFOOT_FRAME_315, FALSE);

    /* ZOMBIE ATTACK */
    sprite = W3DCreateSprite(ZOMBIE_ATTACK_FRAME_0, FALSE);
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_ATTACK][0][0] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_ATTACK][0][1] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_ATTACK][0][2] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_ATTACK][0][3] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_ATTACK][0][4] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_ATTACK][0][5] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_ATTACK][0][6] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_ATTACK][0][7] = sprite;

    sprite = W3DCreateSprite(ZOMBIE_ATTACK_FRAME_1, FALSE);
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_ATTACK][1][0] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_ATTACK][1][1] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_ATTACK][1][2] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_ATTACK][1][3] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_ATTACK][1][4] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_ATTACK][1][5] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_ATTACK][1][6] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_ATTACK][1][7] = sprite;

    sprite = W3DCreateSprite(ZOMBIE_ATTACK_FRAME_2, FALSE);
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_ATTACK][2][0] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_ATTACK][2][1] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_ATTACK][2][2] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_ATTACK][2][3] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_ATTACK][2][4] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_ATTACK][2][5] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_ATTACK][2][6] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_ATTACK][2][7] = sprite;

    /* ZOMBIE DAMAGE */
    sprite = W3DCreateSprite(ZOMBIE_DAMAGE_TAKEN, FALSE);
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_DAMAGE][0][0] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_DAMAGE][0][1] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_DAMAGE][0][2] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_DAMAGE][0][3] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_DAMAGE][0][4] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_DAMAGE][0][5] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_DAMAGE][0][6] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_DAMAGE][0][7] = sprite;

    /* ZOMBIE DYING */
    sprite = W3DCreateSprite(ZOMBIE_DEATH_FRAME_0, FALSE);
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_DYING][0][0] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_DYING][0][1] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_DYING][0][2] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_DYING][0][3] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_DYING][0][4] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_DYING][0][5] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_DYING][0][6] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_DYING][0][7] = sprite;

    sprite = W3DCreateSprite(ZOMBIE_DEATH_FRAME_1, FALSE);
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_DYING][1][0] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_DYING][1][1] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_DYING][1][2] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_DYING][1][3] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_DYING][1][4] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_DYING][1][5] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_DYING][1][6] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_DYING][1][7] = sprite;

    sprite = W3DCreateSprite(ZOMBIE_DEATH_FRAME_2, FALSE);
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_DYING][2][0] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_DYING][2][1] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_DYING][2][2] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_DYING][2][3] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_DYING][2][4] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_DYING][2][5] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_DYING][2][6] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_DYING][2][7] = sprite;

    sprite = W3DCreateSprite(ZOMBIE_DEATH_FRAME_3, FALSE);
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_DYING][3][0] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_DYING][3][1] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_DYING][3][2] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_DYING][3][3] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_DYING][3][4] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_DYING][3][5] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_DYING][3][6] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_DYING][3][7] = sprite;

    /* ZOMBIE DEAD */
    sprite = W3DCreateSprite(ZOMBIE_DEAD, FALSE);
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_DEAD][0][0] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_DEAD][0][1] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_DEAD][0][2] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_DEAD][0][3] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_DEAD][0][4] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_DEAD][0][5] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_DEAD][0][6] = sprite;
    g_enemySprites[ENEMY_ZOMBIE][ENEMY_STATE_DEAD][0][7] = sprite;
}

VOID LoadOfficerResources()
{
    HSPRITE sprite;

    g_enemyDyingSounds[ENEMY_OFFICER] = AudioCreateSFX(CONTENT_ROOT"wavs\\OfficerDying.wav");

    /* OFFICER IDLE */
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_IDLE][0][0] = W3DCreateSprite(OFFICER_STAND_FRAME_0  , FALSE);
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_IDLE][0][1] = W3DCreateSprite(OFFICER_STAND_FRAME_45 , FALSE);
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_IDLE][0][2] = W3DCreateSprite(OFFICER_STAND_FRAME_90 , FALSE);
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_IDLE][0][3] = W3DCreateSprite(OFFICER_STAND_FRAME_135, FALSE);
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_IDLE][0][4] = W3DCreateSprite(OFFICER_STAND_FRAME_180, FALSE);
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_IDLE][0][5] = W3DCreateSprite(OFFICER_STAND_FRAME_225, FALSE);
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_IDLE][0][6] = W3DCreateSprite(OFFICER_STAND_FRAME_270, FALSE);
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_IDLE][0][7] = W3DCreateSprite(OFFICER_STAND_FRAME_315, FALSE);

    /* OFFICER PATROL */
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_PATROL][0][0] = W3DCreateSprite(OFFICER_PATROL_LFOOT_FRAME_0  , FALSE);
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_PATROL][0][1] = W3DCreateSprite(OFFICER_PATROL_LFOOT_FRAME_45 , FALSE);
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_PATROL][0][2] = W3DCreateSprite(OFFICER_PATROL_LFOOT_FRAME_90 , FALSE);
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_PATROL][0][3] = W3DCreateSprite(OFFICER_PATROL_LFOOT_FRAME_135, FALSE);
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_PATROL][0][4] = W3DCreateSprite(OFFICER_PATROL_LFOOT_FRAME_180, FALSE);
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_PATROL][0][5] = W3DCreateSprite(OFFICER_PATROL_LFOOT_FRAME_225, FALSE);
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_PATROL][0][6] = W3DCreateSprite(OFFICER_PATROL_LFOOT_FRAME_270, FALSE);
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_PATROL][0][7] = W3DCreateSprite(OFFICER_PATROL_LFOOT_FRAME_315, FALSE);

    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_PATROL][1][0] = W3DCreateSprite(OFFICER_PATROL_RFOOT_FRAME_0  , FALSE);
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_PATROL][1][1] = W3DCreateSprite(OFFICER_PATROL_RFOOT_FRAME_45 , FALSE);
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_PATROL][1][2] = W3DCreateSprite(OFFICER_PATROL_RFOOT_FRAME_90 , FALSE);
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_PATROL][1][3] = W3DCreateSprite(OFFICER_PATROL_RFOOT_FRAME_135, FALSE);
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_PATROL][1][4] = W3DCreateSprite(OFFICER_PATROL_RFOOT_FRAME_180, FALSE);
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_PATROL][1][5] = W3DCreateSprite(OFFICER_PATROL_RFOOT_FRAME_225, FALSE);
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_PATROL][1][6] = W3DCreateSprite(OFFICER_PATROL_RFOOT_FRAME_270, FALSE);
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_PATROL][1][7] = W3DCreateSprite(OFFICER_PATROL_RFOOT_FRAME_315, FALSE);

    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_PATROL][2][0] = W3DCreateSprite(OFFICER_RUN_RFOOT_FRAME_0  , FALSE);
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_PATROL][2][1] = W3DCreateSprite(OFFICER_RUN_RFOOT_FRAME_45 , FALSE);
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_PATROL][2][2] = W3DCreateSprite(OFFICER_RUN_RFOOT_FRAME_90 , FALSE);
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_PATROL][2][3] = W3DCreateSprite(OFFICER_RUN_RFOOT_FRAME_135, FALSE);
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_PATROL][2][4] = W3DCreateSprite(OFFICER_RUN_RFOOT_FRAME_180, FALSE);
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_PATROL][2][5] = W3DCreateSprite(OFFICER_RUN_RFOOT_FRAME_225, FALSE);
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_PATROL][2][6] = W3DCreateSprite(OFFICER_RUN_RFOOT_FRAME_270, FALSE);
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_PATROL][2][7] = W3DCreateSprite(OFFICER_RUN_RFOOT_FRAME_315, FALSE);

    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_PATROL][3][0] = W3DCreateSprite(OFFICER_RUN_LFOOT_FRAME_0  , FALSE);
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_PATROL][3][1] = W3DCreateSprite(OFFICER_RUN_LFOOT_FRAME_45 , FALSE);
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_PATROL][3][2] = W3DCreateSprite(OFFICER_RUN_LFOOT_FRAME_90 , FALSE);
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_PATROL][3][3] = W3DCreateSprite(OFFICER_RUN_LFOOT_FRAME_135, FALSE);
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_PATROL][3][4] = W3DCreateSprite(OFFICER_RUN_LFOOT_FRAME_180, FALSE);
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_PATROL][3][5] = W3DCreateSprite(OFFICER_RUN_LFOOT_FRAME_225, FALSE);
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_PATROL][3][6] = W3DCreateSprite(OFFICER_RUN_LFOOT_FRAME_270, FALSE);
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_PATROL][3][7] = W3DCreateSprite(OFFICER_RUN_LFOOT_FRAME_315, FALSE);

    /* OFFICER ATTACK */
    sprite = W3DCreateSprite(OFFICER_ATTACK_FRAME_0, FALSE);
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_ATTACK][0][0] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_ATTACK][0][1] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_ATTACK][0][2] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_ATTACK][0][3] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_ATTACK][0][4] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_ATTACK][0][5] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_ATTACK][0][6] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_ATTACK][0][7] = sprite;

    sprite = W3DCreateSprite(OFFICER_ATTACK_FRAME_1, FALSE);
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_ATTACK][1][0] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_ATTACK][1][1] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_ATTACK][1][2] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_ATTACK][1][3] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_ATTACK][1][4] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_ATTACK][1][5] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_ATTACK][1][6] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_ATTACK][1][7] = sprite;

    sprite = W3DCreateSprite(OFFICER_ATTACK_FRAME_2, FALSE);
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_ATTACK][2][0] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_ATTACK][2][1] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_ATTACK][2][2] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_ATTACK][2][3] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_ATTACK][2][4] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_ATTACK][2][5] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_ATTACK][2][6] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_ATTACK][2][7] = sprite;

    /* OFFICER DAMAGE */
    sprite = W3DCreateSprite(OFFICER_DAMAGE_TAKEN, FALSE);
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_DAMAGE][0][0] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_DAMAGE][0][1] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_DAMAGE][0][2] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_DAMAGE][0][3] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_DAMAGE][0][4] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_DAMAGE][0][5] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_DAMAGE][0][6] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_DAMAGE][0][7] = sprite;

    /* OFFICER DYING */
    sprite = W3DCreateSprite(OFFICER_DEATH_FRAME_0, FALSE);
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_DYING][0][0] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_DYING][0][1] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_DYING][0][2] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_DYING][0][3] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_DYING][0][4] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_DYING][0][5] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_DYING][0][6] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_DYING][0][7] = sprite;

    sprite = W3DCreateSprite(OFFICER_DEATH_FRAME_1, FALSE);
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_DYING][1][0] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_DYING][1][1] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_DYING][1][2] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_DYING][1][3] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_DYING][1][4] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_DYING][1][5] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_DYING][1][6] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_DYING][1][7] = sprite;

    sprite = W3DCreateSprite(OFFICER_DEATH_FRAME_2, FALSE);
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_DYING][2][0] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_DYING][2][1] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_DYING][2][2] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_DYING][2][3] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_DYING][2][4] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_DYING][2][5] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_DYING][2][6] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_DYING][2][7] = sprite;

    sprite = W3DCreateSprite(OFFICER_DEATH_FRAME_3, FALSE);
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_DYING][3][0] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_DYING][3][1] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_DYING][3][2] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_DYING][3][3] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_DYING][3][4] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_DYING][3][5] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_DYING][3][6] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_DYING][3][7] = sprite;

    /* OFFICER DEAD */
    sprite = W3DCreateSprite(OFFICER_DEAD, FALSE);
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_DEAD][0][0] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_DEAD][0][1] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_DEAD][0][2] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_DEAD][0][3] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_DEAD][0][4] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_DEAD][0][5] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_DEAD][0][6] = sprite;
    g_enemySprites[ENEMY_OFFICER][ENEMY_STATE_DEAD][0][7] = sprite;
}