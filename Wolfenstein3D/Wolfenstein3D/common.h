#ifndef _COMMON_H_
#define _COMMON_H_

//#define WOLF3D_SHAREWARE_VERSION
#define WOLF3D_FULL_VERSION

#define WIN32_LEAN_AND_MEAN
#define VCEXTRALEAN
#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <math.h>
#include "w3dmath.h"
#include "memmgr.h"
#include "audio.h"
#include "graphics.h"
#include "w3dtypes.h"
#include "w3ddata.h"
#include "animation.h"
#include "player.h"
#include "enemy.h"
#include "level.h"
#include "screens.h"
#include "input.h"
#include "powerup.h"

#define GAME_RANDOM_SEED    1000

#define ENTITY_RADIUS       0.25f

#define CONTENT_ROOT        "..\\..\\Wolf3DContent\\"
#define SPRITES_ROOT        CONTENT_ROOT "sprites\\"
#define UI_ROOT             CONTENT_ROOT "ui\\"
#define WALLS_ROOT          CONTENT_ROOT "walls\\"
#define MAPS_ROOT           CONTENT_ROOT "maps\\"
#define WAV_ROOT            CONTENT_ROOT "wavs\\"
#define MIDI_ROOT           CONTENT_ROOT "midi\\"

#define DIFFICULTY_EASY      0
#define DIFFICULTY_MEDIUM    1
#define DIFFICULTY_HARD      2
#define DIFFICULTY_SUPERHARD 3

extern FLOAT g_totalElapsedTime;
extern FLOAT g_fps;
extern FLOAT g_elapsedTime;
extern BYTE  g_currentScreenId;

#define PLAYER_SPEED        4.0f
#define PLAYER_TURN_SPEED   3.0f

#define PLAYER_START_N      19
#define PLAYER_START_E      20
#define PLAYER_START_S      21
#define PLAYER_START_W      22

#define INVALID_ENTITY      0xffff

#define WEAPON_KNIFE        0
#define WEAPON_PISTOL       1
#define WEAPON_MACHINEGUN   2
#define WEAPON_CHAINGUN     3
#define WEAPON_LAST         4

extern BYTE g_bulletsUsedPerShot[WEAPON_LAST];

#define ENEMY_SOLDIER       0
#define ENEMY_DOG           1
#define ENEMY_BLUEDUDE      2
#define ENEMY_ZOMBIE        3
#define ENEMY_OFFICER       4
#define ENEMY_BOSS1         5
#define ENEMY_MAX           6

#define PATROL_DIR_CHANGE_EAST         90
#define PATROL_DIR_CHANGE_NORTHEAST    91
#define PATROL_DIR_CHANGE_NORTH        92
#define PATROL_DIR_CHANGE_NORTHWEST    93
#define PATROL_DIR_CHANGE_WEST         94
#define PATROL_DIR_CHANGE_SOUTHWEST    95
#define PATROL_DIR_CHANGE_SOUTH        96
#define PATROL_DIR_CHANGE_SOUTHEAST    97

#define PUSHWALL_SECRET                98
#define ENDGAME_TRIGGER                99

#define BOSS_EPISODE_1              214

#define ENEMY_GUARD_DEAD     124

#define EASY_STANDING_GUARD_N       108
#define EASY_STANDING_GUARD_E       109
#define EASY_STANDING_GUARD_S       110
#define EASY_STANDING_GUARD_W       111
#define EASY_PATROL_GUARD_N         112
#define EASY_PATROL_GUARD_E         113
#define EASY_PATROL_GUARD_S         114
#define EASY_PATROL_GUARD_W         115

#define MEDIUM_STANDING_GUARD_N     144
#define MEDIUM_STANDING_GUARD_E     145
#define MEDIUM_STANDING_GUARD_S     146
#define MEDIUM_STANDING_GUARD_W     147
#define MEDIUM_PATROL_GUARD_N       148
#define MEDIUM_PATROL_GUARD_E       149
#define MEDIUM_PATROL_GUARD_S       150
#define MEDIUM_PATROL_GUARD_W       151

#define HARD_STANDING_GUARD_N       180
#define HARD_STANDING_GUARD_E       181
#define HARD_STANDING_GUARD_S       182
#define HARD_STANDING_GUARD_W       183
#define HARD_PATROL_GUARD_N         184
#define HARD_PATROL_GUARD_E         185
#define HARD_PATROL_GUARD_S         186
#define HARD_PATROL_GUARD_W         187

#define EASY_STANDING_DOG_N       206
#define EASY_STANDING_DOG_E       207
#define EASY_STANDING_DOG_S       208
#define EASY_STANDING_DOG_W       209
#define EASY_PATROL_DOG_N         210
#define EASY_PATROL_DOG_E         211
#define EASY_PATROL_DOG_S         212
#define EASY_PATROL_DOG_W         213

#define MEDIUM_STANDING_DOG_N     170
#define MEDIUM_STANDING_DOG_E     171
#define MEDIUM_STANDING_DOG_S     172
#define MEDIUM_STANDING_DOG_W     173
#define MEDIUM_PATROL_DOG_N       174
#define MEDIUM_PATROL_DOG_E       175
#define MEDIUM_PATROL_DOG_S       176
#define MEDIUM_PATROL_DOG_W       177

#define HARD_STANDING_DOG_N       134
#define HARD_STANDING_DOG_E       135
#define HARD_STANDING_DOG_S       136
#define HARD_STANDING_DOG_W       137
#define HARD_PATROL_DOG_N         138
#define HARD_PATROL_DOG_E         139
#define HARD_PATROL_DOG_S         140
#define HARD_PATROL_DOG_W         141

#define EASY_STANDING_SS_N       198
#define EASY_STANDING_SS_E       199
#define EASY_STANDING_SS_S       200
#define EASY_STANDING_SS_W       201
#define EASY_PATROL_SS_N         202
#define EASY_PATROL_SS_E         203
#define EASY_PATROL_SS_S         204
#define EASY_PATROL_SS_W         205

#define MEDIUM_STANDING_SS_N     162
#define MEDIUM_STANDING_SS_E     163
#define MEDIUM_STANDING_SS_S     164
#define MEDIUM_STANDING_SS_W     165
#define MEDIUM_PATROL_SS_N       166
#define MEDIUM_PATROL_SS_E       167
#define MEDIUM_PATROL_SS_S       168
#define MEDIUM_PATROL_SS_W       169

#define HARD_STANDING_SS_N       126
#define HARD_STANDING_SS_E       127
#define HARD_STANDING_SS_S       128
#define HARD_STANDING_SS_W       129
#define HARD_PATROL_SS_N         130
#define HARD_PATROL_SS_E         131
#define HARD_PATROL_SS_S         132
#define HARD_PATROL_SS_W         133

