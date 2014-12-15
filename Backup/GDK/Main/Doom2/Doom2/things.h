#ifndef _THINGS_H_
#define _THINGS_H_

#pragma pack(push,1)

//
// THINGS - well, stuff in the level of course!!
//

#define THINGINFO_ARTIFACT      0x0001 // counts towards item score at end of level
#define THINGINFO_PICKUP        0x0002 // player picks it up when they walk over
#define THINGINFO_WEAPON        0x0004 // pick up a weapon! woot!
#define THINGINFO_MONSTER       0x0008 // counts towards kill percentage if you take it out!
#define THINGINFO_OBSTACLE      0x0010 // can't walk through this... 
#define THINGINFO_AIRBORN       0x0020 // if inanimate, hangs from ceiling. If monster, it flys!!

typedef struct
{
    byte_t Radius; // square radius (from center to side), not circle!
    char Sprite[NAME_SIZE];
    char AnimationPattern[NAME_SIZE]; // rotate through this sequence. + means driven by gameplay, - means none
    ushort_t Flags;
} thinginfo_t;

#define THING_TYPE_NONE                     0
#define THING_TYPE_PLAYER1_START            1
#define THING_TYPE_PLAYER2_START            2
#define THING_TYPE_PLAYER3_START            3
#define THING_TYPE_PLAYER4_START            4
#define THING_TYPE_BLUE_KEY                 5
#define THING_TYPE_YELLOW_KEY               6
#define THING_TYPE_SPIDER_MASTERMIND        7
#define THING_TYPE_BACKPACK                 8
#define THING_TYPE_SARGEANT                 9
#define THING_TYPE_BLOODY_MESS              10
#define THING_TYPE_DEATHMATCH_START         11
#define THING_TYPE_BLOODY_MESS2             12
#define THING_TYPE_RED_KEY                  13
#define THING_TYPE_TELEPORT_LANDING         14
#define THING_TYPE_DEAD_PLAYER              15
#define THING_TYPE_CYBERDEMON               16
#define THING_TYPE_CELL_CHARGE_PACK         17
#define THING_TYPE_DEAD_FORMER_HUMAN        18
#define THING_TYPE_DEAD_SEARGANT            19
#define THING_TYPE_DEAD_IMP                 20
#define THING_TYPE_DEAD_DEMON               21
#define THING_TYPE_DEAD_CACODEMON           22
#define THING_TYPE_DEAD_LOST_SOUL           23
#define THING_TYPE_POOL_OF_BLOOD_FLESH      24
#define THING_TYPE_IMPALED_HUMAN            25
#define THING_TYPE_TWITCHING_IMPALED        26
#define THING_TYPE_SKULL_ON_POLE            27
#define THING_TYPE_FIVE_SKULL_SHISHKEBAB    28
#define THING_TYPE_PILE_OF_SKULL_CANDLE     29
#define THING_TYPE_TALL_GREEN_PILLAR        30
#define THING_TYPE_SHORT_GREEN_PILLAR       31
#define THING_TYPE_TALL_RED_PILLAR          32
#define THING_TYPE_SHORT_RED_PILLAR         33
#define THING_TYPE_CANDLE                   34
#define THING_TYPE_CANDELABRA               35
#define THING_TYPE_SHORT_GRN_PILLAR_HEART   36
#define THING_TYPE_SHORT_RED_PILLAR_SKULL   37
#define THING_TYPE_RED_SKULL_KEY            38
#define THING_TYPE_YELLOW_SKULL_KEY         39
#define THING_TYPE_BLUE_SKULL_KEY           40
#define THING_TYPE_EVIL_EYE                 41
#define THING_TYPE_FLOATING_SKULL           42
#define THING_TYPE_BURNT_TREE               43
#define THING_TYPE_TALL_BLUE_FIRESTICK      44
#define THING_TYPE_TALL_GREEN_FIRESTICK     45
#define THING_TYPE_TALL_RED_FIRESTICK       46
#define THING_TYPE_STALAGMITE               47
#define THING_TYPE_TALL_TECHNO_PILLAR       48
#define THING_TYPE_HANGING_VICTIM_TWITCH    49
#define THING_TYPE_HANGING_VICTIM_ARMS_OUT  50
#define THING_TYPE_HANGING_VICTIM_ONE_LEG   51
#define THING_TYPE_HANGING_PAIR_OF_LEGS     52
#define THING_TYPE_HANGING_SINGLE_LEG       53

// TODO: Should we even bother with the rest of these? There ARE A TON, and we need the rest of the metadata as well.
// We might just want to copy the info.c file from the original source and clean it up, it has all hitpoints, attack
// strengths, etc...


#define THING_SKILL_LEVEL_1_AND_2   0x0001
#define THING_SKILL_LEVEL_3         0x0002
#define THING_SKILL_LEVEL_4_AND_5   0x0004
#define THING_IS_DEAF               0x0008
#define THING_MULTIPLAYER_ONLY      0x0010

typedef struct
{
    short xPosition;
    short yPosition;
    short Angle;
    ushort_t Type;
    ushort_t Flags;
} thing_t;

#pragma pack(pop)

#endif // _THINGS_H_