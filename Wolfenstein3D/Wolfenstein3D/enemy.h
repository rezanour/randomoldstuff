#ifndef _ENEMY_H_
#define _ENEMY_H_

/* Types of enemies */
#define ENEMY_TYPE_INVALID      -1
#define ENEMY_TYPE_SOLDIER      0
#define ENEMY_TYPE_DOG          1
#define ENEMY_TYPE_BLUEDUDE     2
#define ENEMY_TYPE_ZOMBIE       3
#define ENEMY_TYPE_OFFICER      4
#define ENEMY_TYPE_BOSS         5
#define MAX_ENEMY_TYPES         6

/* States that enemies can be in */
#define ENEMY_STATE_IDLE        0
#define ENEMY_STATE_PATROL      1
#define ENEMY_STATE_PERSUE      2
#define ENEMY_STATE_ATTACK      3
#define ENEMY_STATE_DAMAGE      4
#define ENEMY_STATE_DYING       5
#define ENEMY_STATE_DEAD        6
#define MAX_ENEMY_STATES        7

/* animations an enemy can be doing */
#define ENEMY_ANIM_IDLE         0
#define ENEMY_ANIM_WALK         1
#define ENEMY_ANIM_ATTACK       2
#define ENEMY_ANIM_DAMAGE       3
#define ENEMY_ANIM_DYING        4
#define ENEMY_ANIM_DEAD         5
#define MAX_ENEMY_ANIMS         6

#define MAX_ENEMIES 500
typedef struct
{
    VEC2 LastPosition;
    VEC2 Position;
    VEC2 Direction;
    VEC2 AttackDirection;
    BYTE PersueMode;
    BYTE Type;
    BYTE Health;
    BYTE CurrentState;
    BYTE PrevState;
    BYTE SaidHelloToPlayer;
    ANIMATION Animation;
} ENEMY;

/* Enemy API */

VOID EnemyInit();
VOID EnemyShutdown();
VOID EnemyClearAll();

USHORT EnemyCreate(VEC2 position, VEC2 direction, BYTE type, BYTE initialState);
VEC2 EnemyGetPosition(USHORT enemyIndex);
BOOL EnemyIsAlive(USHORT enemyIndex);

VOID EnemyTakeDamage(USHORT enemyIndex, USHORT damage);
VOID EnemyAttackPlayer(USHORT enemyIndex);
VOID EnemyYellAtPlayer(USHORT enemyIndex);

VOID EnemyUpdateAll();

/* returns the appropriate sprite for the enemy, factoring in animation and direction */
HSPRITE EnemyGetSprite(USHORT enemyIndex, VEC2 playerPosition, VEC2 playerDirection);

#endif /* _ENEMY_H_ */