#define EASY_STANDING_ZOMBIE_N       216
#define EASY_STANDING_ZOMBIE_E       217
#define EASY_STANDING_ZOMBIE_S       218
#define EASY_STANDING_ZOMBIE_W       219
#define EASY_PATROL_ZOMBIE_N         220
#define EASY_PATROL_ZOMBIE_E         221
#define EASY_PATROL_ZOMBIE_S         222
#define EASY_PATROL_ZOMBIE_W         223

#define MEDIUM_STANDING_ZOMBIE_N     234
#define MEDIUM_STANDING_ZOMBIE_E     235
#define MEDIUM_STANDING_ZOMBIE_S     236
#define MEDIUM_STANDING_ZOMBIE_W     237
#define MEDIUM_PATROL_ZOMBIE_N       238
#define MEDIUM_PATROL_ZOMBIE_E       239
#define MEDIUM_PATROL_ZOMBIE_S       240
#define MEDIUM_PATROL_ZOMBIE_W       241

#define HARD_STANDING_ZOMBIE_N       252
#define HARD_STANDING_ZOMBIE_E       253
#define HARD_STANDING_ZOMBIE_S       254
#define HARD_STANDING_ZOMBIE_W       255
#define HARD_PATROL_ZOMBIE_N         256
#define HARD_PATROL_ZOMBIE_E         257
#define HARD_PATROL_ZOMBIE_S         258
#define HARD_PATROL_ZOMBIE_W         259

#define EASY_STANDING_OFFICER_N       116
#define EASY_STANDING_OFFICER_E       117
#define EASY_STANDING_OFFICER_S       118
#define EASY_STANDING_OFFICER_W       119
#define EASY_PATROL_OFFICER_N         120
#define EASY_PATROL_OFFICER_E         121
#define EASY_PATROL_OFFICER_S         122
#define EASY_PATROL_OFFICER_W         123

#define MEDIUM_STANDING_OFFICER_N     152
#define MEDIUM_STANDING_OFFICER_E     153
#define MEDIUM_STANDING_OFFICER_S     154
#define MEDIUM_STANDING_OFFICER_W     155
#define MEDIUM_PATROL_OFFICER_N       156
#define MEDIUM_PATROL_OFFICER_E       157
#define MEDIUM_PATROL_OFFICER_S       158
#define MEDIUM_PATROL_OFFICER_W       159

#define HARD_STANDING_OFFICER_N       188
#define HARD_STANDING_OFFICER_E       189
#define HARD_STANDING_OFFICER_S       190
#define HARD_STANDING_OFFICER_W       191
#define HARD_PATROL_OFFICER_N         192
#define HARD_PATROL_OFFICER_E         193
#define HARD_PATROL_OFFICER_S         194
#define HARD_PATROL_OFFICER_W         195

#define WALL_DOOR_NS              90
#define WALL_DOOR_WE              91
#define WALL_DOOR_GOLDKEY_NS      92
#define WALL_DOOR_GOLDKEY_WE      93
#define WALL_DOOR_SILVERKEY_NS    94
#define WALL_DOOR_SILVERKEY_WE    95
#define WALL_DOOR_ELEVATOR_NS    100
#define WALL_DOOR_ELEVATOR_WE    101

#define OBJECT_AI_TURN_E          90
#define OBJECT_AI_TURN_NE         91
#define OBJECT_AI_TURN_NW         92
#define OBJECT_AI_TURN_W          93
#define OBJECT_AI_TURN_SW         94
#define OBJECT_AI_TURN_S          95
#define OBJECT_AI_TURN_SE         96

#define OBJECT_PUSHWALL_MODIFIER  97
#define OBJECT_VICTORY_TRIGGER    98

#ifdef WOLF3D_SHAREWARE_VERSION

#define MAX_WALL_SPRITES        56
#define MAX_LEVELS              10

// sprite identifiers
#define UI_CROUCHINGBJ          0
#define UI_CASTLEPIC            1
#define UI_KEYCONTROLS          2
#define UI_JOYSTICKCONTROLS     3
#define UI_HEALTHEXAMPLES       4
#define UI_GRAYSQUARE           5
#define UI_GUNSEXAMPLES         6
#define UI_GOLDKEY              7
#define UI_BJGATLINGUNS         8
#define UI_HUDWEAPONEXAMPLES    9
#define UI_WOLFENSTEIN3D_TITLE  10
#define UI_VISACARD             11
#define UI_MASTERCARD           12
#define UI_IDLOGO               13
#define UI_TOPBORDER            14
#define UI_RIGHTBORDER          15
#define UI_LEFTBORDER           16
#define UI_BOTTOMBORDER         17
#define UI_GAMEPLAYSHOT         18
#define UI_OPTIONS_TITLE        19
#define UI_CURSORGUN_LIGHT      20
#define UI_CURSORGUN_DARK       21
#define UI_OPTIONTOGGLE_OFF     22
#define UI_OPTIONTOGGLE_ON      23
#define UI_SOUNDEFFECTS_TITLE   24
#define UI_DIGITIZEDSOUND_TITLE 25
#define UI_MUSIC_TITLE          26
#define UI_ESC_BACK_BOTTOM      27
#define UI_CANIPLAYDADDY_FACE   28
#define UI_DONTHURTME_FACE      29
#define UI_BRINGEMON_FACE       30
#define UI_IAMDEATH_FACE        31
#define UI_BLUEFLOPPY           32
#define UI_BUSYDISK1            33
#define UI_BUSYDISK2            34
#define UI_CONTROL_TITLE        35
#define UI_CUSTOMIZE_TITLE      36
#define UI_LOADGAME_TITLE       37
#define UI_SAVEGAME_TITLE       38
#define UI_EPISODE1_PIC         39
#define UI_EPISODE2_PIC         40
#define UI_EPISODE3_PIC         41
#define UI_EPISODE4_PIC         42
#define UI_EPISODE5_PIC         43
#define UI_EPISODE6_PIC         44
#define UI_CODE_OFF             45
#define UI_CODE_ON              46
#define UI_LEVEL_LABEL          47
#define UI_NAME_LABEL           48
#define UI_SCORE_LABEL          49
#define UI_JOYSTICKLEFT         50
#define UI_JOYSTICKRIGHT        51
#define UI_BJCOMPLETETIRED1     52
#define UI_CH_COLON             53
#define UI_CH_0                 54
#define UI_CH_1                 55
#define UI_CH_2                 56
#define UI_CH_3                 57
#define UI_CH_4                 58
#define UI_CH_5                 59
#define UI_CH_6                 60
#define UI_CH_7                 61
#define UI_CH_8                 62
#define UI_CH_9                 63
#define UI_CH_PERCENT           64
#define UI_CH_A                 65
#define UI_CH_B                 66
#define UI_CH_C                 67
#define UI_CH_D                 68
#define UI_CH_E                 69
#define UI_CH_F                 70
#define UI_CH_G                 71
#define UI_CH_H                 72
#define UI_CH_I                 73
#define UI_CH_J                 74
#define UI_CH_K                 75
#define UI_CH_L                 76
#define UI_CH_M                 77
#define UI_CH_N                 78
#define UI_CH_O                 79
#define UI_CH_P                 80
#define UI_CH_Q                 81
#define UI_CH_R                 82
#define UI_CH_S                 83
#define UI_CH_T                 84
#define UI_CH_U                 85
#define UI_CH_V                 86
#define UI_CH_W                 87
#define UI_CH_X                 88
#define UI_CH_Y                 89
#define UI_CH_Z                 90
#define UI_CH_EXCLAMATION       91
#define UI_CH_APOSTR            92
#define UI_BJCOMPLETETIRED2     93
#define UI_BJCOMPLETEGUN        94
#define UI_HUD                  95
#define UI_TITLE                96
#define UI_PC13                 97
#define UI_CREDITS              98
#define UI_HIGHSCORES_TITLE     99
#define UI_HUD_KNIFE            100
#define UI_HUD_PISTOL           101
#define UI_HUD_MACHINEGUN       102
#define UI_HUD_CHAINGUN         103
#define UI_HUD_NOKEY            104
#define UI_HUD_GOLDKEY          105
#define UI_HUD_SILVERKEY        106
#define UI_HUD_CH_SPACE         107
#define UI_HUD_CH_0             108
#define UI_HUD_CH_1             109
#define UI_HUD_CH_2             110
#define UI_HUD_CH_3             111
#define UI_HUD_CH_4             112
#define UI_HUD_CH_5             113
#define UI_HUD_CH_6             114
#define UI_HUD_CH_7             115
#define UI_HUD_CH_8             116
#define UI_HUD_CH_9             117
#define UI_HUD_FACE_FRONT       118
#define UI_HUD_FACE_LEFT        119
#define UI_HUD_FACE_RIGHT       120
#define UI_HUD_FACE_FRONT1      121
#define UI_HUD_FACE_LEFT1       122
#define UI_HUD_FACE_RIGHT1      123
#define UI_HUD_FACE_FRONT2      124
#define UI_HUD_FACE_LEFT2       125
#define UI_HUD_FACE_RIGHT2      126
#define UI_HUD_FACE_FRONT3      127
#define UI_HUD_FACE_LEFT3       128
#define UI_HUD_FACE_RIGHT3      129
#define UI_HUD_FACE_FRONT4      130
#define UI_HUD_FACE_LEFT4       131
#define UI_HUD_FACE_RIGHT4      132
#define UI_HUD_FACE_FRONT5      133
#define UI_HUD_FACE_LEFT5       134
#define UI_HUD_FACE_RIGHT5      135
#define UI_HUD_FACE_FRONT6      136
#define UI_HUD_FACE_LEFT6       137
#define UI_HUD_FACE_RIGHT6      138
#define UI_HUD_FACE_DEAD        139
#define UI_HUD_FACE_HAPPY       140
#define UI_HUD_FACE_INFECTED    141
#define UI_PAUSED_TITLE         142
#define UI_GETPSYCHED_TITLE     143

#endif // WOLF3D_SHAREWARE_VERSION

#ifdef WOLF3D_FULL_VERSION

#define MAX_WALL_SPRITES        98
#define MAX_LEVELS              60

// sprite identifiers
#define UI_CROUCHINGBJ          0
#define UI_CASTLEPIC            1
#define UI_KEYCONTROLS          2
#define UI_JOYSTICKCONTROLS     3
#define UI_HEALTHEXAMPLES       4
#define UI_GRAYSQUARE           5
#define UI_GUNSEXAMPLES         6
#define UI_GOLDKEY              7
#define UI_BJGATLINGUNS         2
#define UI_HUDWEAPONEXAMPLES    9
#define UI_WOLFENSTEIN3D_TITLE  10
#define UI_VISACARD             11
#define UI_MASTERCARD           12
#define UI_IDLOGO               13
#define UI_TOPBORDER            3
#define UI_RIGHTBORDER          4
#define UI_LEFTBORDER           5
#define UI_BOTTOMBORDER         6
#define UI_GAMEPLAYSHOT         18
#define UI_OPTIONS_TITLE        7
#define UI_CURSORGUN_LIGHT      8
#define UI_CURSORGUN_DARK       9
#define UI_OPTIONTOGGLE_OFF     10
#define UI_OPTIONTOGGLE_ON      11
#define UI_SOUNDEFFECTS_TITLE   12
#define UI_DIGITIZEDSOUND_TITLE 13
#define UI_MUSIC_TITLE          14
#define UI_ESC_BACK_BOTTOM      15
#define UI_CANIPLAYDADDY_FACE   16
#define UI_DONTHURTME_FACE      17
#define UI_BRINGEMON_FACE       18
#define UI_IAMDEATH_FACE        19
#define UI_BLUEFLOPPY           20
#define UI_BUSYDISK1            21
#define UI_BUSYDISK2            22
#define UI_CONTROL_TITLE        23
#define UI_CUSTOMIZE_TITLE      24
#define UI_LOADGAME_TITLE       25
#define UI_SAVEGAME_TITLE       26
#define UI_EPISODE1_PIC         27
#define UI_EPISODE2_PIC         28
#define UI_EPISODE3_PIC         29
#define UI_EPISODE4_PIC         30
#define UI_EPISODE5_PIC         31
#define UI_EPISODE6_PIC         32
#define UI_CODE_OFF             33
#define UI_CODE_ON              34
#define UI_LEVEL_LABEL          35
#define UI_NAME_LABEL           36
#define UI_SCORE_LABEL          37
#define UI_JOYSTICKLEFT         38
#define UI_JOYSTICKRIGHT        39
#define UI_BJCOMPLETETIRED1     40
#define UI_CH_COLON             41
#define UI_CH_0                 42
#define UI_CH_1                 43
#define UI_CH_2                 44
#define UI_CH_3                 45
#define UI_CH_4                 46
#define UI_CH_5                 47
#define UI_CH_6                 48
#define UI_CH_7                 49
#define UI_CH_8                 50
#define UI_CH_9                 51
#define UI_CH_PERCENT           52
#define UI_CH_A                 53
#define UI_CH_B                 54
#define UI_CH_C                 55
#define UI_CH_D                 56
#define UI_CH_E                 57
#define UI_CH_F                 58
#define UI_CH_G                 59
#define UI_CH_H                 60
#define UI_CH_I                 61
#define UI_CH_J                 62
#define UI_CH_K                 63
#define UI_CH_L                 64
#define UI_CH_M                 65
#define UI_CH_N                 66
#define UI_CH_O                 67
#define UI_CH_P                 68
#define UI_CH_Q                 69
#define UI_CH_R                 70
#define UI_CH_S                 71
#define UI_CH_T                 72
#define UI_CH_U                 73
#define UI_CH_V                 74
#define UI_CH_W                 75
#define UI_CH_X                 76
#define UI_CH_Y                 77
#define UI_CH_Z                 78
#define UI_CH_EXCLAMATION       79
#define UI_CH_APOSTR            80
#define UI_BJCOMPLETETIRED2     81
#define UI_BJCOMPLETEGUN        82
#define UI_HUD                  83
#define UI_TITLE                84
#define UI_PC13                 85
#define UI_CREDITS              86
#define UI_HIGHSCORES_TITLE     87
#define UI_HUD_KNIFE            88
#define UI_HUD_PISTOL           89
#define UI_HUD_MACHINEGUN       90
#define UI_HUD_CHAINGUN         91
#define UI_HUD_NOKEY            92
#define UI_HUD_GOLDKEY          93
#define UI_HUD_SILVERKEY        94
#define UI_HUD_CH_SPACE         95
#define UI_HUD_CH_0             96
#define UI_HUD_CH_1             97
#define UI_HUD_CH_2             98
#define UI_HUD_CH_3             99
#define UI_HUD_CH_4             100
#define UI_HUD_CH_5             101
#define UI_HUD_CH_6             102
#define UI_HUD_CH_7             103
#define UI_HUD_CH_8             104
#define UI_HUD_CH_9             105
#define UI_HUD_FACE_FRONT       106
#define UI_HUD_FACE_LEFT        107
#define UI_HUD_FACE_RIGHT       108
#define UI_HUD_FACE_FRONT1      109
#define UI_HUD_FACE_LEFT1       110
#define UI_HUD_FACE_RIGHT1      111
#define UI_HUD_FACE_FRONT2      112
#define UI_HUD_FACE_LEFT2       113
#define UI_HUD_FACE_RIGHT2      114
#define UI_HUD_FACE_FRONT3      115
#define UI_HUD_FACE_LEFT3       116
#define UI_HUD_FACE_RIGHT3      117
#define UI_HUD_FACE_FRONT4      118
#define UI_HUD_FACE_LEFT4       119
#define UI_HUD_FACE_RIGHT4      120
#define UI_HUD_FACE_FRONT5      121
#define UI_HUD_FACE_LEFT5       122
#define UI_HUD_FACE_RIGHT5      123
#define UI_HUD_FACE_FRONT6      124
#define UI_HUD_FACE_LEFT6       125
#define UI_HUD_FACE_RIGHT6      126
#define UI_HUD_FACE_DEAD        127
#define UI_HUD_FACE_HAPPY       128
#define UI_HUD_FACE_INFECTED    129
#define UI_PAUSED_TITLE         130
#define UI_GETPSYCHED_TITLE     131

#endif // WOLF3D_FULL_VERSION

#define OBJECT_DEMO             106
#define OBJECT_DEATHCAM         107
#define OBJECT_WATER            108
#define OBJECT_GREENBARREL      109
#define OBJECT_TABLE_AND_CHAIRS 110
#define OBJECT_GREENLAMP        111
#define OBJECT_CHANDALIER       112
#define OBJECT_HANGING_SKELETON 113
#define OBJECT_DOGFOOD          114
#define OBJECT_PILLER           115
#define OBJECT_TREEPLANT        116
#define OBJECT_FLOORSKELETON    117
#define OBJECT_SINK             118
#define OBJECT_POTTEDPLANT      119
#define OBJECT_BLUEVACE         120
#define OBJECT_TABLE            121
#define OBJECT_GREENCEILINGLIGHT    122
#define OBJECT_POTSANDPANS      123
#define OBJECT_KNIGHT           124
#define OBJECT_CAGEEMPTY        125
#define OBJECT_CAGESKELETON     126
#define OBJECT_BONEPILES        127
#define OBJECT_GOLDKEY          128
#define OBJECT_SILVERKEY        129
#define OBJECT_BED              130
#define OBJECT_DOGFOOD2         131
#define OBJECT_TVDINNER         132
#define OBJECT_HEALTHKIT        133
#define OBJECT_AMMO             134
#define OBJECT_MACHINEGUN       135
#define OBJECT_CHAINGUN         136
#define OBJECT_CROSS            137
#define OBJECT_CUP              138
#define OBJECT_CHEST            139
#define OBJECT_CROWN            140
#define OBJECT_LIFEPOWERUP      141
#define OBJECT_BLOODYBONES      142
#define OBJECT_BROWNBARREL      143
#define OBJECT_WELLFULL         144
#define OBJECT_WELLEMPTY        145
#define OBJECT_BLOOD            146
#define OBJECT_FLAG             147
#define OBJECT_CALLARDWARF      148
#define OBJECT_MOREBONES        149
#define OBJECT_EVENMOREBONES    150
#define OBJECT_SOMEBONES        151
#define OBJECT_MOREPOTSANDPANS  152
#define OBJECT_WOODSTOVE        153
#define OBJECT_SPEARS           154
#define OBJECT_VINES            155
#define MAX_OBJECT_ID           156

#define SOLDIER_STAND_FRAME_0           156
#define SOLDIER_STAND_FRAME_45          157
#define SOLDIER_STAND_FRAME_90          158
#define SOLDIER_STAND_FRAME_135         159
#define SOLDIER_STAND_FRAME_180         160
#define SOLDIER_STAND_FRAME_225         161
#define SOLDIER_STAND_FRAME_270         162
#define SOLDIER_STAND_FRAME_315         163
#define SOLDIER_PATROL_LFOOT_FRAME_0    164
#define SOLDIER_PATROL_LFOOT_FRAME_45   165
#define SOLDIER_PATROL_LFOOT_FRAME_90   166
#define SOLDIER_PATROL_LFOOT_FRAME_135  167
#define SOLDIER_PATROL_LFOOT_FRAME_180  168
#define SOLDIER_PATROL_LFOOT_FRAME_225  169
#define SOLDIER_PATROL_LFOOT_FRAME_270  170
#define SOLDIER_PATROL_LFOOT_FRAME_315  171
#define SOLDIER_PATROL_RFOOT_FRAME_0    172
#define SOLDIER_PATROL_RFOOT_FRAME_45   173
#define SOLDIER_PATROL_RFOOT_FRAME_90   174
#define SOLDIER_PATROL_RFOOT_FRAME_135  175
#define SOLDIER_PATROL_RFOOT_FRAME_180  176
#define SOLDIER_PATROL_RFOOT_FRAME_225  177
#define SOLDIER_PATROL_RFOOT_FRAME_270  178
#define SOLDIER_PATROL_RFOOT_FRAME_315  179
#define SOLDIER_RUN_RFOOT_FRAME_0       180
#define SOLDIER_RUN_RFOOT_FRAME_45      181
#define SOLDIER_RUN_RFOOT_FRAME_90      182
#define SOLDIER_RUN_RFOOT_FRAME_135     183
#define SOLDIER_RUN_RFOOT_FRAME_180     184
#define SOLDIER_RUN_RFOOT_FRAME_225     185
#define SOLDIER_RUN_RFOOT_FRAME_270     186
#define SOLDIER_RUN_RFOOT_FRAME_315     187
#define SOLDIER_RUN_LFOOT_FRAME_0       188
#define SOLDIER_RUN_LFOOT_FRAME_45      189
#define SOLDIER_RUN_LFOOT_FRAME_90      190
#define SOLDIER_RUN_LFOOT_FRAME_135     191
#define SOLDIER_RUN_LFOOT_FRAME_180     192
#define SOLDIER_RUN_LFOOT_FRAME_225     193
#define SOLDIER_RUN_LFOOT_FRAME_270     194
#define SOLDIER_RUN_LFOOT_FRAME_315     195
#define SOLDIER_DEATH_FRAME_0           196
#define SOLDIER_DEATH_FRAME_1           197
#define SOLDIER_DEATH_FRAME_2           198
#define SOLDIER_DEATH_FRAME_3           199
#define SOLDIER_DAMAGE_TAKEN            200
#define SOLDIER_DEAD                    201
#define SOLDIER_ATTACK_FRAME_0          202
#define SOLDIER_ATTACK_FRAME_1          203
#define SOLDIER_ATTACK_FRAME_2          204

#define DOG_STAND_FRAME_0           205
#define DOG_STAND_FRAME_45          206
#define DOG_STAND_FRAME_90          207
#define DOG_STAND_FRAME_135         208
#define DOG_STAND_FRAME_180         209
#define DOG_STAND_FRAME_225         210
#define DOG_STAND_FRAME_270         211
#define DOG_STAND_FRAME_315         212
#define DOG_PATROL_LFOOT_FRAME_0    213
#define DOG_PATROL_LFOOT_FRAME_45   214
#define DOG_PATROL_LFOOT_FRAME_90   215
#define DOG_PATROL_LFOOT_FRAME_135  216
#define DOG_PATROL_LFOOT_FRAME_180  217
#define DOG_PATROL_LFOOT_FRAME_225  218
#define DOG_PATROL_LFOOT_FRAME_270  219
#define DOG_PATROL_LFOOT_FRAME_315  220
#define DOG_PATROL_RFOOT_FRAME_0    221
#define DOG_PATROL_RFOOT_FRAME_45   222
#define DOG_PATROL_RFOOT_FRAME_90   223
#define DOG_PATROL_RFOOT_FRAME_135  224
#define DOG_PATROL_RFOOT_FRAME_180  225
#define DOG_PATROL_RFOOT_FRAME_225  226
#define DOG_PATROL_RFOOT_FRAME_270  227
#define DOG_PATROL_RFOOT_FRAME_315  228
#define DOG_RUN_RFOOT_FRAME_0       229
#define DOG_RUN_RFOOT_FRAME_45      230
#define DOG_RUN_RFOOT_FRAME_90      231
#define DOG_RUN_RFOOT_FRAME_135     232
#define DOG_RUN_RFOOT_FRAME_180     233
#define DOG_RUN_RFOOT_FRAME_225     234
#define DOG_RUN_RFOOT_FRAME_270     235
#define DOG_RUN_RFOOT_FRAME_315     236
#define DOG_DEATH_FRAME_0           237
#define DOG_DEATH_FRAME_1           238
#define DOG_DEATH_FRAME_2           239
#define DOG_DEAD                    240
#define DOG_ATTACK_FRAME_0          241
#define DOG_ATTACK_FRAME_1          242
#define DOG_ATTACK_FRAME_2          243

#define SS_STAND_FRAME_0           244
#define SS_STAND_FRAME_45          245
#define SS_STAND_FRAME_90          246
#define SS_STAND_FRAME_135         247
#define SS_STAND_FRAME_180         248
#define SS_STAND_FRAME_225         249
#define SS_STAND_FRAME_270         250
#define SS_STAND_FRAME_315         251
#define SS_PATROL_LFOOT_FRAME_0    252
#define SS_PATROL_LFOOT_FRAME_45   253
#define SS_PATROL_LFOOT_FRAME_90   254
#define SS_PATROL_LFOOT_FRAME_135  255
#define SS_PATROL_LFOOT_FRAME_180  256
#define SS_PATROL_LFOOT_FRAME_225  257
#define SS_PATROL_LFOOT_FRAME_270  258
#define SS_PATROL_LFOOT_FRAME_315  259
#define SS_PATROL_RFOOT_FRAME_0    260
#define SS_PATROL_RFOOT_FRAME_45   261
#define SS_PATROL_RFOOT_FRAME_90   262
#define SS_PATROL_RFOOT_FRAME_135  263
#define SS_PATROL_RFOOT_FRAME_180  264
#define SS_PATROL_RFOOT_FRAME_225  265
#define SS_PATROL_RFOOT_FRAME_270  266
#define SS_PATROL_RFOOT_FRAME_315  267
#define SS_RUN_RFOOT_FRAME_0       268
#define SS_RUN_RFOOT_FRAME_45      269
#define SS_RUN_RFOOT_FRAME_90      270
#define SS_RUN_RFOOT_FRAME_135     271
#define SS_RUN_RFOOT_FRAME_180     272
#define SS_RUN_RFOOT_FRAME_225     273
#define SS_RUN_RFOOT_FRAME_270     274
#define SS_RUN_RFOOT_FRAME_315     275
#define SS_RUN_LFOOT_FRAME_0       276
#define SS_RUN_LFOOT_FRAME_45      277
#define SS_RUN_LFOOT_FRAME_90      278
#define SS_RUN_LFOOT_FRAME_135     279
#define SS_RUN_LFOOT_FRAME_180     280
#define SS_RUN_LFOOT_FRAME_225     281
#define SS_RUN_LFOOT_FRAME_270     282
#define SS_RUN_LFOOT_FRAME_315     283
#define SS_DEATH_FRAME_0           284
#define SS_DEATH_FRAME_1           285
#define SS_DEATH_FRAME_2           286
#define SS_DEATH_FRAME_3           287
#define SS_DAMAGE_TAKEN            288
#define SS_DEAD                    289
#define SS_ATTACK_FRAME_0          290
#define SS_ATTACK_FRAME_1          291
#define SS_ATTACK_FRAME_2          292

#define ZOMBIE_STAND_FRAME_0           293
#define ZOMBIE_STAND_FRAME_45          294
#define ZOMBIE_STAND_FRAME_90          295
#define ZOMBIE_STAND_FRAME_135         296
#define ZOMBIE_STAND_FRAME_180         297
#define ZOMBIE_STAND_FRAME_225         298
#define ZOMBIE_STAND_FRAME_270         299
#define ZOMBIE_STAND_FRAME_315         300
#define ZOMBIE_PATROL_LFOOT_FRAME_0    301
#define ZOMBIE_PATROL_LFOOT_FRAME_45   302
#define ZOMBIE_PATROL_LFOOT_FRAME_90   303
#define ZOMBIE_PATROL_LFOOT_FRAME_135  304
#define ZOMBIE_PATROL_LFOOT_FRAME_180  305
#define ZOMBIE_PATROL_LFOOT_FRAME_225  306
#define ZOMBIE_PATROL_LFOOT_FRAME_270  307
#define ZOMBIE_PATROL_LFOOT_FRAME_315  308
#define ZOMBIE_PATROL_RFOOT_FRAME_0    309
#define ZOMBIE_PATROL_RFOOT_FRAME_45   310
#define ZOMBIE_PATROL_RFOOT_FRAME_90   311
#define ZOMBIE_PATROL_RFOOT_FRAME_135  312
#define ZOMBIE_PATROL_RFOOT_FRAME_180  313
#define ZOMBIE_PATROL_RFOOT_FRAME_225  314
#define ZOMBIE_PATROL_RFOOT_FRAME_270  315
#define ZOMBIE_PATROL_RFOOT_FRAME_315  316
#define ZOMBIE_RUN_RFOOT_FRAME_0       317
#define ZOMBIE_RUN_RFOOT_FRAME_45      318
#define ZOMBIE_RUN_RFOOT_FRAME_90      319
#define ZOMBIE_RUN_RFOOT_FRAME_135     320
#define ZOMBIE_RUN_RFOOT_FRAME_180     321
#define ZOMBIE_RUN_RFOOT_FRAME_225     322
#define ZOMBIE_RUN_RFOOT_FRAME_270     323
#define ZOMBIE_RUN_RFOOT_FRAME_315     324
#define ZOMBIE_RUN_LFOOT_FRAME_0       325
#define ZOMBIE_RUN_LFOOT_FRAME_45      326
#define ZOMBIE_RUN_LFOOT_FRAME_90      327
#define ZOMBIE_RUN_LFOOT_FRAME_135     328
#define ZOMBIE_RUN_LFOOT_FRAME_180     329
#define ZOMBIE_RUN_LFOOT_FRAME_225     330
#define ZOMBIE_RUN_LFOOT_FRAME_270     331
#define ZOMBIE_RUN_LFOOT_FRAME_315     332
#define ZOMBIE_DEATH_FRAME_0           334
#define ZOMBIE_DEATH_FRAME_1           335
#define ZOMBIE_DEATH_FRAME_2           336
#define ZOMBIE_DEATH_FRAME_3           337
#define ZOMBIE_DEATH_FRAME_4           338
#define ZOMBIE_DAMAGE_TAKEN            333
#define ZOMBIE_DEAD                    339
#define ZOMBIE_ATTACK_FRAME_0          340
#define ZOMBIE_ATTACK_FRAME_1          341
#define ZOMBIE_ATTACK_FRAME_2          342
#define ZOMBIE_ATTACK_FRAME_2          343

#define OFFICER_STAND_FRAME_0           344
#define OFFICER_STAND_FRAME_45          345
#define OFFICER_STAND_FRAME_90          346
#define OFFICER_STAND_FRAME_135         347
#define OFFICER_STAND_FRAME_180         348
#define OFFICER_STAND_FRAME_225         349
#define OFFICER_STAND_FRAME_270         350
#define OFFICER_STAND_FRAME_315         351
#define OFFICER_PATROL_LFOOT_FRAME_0    352
#define OFFICER_PATROL_LFOOT_FRAME_45   353
#define OFFICER_PATROL_LFOOT_FRAME_90   354
#define OFFICER_PATROL_LFOOT_FRAME_135  355
#define OFFICER_PATROL_LFOOT_FRAME_180  356
#define OFFICER_PATROL_LFOOT_FRAME_225  357
#define OFFICER_PATROL_LFOOT_FRAME_270  358
#define OFFICER_PATROL_LFOOT_FRAME_315  359
#define OFFICER_PATROL_RFOOT_FRAME_0    360
#define OFFICER_PATROL_RFOOT_FRAME_45   361
#define OFFICER_PATROL_RFOOT_FRAME_90   362
#define OFFICER_PATROL_RFOOT_FRAME_135  363
#define OFFICER_PATROL_RFOOT_FRAME_180  364
#define OFFICER_PATROL_RFOOT_FRAME_225  365
#define OFFICER_PATROL_RFOOT_FRAME_270  366
#define OFFICER_PATROL_RFOOT_FRAME_315  367
#define OFFICER_RUN_RFOOT_FRAME_0       368
#define OFFICER_RUN_RFOOT_FRAME_45      369
#define OFFICER_RUN_RFOOT_FRAME_90      370
#define OFFICER_RUN_RFOOT_FRAME_135     371
#define OFFICER_RUN_RFOOT_FRAME_180     372
#define OFFICER_RUN_RFOOT_FRAME_225     373
#define OFFICER_RUN_RFOOT_FRAME_270     374
#define OFFICER_RUN_RFOOT_FRAME_315     375
#define OFFICER_RUN_LFOOT_FRAME_0       376
#define OFFICER_RUN_LFOOT_FRAME_45      377
#define OFFICER_RUN_LFOOT_FRAME_90      378
#define OFFICER_RUN_LFOOT_FRAME_135     379
#define OFFICER_RUN_LFOOT_FRAME_180     380
#define OFFICER_RUN_LFOOT_FRAME_225     381
#define OFFICER_RUN_LFOOT_FRAME_270     382
#define OFFICER_RUN_LFOOT_FRAME_315     383
#define OFFICER_DEATH_FRAME_0           385
#define OFFICER_DEATH_FRAME_1           386
#define OFFICER_DEATH_FRAME_2           387
#define OFFICER_DEATH_FRAME_3           389
#define OFFICER_DAMAGE_TAKEN            384
#define OFFICER_DEAD                    390
#define OFFICER_ATTACK_FRAME_0          391
#define OFFICER_ATTACK_FRAME_1          392
#define OFFICER_ATTACK_FRAME_2          393

#define RED_GHOST_FRAME_0          394
#define RED_GHOST_FRAME_1          395
#define PINK_GHOST_FRAME_0         396
#define PINK_GHOST_FRAME_1         397
#define YELLOW_GHOST_FRAME_0       398
#define YELLOW_GHOST_FRAME_1       399
#define BLUE_GHOST_FRAME_0         400
#define BLUE_GHOST_FRAME_1         401

#define BOSS1_WALK_FRAME_0         402
#define BOSS1_WALK_FRAME_1         403
#define BOSS1_WALK_FRAME_2         404
#define BOSS1_WALK_FRAME_3         405
#define BOSS1_ATTACK_FRAME_0       406
#define BOSS1_ATTACK_FRAME_1       407
#define BOSS1_ATTACK_FRAME_2       408
#define BOSS1_DEAD                 409
#define BOSS1_DEATH_FRAME_0        410
#define BOSS1_DEATH_FRAME_1        411
#define BOSS1_DEATH_FRAME_2        412

#define BOSS2_WALK_FRAME_0         413
#define BOSS2_WALK_FRAME_1         414
#define BOSS2_WALK_FRAME_2         415
#define BOSS2_WALK_FRAME_3         416
#define BOSS2_ATTACK_FRAME_0       417
#define BOSS2_ATTACK_FRAME_1       418
#define BOSS2_DEAD                 422
#define BOSS2_DEATH_FRAME_0        419
#define BOSS2_DEATH_FRAME_1        420
#define BOSS2_DEATH_FRAME_2        421
#define BOSS2_PROJECTILE_FRAME_0   423
#define BOSS2_PROJECTILE_FRAME_1   424
#define BOSS2_PROJECTILE_FRAME_2   425
#define BOSS2_PROJECTILE_FRAME_3   426

// SPECTORs appear in BOSS3 level
#define HITLER_SPECTOR_WALK_FRAME_0         427
#define HITLER_SPECTOR_WALK_FRAME_1         428
#define HITLER_SPECTOR_WALK_FRAME_2         429
#define HITLER_SPECTOR_WALK_FRAME_3         430
#define HITLER_SPECTOR_ATTACK_FRAME_0       431
#define HITLER_SPECTOR_DEAD                 439
#define HITLER_SPECTOR_DEATH_FRAME_0        434
#define HITLER_SPECTOR_DEATH_FRAME_1        435
#define HITLER_SPECTOR_DEATH_FRAME_2        436
#define HITLER_SPECTOR_DEATH_FRAME_3        437
#define HITLER_SPECTOR_DEATH_FRAME_4        438
#define HITLER_SPECTOR_PROJECTILE_FRAME_0   432
#define HITLER_SPECTOR_PROJECTILE_FRAME_1   433

// HITLER is BOSS3 (with and without armor suit)
#define HITLER_ARMORSUIT_WALK_FRAME_0         440
#define HITLER_ARMORSUIT_WALK_FRAME_1         441
#define HITLER_ARMORSUIT_WALK_FRAME_2         442
#define HITLER_ARMORSUIT_WALK_FRAME_3         443
#define HITLER_ARMORSUIT_ATTACK_FRAME_0       444
#define HITLER_ARMORSUIT_ATTACK_FRAME_1       445
#define HITLER_ARMORSUIT_ATTACK_FRAME_2       446
#define HITLER_ARMORSUIT_DEAD                 447
#define HITLER_ARMORSUIT_DEATH_FRAME_0        448
#define HITLER_ARMORSUIT_DEATH_FRAME_1        449
#define HITLER_ARMORSUIT_DEATH_FRAME_2        450

#define HITLER_WALK_FRAME_0         451
#define HITLER_WALK_FRAME_1         452
#define HITLER_WALK_FRAME_2         453
#define HITLER_WALK_FRAME_3         454
#define HITLER_ATTACK_FRAME_0       455
#define HITLER_ATTACK_FRAME_1       456
#define HITLER_ATTACK_FRAME_2       457
#define HITLER_DEAD                 448
#define HITLER_DEATH_FRAME_0        459
#define HITLER_DEATH_FRAME_1        460
#define HITLER_DEATH_FRAME_2        461
#define HITLER_DEATH_FRAME_3        462
#define HITLER_DEATH_FRAME_4        463
#define HITLER_DEATH_FRAME_5        464
#define HITLER_DEATH_FRAME_6        465

#define BOSS4_WALK_FRAME_0         466
#define BOSS4_WALK_FRAME_1         467
#define BOSS4_WALK_FRAME_2         468
#define BOSS4_WALK_FRAME_3         469
#define BOSS4_ATTACK_FRAME_0       470
#define BOSS4_ATTACK_FRAME_1       471
#define BOSS4_DEAD                 475
#define BOSS4_DEATH_FRAME_0        472
#define BOSS4_DEATH_FRAME_1        473
#define BOSS4_DEATH_FRAME_2        474
#define BOSS4_PROJECTILE_FRAME_0   476
#define BOSS4_PROJECTILE_FRAME_1   477
#define BOSS4_PROJECTILE_FRAME_2   478
#define BOSS4_PROJECTILE_FRAME_3   479
#define BOSS4_PROJECTILE_FRAME_4   480
#define BOSS4_PROJECTILE_FRAME_5   481
#define BOSS4_PROJECTILE_FRAME_6   482
#define BOSS4_PROJECTILE_FRAME_7   483
#define BOSS4_PROJECTILE_FRAME_8   484
#define BOSS4_PROJECTILE_FRAME_9   485
#define BOSS4_PROJECTILE_FRAME_10  486
#define BOSS4_PROJECTILE_FRAME_11  487
#define BOSS4_PROJECTILE_FRAME_12  488
#define BOSS4_PROJECTILE_FRAME_13  489
#define BOSS4_PROJECTILE_FRAME_14  490

#define BOSS5_WALK_FRAME_0         491
#define BOSS5_WALK_FRAME_1         492
#define BOSS5_WALK_FRAME_2         493
#define BOSS5_WALK_FRAME_3         494
#define BOSS5_ATTACK_FRAME_0       495
#define BOSS5_ATTACK_FRAME_1       496
#define BOSS5_ATTACK_FRAME_2       497
#define BOSS5_DEAD                 498
#define BOSS5_DEATH_FRAME_0        499
#define BOSS5_DEATH_FRAME_1        500
#define BOSS5_DEATH_FRAME_2        501

#define BOSS6_WALK_FRAME_0         502
#define BOSS6_WALK_FRAME_1         503
#define BOSS6_WALK_FRAME_2         504
#define BOSS6_WALK_FRAME_3         505
#define BOSS6_ATTACK_FRAME_0       506
#define BOSS6_ATTACK_FRAME_1       507
#define BOSS6_ATTACK_FRAME_2       508
#define BOSS6_DEAD                 513
#define BOSS6_DEATH_FRAME_0        510
#define BOSS6_DEATH_FRAME_1        511
#define BOSS6_DEATH_FRAME_2        512

#define BJ_RUN_FRAME_0             514
#define BJ_RUN_FRAME_1             515
#define BJ_RUN_FRAME_2             516
#define BJ_RUN_FRAME_3             517
#define BJ_JUMP_FRAME_0            518
#define BJ_JUMP_FRAME_1            519
#define BJ_JUMP_FRAME_2            520
#define BJ_JUMP_FRAME_3            521
#define KNIFE_IDLE_FRAME           522
#define KNIFE_ATTACK_FRAME_0       523
#define KNIFE_ATTACK_FRAME_1       524
#define KNIFE_ATTACK_FRAME_2       525
#define KNIFE_ATTACK_FRAME_3       526
#define PISTOL_IDLE_FRAME          527
#define PISTOL_ATTACK_FRAME_0      528
#define PISTOL_ATTACK_FRAME_1      529
#define PISTOL_ATTACK_FRAME_2      530
#define PISTOL_ATTACK_FRAME_3      531
#define MACHINEGUN_IDLE_FRAME      532
#define MACHINEGUN_ATTACK_FRAME_0  533
#define MACHINEGUN_ATTACK_FRAME_1  534
#define MACHINEGUN_ATTACK_FRAME_2  535
#define MACHINEGUN_ATTACK_FRAME_3  536
#define CHAINGUN_IDLE_FRAME        537
#define CHAINGUN_ATTACK_FRAME_0    538
#define CHAINGUN_ATTACK_FRAME_1    539
#define CHAINGUN_ATTACK_FRAME_2    540
#define CHAINGUN_ATTACK_FRAME_3    541

#define WALL_STONE_GREY_DARK                1
#define WALL_STONE_GREY2_LIGHT              2
#define WALL_STONE_GREY2_DARK               3
#define WALL_STONE_GREY_FLAG_LIGHT          4
#define WALL_STONE_GREY_FLAG_DARK           5
#define WALL_STONE_GREY_HITLER_LIGHT        6
#define WALL_STONE_GREY_HITLER_DARK         7
#define WALL_STONE_BLUE_CAGE_EMPTY_LIGHT    8
#define WALL_STONE_BLUE_CAGE_EMPTY_DARK     9
#define WALL_STONE_GREY_EAGLE_LIGHT         10
#define WALL_STONE_GREY_EAGLE_DARK          11
#define WALL_STONE_BLUE_CAGE_SKELE_LIGHT    12
#define WALL_STONE_BLUE_CAGE_SKELE_DARK     13
#define WALL_STONE_BLUE_DARK                14
#define WALL_STONE_BLUE_LIGHT               15
#define WALL_STONE_BLUE2_DARK               16
#define WALL_STONE_BLUE2_LIGHT              17
#define WALL_WOOD_EAGLE_LIGHT               18
#define WALL_WOOD_EAGLE_DARK                19
#define WALL_WOOD_HITLER_LIGHT              20
#define WALL_WOOD_HITLER_DARK               21
#define WALL_WOOD_LIGHT                     22
#define WALL_WOOD_DARK                      23
#define WALL_ELEVATOR_DOOR_LIGHT            24
#define WALL_ELEVATOR_DOOR_DARK             25
#define WALL_STEEL_WARNINGSIGN_LIGHT        26
#define WALL_STEEL_WARNINGSIGN_DARK         27
#define WALL_STEEL_LIGHT                    28
#define WALL_STEEL_DARK                     29
#define WALL_OUTSIDE_DAYTIME                30
#define WALL_OUTSIDE_NIGHT                  31
#define WALL_STONE_RED_LIGHT                32
#define WALL_STONE_RED_DARK                 33
#define WALL_STONE_RED_YELLOWBANNER_LIGHT   34
#define WALL_STONE_RED_YELLOWBANNER_DARK    35
#define WALL_PURPLE_LIGHT                   36
#define WALL_PURPLE_DARK                    37
#define WALL_PURPLE_BANNER_LIGHT            38
#define WALL_PURPLE_BANNER_DARK             39
#define WALL_ELEVATOR_INSIDE                40
#define WALL_ELEVATOR_SWITCH_DOWN           41
#define WALL_ELEVATOR_GREY                  42
#define WALL_ELEVATOR_SWITCH_UP             43
#define WALL_WOOD_CROSS_LIGHT               44
#define WALL_WOOD_CROSS_DARK                45
#define WALL_STONE_SLIME_LIGHT              46
#define WALL_STONE_SLIME_DARK               47
#define WALL_PURPLE_BLOOD_LIGHT             48
#define WALL_PURPLE_BLOOD_DARK              49
#define WALL_STONE_SLIME2_LIGHT             50
#define WALL_STONE_SLIME2_DARK              51
#define WALL_STONE_GREY3_LIGHT              52
#define WALL_STONE_GREY3_DARK               53
#define WALL_STONE_GREY3_WARNING_LIGHT      54
#define WALL_STONE_GREY3_WARNING_DARK       55

// WOLFENSTEIN 3D Full Version ids here

#define WALL_DOOR_LIGHT                     98
#define WALL_DOOR_DARK                      99
#define WALL_DOOR_LIGHT_SIDE                100
#define WALL_DOOR_DARK_SIDE                 101
#define WALL_ELEVATOR_DOOR_LIGHT_SIDE       102
#define WALL_ELEVATOR_DOOR_DARK_SIDE        103
#define WALL_DOOR_LOCKED_LIGHT_SIDE         104
#define WALL_DOOR_LOCKED_DARK_SIDE          105

VOID StartNewLevel(BYTE level);
VOID StartNextLevel();
VOID StartPrevLevel();
VOID ChangeScreen(BYTE nextScreen);
extern HWND g_hwnd;

#endif /* _COMMON_H_ */
