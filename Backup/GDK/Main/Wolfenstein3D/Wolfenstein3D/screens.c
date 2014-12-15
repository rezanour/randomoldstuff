#include "common.h"
extern HSPRITE* g_wallTextures;
USHORT* g_debugLayer1Data = NULL;
USHORT* g_debugLayer2Data = NULL;
BYTE    g_debugLastlevelNumber = 0;
BOOL    g_drawDebugScreen = FALSE;
BOOL    g_gameInProgress = FALSE;
BOOL    g_noHudGun = FALSE;
FLOAT   g_percentComplete = 0.0f;

extern MAPCELL* g_map;
extern USHORT g_mapWidth;
extern USHORT g_mapHeight;
extern ENEMY g_enemies[MAX_ENEMIES];

BOOL    g_debugLevelLegendMode = FALSE;
BOOL    g_debugLevelPatrolMarkersMode = FALSE;

#define MAX_SCREEN_SPRITES 64

#define COLOR_ORANGE        56
#define COLOR_BRIGHT_WHITE  15
#define COLOR_WHITE         18
#define COLOR_HI_YELLOW     14
#define COLOR_YELLOW        67
#define COLOR_LIGHT_YELLOW  68
#define COLOR_GREEN         107
#define COLOR_GRAY          8
#define COLOR_LIGHT_BLUE    3
#define COLOR_BLUE          1
#define COLOR_BROWN         6
#define COLOR_LIGHTBROWN    65
#define COLOR_RED           12
#define COLOR_PURPLE        5

#define SSFLAG_FILEBASED    0x000
#define SSFLAG_RAW          0x001
#define SSFLAG_UI           0x002
#define SSFLAG_SPRITE       0x004
#define SSFLAG_SUBSPRITE    0x008
#define SSFLAG_FONT         0x010

#define SSFLAG_RAWUI        SSFLAG_RAW|SSFLAG_UI
#define SSFLAG_RAWSPRITE    SSFLAG_RAW|SSFLAG_SPRITE

#define PROGRESS_ANIMATION_INTERVAL 0.001f
#define DIGIT_TERMINATOR            255
#define FACE_ANIMATION_INTERVAL     0.7f
#define FACE_ANIMATION_HURT_OFFSET  3

#define CURSOR_ANIMATION_INTERVAL   0.5f

#define SOUND_MENU_MOVE         0
#define SOUND_MENU_SELECT       1
#define SOUND_HUD_KNIFE         2
#define SOUND_HUD_PISTOL        3
#define SOUND_HUD_MACHINEGUN    4
#define SOUND_HUD_CHAINGUN      5
#define SOUND_TITLE_SCREEN      6
#define SOUND_SOLDIER_YELL      7
#define SOUND_DOG_YELL          8
#define SOUND_SS_YELL           9
#define SOUND_ZOMBIE_YELL       10
#define SOUND_OFFICER_YELL      11
#define SOUND_BOSS1_YELL        12

typedef VOID (*PMENUITEM_FUNC)();
typedef struct _MENU_ITEM
{
    CHAR* name;
    INT y;
    PMENUITEM_FUNC menuItemFunc;
}MENU_ITEM,*PMENU_ITEM;

typedef struct _SCREEN_SPRITE
{
    CHAR* filePath;
    HSPRITE sprite;
    USHORT id;
    USHORT flags;
}SCREEN_SPRITE, *PSCREEN_SPRITE;

typedef VOID (*PDRAWSCREEN_FUNC)(PSCREEN_SPRITE);

typedef struct _SCREEN_SOUND
{
    CHAR* filePath;
    HSOUND sound;
}SCREEN_SOUND, *PSCREEN_SOUND;

typedef struct _GAME_SCREEN
{
    BYTE id;
    PDRAWSCREEN_FUNC drawScreenFunc;
    SCREEN_SPRITE sprites[MAX_SCREEN_SPRITES];
}GAME_SCREEN,*PGAME_SCREEN;

VOID DestroyScreenSprites(PSCREEN_SPRITE sprites);
BOOL CreateScreenSprites(PSCREEN_SPRITE sprites);
VOID DrawFullScreenSprite(HSPRITE sprite);

VOID InitializeScreenSpriteTable();

VOID DrawSplashScreen(PSCREEN_SPRITE sprites);
VOID DrawRatingScreen(PSCREEN_SPRITE sprites);
VOID DrawOptionsScreen(PSCREEN_SPRITE sprites);
VOID DrawEpisodeScreen(PSCREEN_SPRITE sprites);
VOID DrawGameHud(PSCREEN_SPRITE sprites);
VOID DrawWelcomeStatsScreen(PSCREEN_SPRITE sprites);
VOID DrawSoundScreen(PSCREEN_SPRITE sprites);
VOID DrawDifficultyScreen(PSCREEN_SPRITE sprites);
VOID DrawLevelDebugScreen(PSCREEN_SPRITE sprites);
VOID DrawGetPsychedScreen(PSCREEN_SPRITE sprites);

VOID DrawDigits(PSCREEN_SPRITE sprites, INT x, INT y, BYTE* digits, INT count);
VOID NumberToDigitArray(INT value, BYTE* digits, INT count);

VOID DrawSmallString(PSCREEN_SPRITE sprites, INT x, INT y, CHAR* text);
VOID DrawString(PSCREEN_SPRITE sprites, INT x, INT y, BYTE color, CHAR* text);

VOID MenuItem_NewGame();
VOID MenuItem_Sound();
VOID MenuItem_BackToGame();
VOID MenuItem_ExitGame();
VOID MenuItem_ToggleSoundOnOff();
VOID MenuItem_ToggleMusicOnOff();

VOID MenuItem_CanIPlayDaddy();
VOID MenuItem_DontHurtMe();
VOID MenuItem_BringEmOn();
VOID MenuItem_DeathIncarnate();

VOID MenuItem_Episode1Selected();
VOID MenuItem_Episode2Selected();
VOID MenuItem_Episode3Selected();
VOID MenuItem_Episode4Selected();
VOID MenuItem_Episode5Selected();
VOID MenuItem_Episode6Selected();

VOID PlayScreenSound(INT soundId, BOOL playNow);

BOOL g_hackSoundPlayed = FALSE;

HSOUND g_bang = INVALID_HSOUND_VALUE;

SCREEN_SOUND g_screenSounds[] = 
{
    {NULL, INVALID_HSOUND_VALUE},                                   // SOUND_MENU_MOVE
    {NULL, INVALID_HSOUND_VALUE},                                   // SOUND_MENU_SELECT
    {NULL, INVALID_HSOUND_VALUE},                                   // SOUND_HUD_KNIFE
    {CONTENT_ROOT"\\wavs\\PistolShot.wav", INVALID_HSOUND_VALUE},   // SOUND_HUD_PISTOL
    {CONTENT_ROOT"\\wavs\\MachineGun.wav", INVALID_HSOUND_VALUE},   // SOUND_HUD_MACHINEGUN
    {CONTENT_ROOT"\\wavs\\ChaingunShot.wav", INVALID_HSOUND_VALUE},      // SOUND_HUD_CHAINGUN
    {NULL, INVALID_HSOUND_VALUE},                                   // SOUND_TITLE_SCREEN

    {CONTENT_ROOT"\\wavs\\SoldierYell.wav", INVALID_HSOUND_VALUE},  // SOUND_SOLDIER_YELL
    {CONTENT_ROOT"\\wavs\\DogYell.wav", INVALID_HSOUND_VALUE},      // SOUND_DOG_YELL
    {CONTENT_ROOT"\\wavs\\SSYell.wav", INVALID_HSOUND_VALUE},       // SOUND_SS_YELL
    {CONTENT_ROOT"\\wavs\\ZombieYell.wav", INVALID_HSOUND_VALUE},   // SOUND_ZOMBIE_YELL
    {CONTENT_ROOT"\\wavs\\OfficerYell.wav", INVALID_HSOUND_VALUE},  // SOUND_OFFICER_YELL

    {CONTENT_ROOT"\\wavs\\Boss1Yell.wav", INVALID_HSOUND_VALUE},  // SOUND_BOSS1_YELL
};

GAME_SCREEN g_screens[] = 
{
    { SPLASH_SCREEN, DrawSplashScreen   }, /* splash screen (wolfenstein title screen) */
    { RATING_SCREEN, DrawRatingScreen   }, /* blue PC-13 rating screen                 */
    { OPTIONS_SCREEN, DrawOptionsScreen }, /* options screen, new game, exit, etc.     */
    { EPISODE_SCREEN, DrawEpisodeScreen }, /* episode selection screen                 */
    { GAMEHUD_SCREEN, DrawGameHud       }, /* game hud                                 */
    { WELCOME_STATS_SCREEN, DrawWelcomeStatsScreen },   /* game welcome and PC stats screen */
    { SOUND_SCREEN, DrawSoundScreen },                  /* sound options screen */
    { DIFFICULTY_SCREEN, DrawDifficultyScreen },        /* choose game difficulty setting screen */
    { LEVELDEBUG_SCREEN, DrawLevelDebugScreen },        /* debug only!!! */
    { GETPSYCHED_SCREEN, DrawGetPsychedScreen },
};

INT g_currentEpisodeMenuItem = 0;
MENU_ITEM g_episodesMenu[] = 
{
    { "Episode 1\nEscape from Wolfenstein",  20,  MenuItem_Episode1Selected},
    { "Episode 2\nOperation: Eisenfaust",    47,  MenuItem_Episode2Selected},
    { "Episode 3\nDie, Fuhrer, Die!",        74,  MenuItem_Episode3Selected},
    { "Episode 4\nA Dark Secret",           101,  MenuItem_Episode4Selected},
    { "Episode 5\nTrail of a Mad Man",      128,  MenuItem_Episode5Selected},
    { "Episode 6\nConfrontation",           155,  MenuItem_Episode6Selected},
};

INT g_currentDifficultyMenuItem = 0;
MENU_ITEM g_difficultyMenu[] = 
{
    { "Can I play, Daddy?",    100,  MenuItem_CanIPlayDaddy},
    { "Don't hurt me.",        113,  MenuItem_DontHurtMe},
    { "Bring 'em on!",         127,  MenuItem_BringEmOn},
    { "I am Death incarnate",  139,  MenuItem_DeathIncarnate},
};

INT g_currentOptionsMenuItem = 0;
MENU_ITEM g_optionsMenu[] = 
{
    { "New Game",     52,  MenuItem_NewGame},
    { "Sound",        65,  MenuItem_Sound},
    { "Back to Game", 78,  MenuItem_BackToGame},
/*
    { "Load Game",    91,  NULL},
    { "Save Game",    104, NULL},
    { "Change View",  117, NULL},
    { "Read This",    130, NULL},
    { "View Scores",  143, NULL},
    { "Back To Demo", 156, NULL}, */
    { "Quit",         169, MenuItem_ExitGame }
};

INT g_currentSoundMenuItem = 0;
MENU_ITEM g_soundMenu[] = 
{
    { "None",                  20,  MenuItem_ToggleSoundOnOff},
    { "PC Speaker",            32,  MenuItem_ToggleSoundOnOff},
    { "AdLib/Sound Blaster",   44,  MenuItem_ToggleSoundOnOff},
    { "None",                  85,  MenuItem_ToggleSoundOnOff},
    { "Disney Sound Source",   97,  MenuItem_ToggleSoundOnOff},
    { "Sound Blaster",        109, MenuItem_ToggleSoundOnOff},
    { "None",                 150, MenuItem_ToggleMusicOnOff},
    { "AdLib/Sound Blaster",  162, MenuItem_ToggleMusicOnOff},
};

#define HUD_BITMAP              0
#define HUD_DUDELOOKING_AHEAD   1
#define HUD_DUDELOOKING_LEFT    2
#define HUD_DUDELOOKING_RIGHT   3
#define HUD_DUDELOOKING_AHEAD_1 4
#define HUD_DUDELOOKING_LEFT_1  5
#define HUD_DUDELOOKING_RIGHT_1 6
#define HUD_DUDELOOKING_AHEAD_2 7
#define HUD_DUDELOOKING_LEFT_2  8
#define HUD_DUDELOOKING_RIGHT_2 9
#define HUD_DUDELOOKING_AHEAD_3 10
#define HUD_DUDELOOKING_LEFT_3  11
#define HUD_DUDELOOKING_RIGHT_3 12
#define HUD_DUDELOOKING_AHEAD_4 13
#define HUD_DUDELOOKING_LEFT_4  14
#define HUD_DUDELOOKING_RIGHT_4 15
#define HUD_DUDELOOKING_AHEAD_5 16
#define HUD_DUDELOOKING_LEFT_5  17
#define HUD_DUDELOOKING_RIGHT_5 18
#define HUD_DUDELOOKING_AHEAD_6 19
#define HUD_DUDELOOKING_LEFT_6  20
#define HUD_DUDELOOKING_RIGHT_6 21
#define HUD_DUDELOOKING_AHEAD_DEAD 22

#define HUD_KNIFE             23
#define HUD_PISTOL            24
#define HUD_MACHINEGUN        25
#define HUD_CHAINGUN          26

#define HUD_0                 27
#define HUD_1                 28
#define HUD_2                 29
#define HUD_3                 30
#define HUD_4                 31
#define HUD_5                 32
#define HUD_6                 33
#define HUD_7                 34
#define HUD_8                 35
#define HUD_9                 36

#define HUD_KNIFE_0           37
#define HUD_KNIFE_1           38
#define HUD_KNIFE_2           39
#define HUD_KNIFE_3           40
#define HUD_KNIFE_4           41
#define HUD_PISTOL_0          42
#define HUD_PISTOL_1          43
#define HUD_PISTOL_2          44
#define HUD_PISTOL_3          45
#define HUD_PISTOL_4          46
#define HUD_MACHINEGUN_0      47
#define HUD_MACHINEGUN_1      48
#define HUD_MACHINEGUN_2      49
#define HUD_MACHINEGUN_3      50
#define HUD_MACHINEGUN_4      51
#define HUD_CHAINGUN_0        52
#define HUD_CHAINGUN_1        53
#define HUD_CHAINGUN_2        54
#define HUD_CHAINGUN_3        55
#define HUD_CHAINGUN_4        56

#define HUD_KEYBOX            57
#define HUD_SILVERKEY         58
#define HUD_GOLDKEY           59

FLOAT g_GetPsychedAnimationTime = 0;

FLOAT g_faceAnimationTime = 0;
BYTE g_currentFaceFrame = HUD_DUDELOOKING_AHEAD;
BYTE g_faceAnimationFrames[] = 
{
    HUD_DUDELOOKING_AHEAD,
    HUD_DUDELOOKING_LEFT,
    HUD_DUDELOOKING_AHEAD,
    HUD_DUDELOOKING_RIGHT
};

#define OPTIONS_BITMAP          0
#define OPTIONS_CURSOR_0        1
#define OPTIONS_CURSOR_1        2
#define OPTIONS_MENUBACKGROUND  3
#define OPTIONS_TOPBORDER       4
#define OPTIONS_ESC_BACK        5

FLOAT g_cursorAnimationTime = 0;
BYTE g_currentCursorFrame = OPTIONS_CURSOR_0;

SCREEN_SPRITE g_gameHudSprites[] = 
{
    { NULL, INVALID_HSPRITE_VALUE, UI_HUD, SSFLAG_RAWUI  },   /* HUD_BITMAP            */

    { NULL, INVALID_HSPRITE_VALUE, UI_HUD_FACE_FRONT , SSFLAG_RAWUI },  /* HUD_DUDELOOKING_AHEAD */
    { NULL, INVALID_HSPRITE_VALUE, UI_HUD_FACE_LEFT  , SSFLAG_RAWUI },  /* HUD_DUDELOOKING_LEFT  */
    { NULL, INVALID_HSPRITE_VALUE, UI_HUD_FACE_RIGHT , SSFLAG_RAWUI },  /* HUD_DUDELOOKING_RIGHT */
    { NULL, INVALID_HSPRITE_VALUE, UI_HUD_FACE_FRONT1, SSFLAG_RAWUI },  /* HUD_DUDELOOKING_AHEAD_1 */
    { NULL, INVALID_HSPRITE_VALUE, UI_HUD_FACE_LEFT1 , SSFLAG_RAWUI },  /* HUD_DUDELOOKING_LEFT_1  */
    { NULL, INVALID_HSPRITE_VALUE, UI_HUD_FACE_RIGHT1, SSFLAG_RAWUI },  /* HUD_DUDELOOKING_RIGHT_1 */
    { NULL, INVALID_HSPRITE_VALUE, UI_HUD_FACE_FRONT2, SSFLAG_RAWUI },  /* HUD_DUDELOOKING_AHEAD_2 */
    { NULL, INVALID_HSPRITE_VALUE, UI_HUD_FACE_LEFT2 , SSFLAG_RAWUI },  /* HUD_DUDELOOKING_LEFT_2  */
    { NULL, INVALID_HSPRITE_VALUE, UI_HUD_FACE_RIGHT2, SSFLAG_RAWUI },  /* HUD_DUDELOOKING_RIGHT_2 */
    { NULL, INVALID_HSPRITE_VALUE, UI_HUD_FACE_FRONT3, SSFLAG_RAWUI },  /* HUD_DUDELOOKING_AHEAD_3 */
    { NULL, INVALID_HSPRITE_VALUE, UI_HUD_FACE_LEFT3 , SSFLAG_RAWUI },  /* HUD_DUDELOOKING_LEFT_3  */
    { NULL, INVALID_HSPRITE_VALUE, UI_HUD_FACE_RIGHT3, SSFLAG_RAWUI },  /* HUD_DUDELOOKING_RIGHT_3 */
    { NULL, INVALID_HSPRITE_VALUE, UI_HUD_FACE_FRONT4, SSFLAG_RAWUI },  /* HUD_DUDELOOKING_AHEAD_4 */
    { NULL, INVALID_HSPRITE_VALUE, UI_HUD_FACE_LEFT4 , SSFLAG_RAWUI },  /* HUD_DUDELOOKING_LEFT_4  */
    { NULL, INVALID_HSPRITE_VALUE, UI_HUD_FACE_RIGHT4, SSFLAG_RAWUI },  /* HUD_DUDELOOKING_RIGHT_4 */
    { NULL, INVALID_HSPRITE_VALUE, UI_HUD_FACE_FRONT5, SSFLAG_RAWUI },  /* HUD_DUDELOOKING_AHEAD_5 */
    { NULL, INVALID_HSPRITE_VALUE, UI_HUD_FACE_LEFT5 , SSFLAG_RAWUI },  /* HUD_DUDELOOKING_LEFT_5  */
    { NULL, INVALID_HSPRITE_VALUE, UI_HUD_FACE_RIGHT5, SSFLAG_RAWUI },  /* HUD_DUDELOOKING_RIGHT_5 */
    { NULL, INVALID_HSPRITE_VALUE, UI_HUD_FACE_FRONT6, SSFLAG_RAWUI },  /* HUD_DUDELOOKING_AHEAD_6 */
    { NULL, INVALID_HSPRITE_VALUE, UI_HUD_FACE_LEFT6 , SSFLAG_RAWUI },  /* HUD_DUDELOOKING_LEFT_6  */
    { NULL, INVALID_HSPRITE_VALUE, UI_HUD_FACE_RIGHT6, SSFLAG_RAWUI },  /* HUD_DUDELOOKING_RIGHT_6 */
    { NULL, INVALID_HSPRITE_VALUE, UI_HUD_FACE_DEAD  , SSFLAG_RAWUI },  /* HUD_DUDELOOKING_AHEAD_DEAD */

    { NULL, INVALID_HSPRITE_VALUE, UI_HUD_KNIFE     , SSFLAG_RAWUI },  /* HUD_KNIFE */
    { NULL, INVALID_HSPRITE_VALUE, UI_HUD_PISTOL    , SSFLAG_RAWUI },  /* HUD_PISTOL */
    { NULL, INVALID_HSPRITE_VALUE, UI_HUD_MACHINEGUN, SSFLAG_RAWUI },  /* HUD_MACHINEGUN */
    { NULL, INVALID_HSPRITE_VALUE, UI_HUD_CHAINGUN  , SSFLAG_RAWUI },  /* HUD_CHAINGUN */
      
    { NULL, INVALID_HSPRITE_VALUE, UI_HUD_CH_0, SSFLAG_RAWUI },  /* HUD_0 */
    { NULL, INVALID_HSPRITE_VALUE, UI_HUD_CH_1, SSFLAG_RAWUI },  /* HUD_1 */
    { NULL, INVALID_HSPRITE_VALUE, UI_HUD_CH_2, SSFLAG_RAWUI },  /* HUD_2 */
    { NULL, INVALID_HSPRITE_VALUE, UI_HUD_CH_3, SSFLAG_RAWUI },  /* HUD_3 */
    { NULL, INVALID_HSPRITE_VALUE, UI_HUD_CH_4, SSFLAG_RAWUI },  /* HUD_4 */
    { NULL, INVALID_HSPRITE_VALUE, UI_HUD_CH_5, SSFLAG_RAWUI },  /* HUD_5 */
    { NULL, INVALID_HSPRITE_VALUE, UI_HUD_CH_6, SSFLAG_RAWUI },  /* HUD_6 */
    { NULL, INVALID_HSPRITE_VALUE, UI_HUD_CH_7, SSFLAG_RAWUI },  /* HUD_7 */
    { NULL, INVALID_HSPRITE_VALUE, UI_HUD_CH_8, SSFLAG_RAWUI },  /* HUD_8 */
    { NULL, INVALID_HSPRITE_VALUE, UI_HUD_CH_9, SSFLAG_RAWUI },  /* HUD_9 */

    { NULL, INVALID_HSPRITE_VALUE, KNIFE_IDLE_FRAME    , SSFLAG_RAWSPRITE },  /* HUD_KNIFE_0 */
    { NULL, INVALID_HSPRITE_VALUE, KNIFE_ATTACK_FRAME_0, SSFLAG_RAWSPRITE  },  /* HUD_KNIFE_1 */
    { NULL, INVALID_HSPRITE_VALUE, KNIFE_ATTACK_FRAME_1, SSFLAG_RAWSPRITE  },  /* HUD_KNIFE_2 */
    { NULL, INVALID_HSPRITE_VALUE, KNIFE_ATTACK_FRAME_2, SSFLAG_RAWSPRITE  },  /* HUD_KNIFE_3 */
    { NULL, INVALID_HSPRITE_VALUE, KNIFE_ATTACK_FRAME_3, SSFLAG_RAWSPRITE  },  /* HUD_KNIFE_4 */

    { NULL, INVALID_HSPRITE_VALUE, PISTOL_IDLE_FRAME    , SSFLAG_RAWSPRITE  },  /* HUD_PISTOL_0 */
    { NULL, INVALID_HSPRITE_VALUE, PISTOL_ATTACK_FRAME_0, SSFLAG_RAWSPRITE  },  /* HUD_PISTOL_1 */
    { NULL, INVALID_HSPRITE_VALUE, PISTOL_ATTACK_FRAME_1, SSFLAG_RAWSPRITE  },  /* HUD_PISTOL_2 */
    { NULL, INVALID_HSPRITE_VALUE, PISTOL_ATTACK_FRAME_2, SSFLAG_RAWSPRITE  },  /* HUD_PISTOL_3 */
    { NULL, INVALID_HSPRITE_VALUE, PISTOL_ATTACK_FRAME_3, SSFLAG_RAWSPRITE  },  /* HUD_PISTOL_4 */

    { NULL, INVALID_HSPRITE_VALUE, MACHINEGUN_IDLE_FRAME    , SSFLAG_RAWSPRITE  },  /* HUD_MACHINEGUN_0 */
    { NULL, INVALID_HSPRITE_VALUE, MACHINEGUN_ATTACK_FRAME_0, SSFLAG_RAWSPRITE  },  /* HUD_MACHINEGUN_1 */
    { NULL, INVALID_HSPRITE_VALUE, MACHINEGUN_ATTACK_FRAME_1, SSFLAG_RAWSPRITE  },  /* HUD_MACHINEGUN_2 */
    { NULL, INVALID_HSPRITE_VALUE, MACHINEGUN_ATTACK_FRAME_2, SSFLAG_RAWSPRITE  },  /* HUD_MACHINEGUN_3 */
    { NULL, INVALID_HSPRITE_VALUE, MACHINEGUN_ATTACK_FRAME_3, SSFLAG_RAWSPRITE  },  /* HUD_MACHINEGUN_4 */

    { NULL, INVALID_HSPRITE_VALUE, CHAINGUN_IDLE_FRAME    , SSFLAG_RAWSPRITE  },  /* HUD_CHAINGUN_0 */
    { NULL, INVALID_HSPRITE_VALUE, CHAINGUN_ATTACK_FRAME_0, SSFLAG_RAWSPRITE  },  /* HUD_CHAINGUN_1 */
    { NULL, INVALID_HSPRITE_VALUE, CHAINGUN_ATTACK_FRAME_1, SSFLAG_RAWSPRITE  },  /* HUD_CHAINGUN_2 */
    { NULL, INVALID_HSPRITE_VALUE, CHAINGUN_ATTACK_FRAME_2, SSFLAG_RAWSPRITE  },  /* HUD_CHAINGUN_3 */
    { NULL, INVALID_HSPRITE_VALUE, CHAINGUN_ATTACK_FRAME_3, SSFLAG_RAWSPRITE  },  /* HUD_CHAINGUN_4 */

    { NULL, INVALID_HSPRITE_VALUE, UI_HUD_NOKEY, SSFLAG_RAWUI },  /* HUD_KEYBOX    */
    { NULL, INVALID_HSPRITE_VALUE, UI_HUD_SILVERKEY, SSFLAG_RAWUI },  /* HUD_SILVERKEY */
    { NULL, INVALID_HSPRITE_VALUE, UI_HUD_GOLDKEY, SSFLAG_RAWUI },  /* HUD_GOLDKEY   */
};

#define SPLASH_BITMAP 0
SCREEN_SPRITE g_splashSprites[] = 
{
    { NULL, INVALID_HSPRITE_VALUE, UI_TITLE, SSFLAG_RAWUI },   /* SPLASH_BITMAP  */
};

#define RATINGS_BITMAP 0
SCREEN_SPRITE g_ratingsSprites[] = 
{
    { NULL, INVALID_HSPRITE_VALUE, UI_PC13, SSFLAG_RAWUI },   /* RATINGS_BITMAP  */
};

SCREEN_SPRITE g_fontSprites[96] = {0};

SCREEN_SPRITE g_optionsSprites[] = 
{
    { NULL, INVALID_HSPRITE_VALUE, UI_OPTIONS_TITLE, SSFLAG_RAWUI},  /* OPTIONS_BITMAP    */
    { NULL, INVALID_HSPRITE_VALUE, UI_CURSORGUN_LIGHT, SSFLAG_RAWUI },  /* OPTIONS_CURSOR_0  */
    { NULL, INVALID_HSPRITE_VALUE, UI_CURSORGUN_DARK, SSFLAG_RAWUI },  /* OPTIONS_CURSOR_1  */
    { NULL, INVALID_HSPRITE_VALUE, UI_CURSORGUN_LIGHT, SSFLAG_RAWUI },  /* OPTIONS_MENUBACKGROUND  */
    { NULL, INVALID_HSPRITE_VALUE, UI_OPTIONS_TITLE, SSFLAG_RAWUI },  /* OPTIONS_TOPBORDER  */
    { NULL, INVALID_HSPRITE_VALUE, UI_ESC_BACK_BOTTOM, SSFLAG_RAWUI },  /* OPTIONS_ESC_BACK */
};

#define DIFFICULTY_CURSOR_0         1
#define DIFFICULTY_CURSOR_1         2
#define DIFFICULTY_MENUBACKGROUND   3
#define DIFFICULTY_ESC_BACK         4
#define DIFFICULTY_EASYPIC          5
#define DIFFICULTY_MEDIUMPIC        6
#define DIFFICULTY_HARDPIC          7
#define DIFFICULTY_SUPERHARDPIC     8

SCREEN_SPRITE g_difficultySprites[] = 
{
    { NULL, INVALID_HSPRITE_VALUE, UI_OPTIONS_TITLE, SSFLAG_RAWUI},  /* OPTIONS_BITMAP    */
    { NULL, INVALID_HSPRITE_VALUE, UI_CURSORGUN_LIGHT, SSFLAG_RAWUI },  /* DIFFICULTY_CURSOR_1  */
    { NULL, INVALID_HSPRITE_VALUE, UI_CURSORGUN_DARK, SSFLAG_RAWUI },  /* DIFFICULTY_CURSOR_0  */
    { NULL, INVALID_HSPRITE_VALUE, UI_CURSORGUN_LIGHT, SSFLAG_RAWUI },  /* DIFFICULTY_MENUBACKGROUND  */
    { NULL, INVALID_HSPRITE_VALUE, UI_ESC_BACK_BOTTOM, SSFLAG_RAWUI },  /* DIFFICULTY_ESC_BACK */

    { NULL, INVALID_HSPRITE_VALUE, UI_CANIPLAYDADDY_FACE, SSFLAG_RAWUI },  /* DIFFICULTY_EASYPIC      */
    { NULL, INVALID_HSPRITE_VALUE, UI_DONTHURTME_FACE   , SSFLAG_RAWUI },  /* DIFFICULTY_MEDIUMPIC    */
    { NULL, INVALID_HSPRITE_VALUE, UI_BRINGEMON_FACE    , SSFLAG_RAWUI },  /* DIFFICULTY_HARDPIC      */
    { NULL, INVALID_HSPRITE_VALUE, UI_IAMDEATH_FACE     , SSFLAG_RAWUI },  /* DIFFICULTY_SUPERHARDPIC */
};

#define DEBUG_BLOCK 0
SCREEN_SPRITE g_levelDebugSprites[] = 
{
    { NULL, INVALID_HSPRITE_VALUE, UI_GRAYSQUARE, SSFLAG_RAWUI},  /* DEBUG_BLOCK */
};

#define SOUND_BITMAP            0
#define SOUND_OPTION_SELECTED   3
#define SOUND_OPTION_UNSELECTED 4
#define SOUND_OPTION_SOUNDEFFECTS_LABEL 5
#define SOUND_OPTION_DIGITIZED_LABEL    6
#define SOUND_OPTION_MUSIC_LABEL        7
#define SOUND_OPTION_ESC_BACK           8
#define SOUND_OPTIONS_MENUBACKGROUND    9

SCREEN_SPRITE g_soundSprites[] = 
{
    { NULL, INVALID_HSPRITE_VALUE }, /* SOUND_BITMAP          */
    { NULL, INVALID_HSPRITE_VALUE, UI_CURSORGUN_LIGHT, SSFLAG_RAWUI },  /* OPTIONS_CURSOR_0      */
    { NULL, INVALID_HSPRITE_VALUE, UI_CURSORGUN_DARK, SSFLAG_RAWUI },  /* OPTIONS_CURSOR_1      */
    { NULL, INVALID_HSPRITE_VALUE, UI_OPTIONTOGGLE_ON, SSFLAG_RAWUI },  /* SOUND_OPTION_SELECTED */
    { NULL, INVALID_HSPRITE_VALUE, UI_OPTIONTOGGLE_OFF, SSFLAG_RAWUI },  /* SOUND_OPTION_UNSELECTED */

    { NULL, INVALID_HSPRITE_VALUE, UI_SOUNDEFFECTS_TITLE  , SSFLAG_RAWUI },  /* SOUND_OPTION_SOUNDEFFECTS_LABEL */
    { NULL, INVALID_HSPRITE_VALUE, UI_DIGITIZEDSOUND_TITLE, SSFLAG_RAWUI },  /* SOUND_OPTION_DIGITIZED_LABEL */
    { NULL, INVALID_HSPRITE_VALUE, UI_MUSIC_TITLE         , SSFLAG_RAWUI },  /* SOUND_OPTION_MUSIC_LABEL */
    { NULL, INVALID_HSPRITE_VALUE, UI_ESC_BACK_BOTTOM     , SSFLAG_RAWUI },  /* SOUND_OPTION_ESC_BACK */
    { NULL, INVALID_HSPRITE_VALUE, UI_CURSORGUN_LIGHT, SSFLAG_RAWUI },  /* SOUND_OPTIONS_MENUBACKGROUND  */

};

#define WELCOMESTATS_BITMAP 0
#define WELCOMESTATS_TOPBORDER 1
SCREEN_SPRITE g_welcomeStatsSprites[] = 
{
    { NULL, INVALID_HSPRITE_VALUE, UI_WOLFENSTEIN3D_TITLE, SSFLAG_RAWUI},  /* WELCOMESTATS_BITMAP */
    { NULL, INVALID_HSPRITE_VALUE, UI_OPTIONS_TITLE, SSFLAG_RAWUI },  /* WELCOMESTATS_TOPBORDER  */
};

#define EPISODES_MENUBACKGROUND 0
#define EPISODES_1              3
#define EPISODES_2              4
#define EPISODES_3              5
#define EPISODES_4              6
#define EPISODES_5              7
#define EPISODES_6              8
#define EPISODES_ESC_BACK       9
SCREEN_SPRITE g_episodesSprites[] = 
{
    { NULL, INVALID_HSPRITE_VALUE, UI_CURSORGUN_LIGHT, SSFLAG_RAWUI },  /* EPISODES_MENUBACKGROUND  */
    { NULL, INVALID_HSPRITE_VALUE, UI_CURSORGUN_LIGHT, SSFLAG_RAWUI },  /* OPTIONS_CURSOR_0  */
    { NULL, INVALID_HSPRITE_VALUE, UI_CURSORGUN_DARK, SSFLAG_RAWUI },  /* OPTIONS_CURSOR_1  */
    { NULL, INVALID_HSPRITE_VALUE, UI_EPISODE1_PIC, SSFLAG_RAWUI },  /* EPISODES_1  */
    { NULL, INVALID_HSPRITE_VALUE, UI_EPISODE2_PIC, SSFLAG_RAWUI },  /* EPISODES_2  */
    { NULL, INVALID_HSPRITE_VALUE, UI_EPISODE3_PIC, SSFLAG_RAWUI },  /* EPISODES_3  */
    { NULL, INVALID_HSPRITE_VALUE, UI_EPISODE4_PIC, SSFLAG_RAWUI },  /* EPISODES_4  */
    { NULL, INVALID_HSPRITE_VALUE, UI_EPISODE5_PIC, SSFLAG_RAWUI },  /* EPISODES_5  */
    { NULL, INVALID_HSPRITE_VALUE, UI_EPISODE6_PIC, SSFLAG_RAWUI },  /* EPISODES_6  */
    { NULL, INVALID_HSPRITE_VALUE, UI_ESC_BACK_BOTTOM, SSFLAG_RAWUI },  /* EPISODES_ESC_BACK */
};

#define GETPSYCHED_TITLE 0
#define GETPSYCHED_PROGRESS_LINE 1
SCREEN_SPRITE g_getPsychedSprites[] = 
{
    { NULL, INVALID_HSPRITE_VALUE, UI_GETPSYCHED_TITLE, SSFLAG_RAWUI },  /* GETPSYCHED_TITLE  */
    { NULL, INVALID_HSPRITE_VALUE, UI_GRAYSQUARE, SSFLAG_RAWUI},         /* GETPSYCHED_PROGRESS_LINE */
};

BOOL ScreenCreate(BYTE screen)
{
    if (screen != INVALID_SCREEN)
    {
        return CreateScreenSprites(g_screens[screen].sprites);
    }
    else
    {
        return FALSE;
    }
}

VOID ScreenDestroy(BYTE screen)
{
    if (screen != INVALID_SCREEN)
    {
        DestroyScreenSprites(g_screens[screen].sprites);
        //DestroyScreenSprites(g_fontSprites);
        ShutdownScreenSounds();
    }
}

VOID InitializeScreens()
{
    BYTE i = 0;
    BYTE s = 0;

#define AWESOME(x) \
    { \
        for (s = 0; s < MAX_SCREEN_SPRITES; s++) \
        { \
            if (s < ARRAYSIZE(x)) \
            { \
                memcpy(&g_screens[i].sprites[s], &x[s], sizeof(SCREEN_SPRITE)); \
            } \
            else \
            { \
                g_screens[i].sprites[s].sprite = INVALID_HSPRITE_VALUE; \
            } \
        } \
    }

    for (i = 0; i < ARRAYSIZE(g_screens); i++)
    {
        switch(i)
        {
        case SPLASH_SCREEN:         AWESOME(g_splashSprites);           break;
        case RATING_SCREEN:         AWESOME(g_ratingsSprites);          break;
        case OPTIONS_SCREEN:        AWESOME(g_optionsSprites);          break;
        case SOUND_SCREEN:          AWESOME(g_soundSprites);            break;
        case WELCOME_STATS_SCREEN:  AWESOME(g_welcomeStatsSprites);     break;
        case EPISODE_SCREEN:        AWESOME(g_episodesSprites);         break;
        case GAMEHUD_SCREEN:        AWESOME(g_gameHudSprites);          break;
        case DIFFICULTY_SCREEN:     AWESOME(g_difficultySprites);       break;
        case LEVELDEBUG_SCREEN:     AWESOME(g_levelDebugSprites);       break;
        case GETPSYCHED_SCREEN:     AWESOME(g_getPsychedSprites);       break;
        default:
            break;
        }
    }
}

VOID DestroyScreenSprites(PSCREEN_SPRITE sprites)
{
    BYTE i = 0;
    for (i = 0; i < MAX_SCREEN_SPRITES; i++)
    {
        if (W3DIsValidSprite(sprites[i].sprite))
        {
            W3DDestroySprite(sprites[i].sprite);
            sprites[i].sprite = INVALID_HSPRITE_VALUE;
            if (sprites[i].flags & SSFLAG_SUBSPRITE)
            {
                sprites[i].flags &= ~SSFLAG_SUBSPRITE;
            }
        }
    }
}

VOID ShutdownScreenSounds()
{
    UINT i;
    for (i = 0; i < ARRAYSIZE(g_screenSounds); ++i)
    {
        AudioDestroy(g_screenSounds[i].sound);
        g_screenSounds[i].sound = INVALID_HSOUND_VALUE;
    }
}

HSPRITE W3DCreateFontSprite(CHAR ch)
{
    HSPRITE sprite = INVALID_HSPRITE_VALUE;
    HSPRITE subSprite = INVALID_HSPRITE_VALUE;
    RECT rect;
    RectFromCharacter(ch, &rect);

    sprite = W3DCreateSpriteFromFile(fontFilePath);
    subSprite = W3DCreateSubSprite(sprite, &rect);
    W3DDestroySprite(sprite);

    return subSprite;
}

VOID CreateFontSprites()
{
    if (!W3DIsValidSprite(g_fontSprites[0].sprite))
    {
        BYTE s = 0;
        for (s = 0; s < ARRAYSIZE(g_fontSprites); s++)
        {
            g_fontSprites[s].filePath = NULL;
            g_fontSprites[s].id       = s + 32;
            g_fontSprites[s].sprite   = W3DCreateFontSprite(g_fontSprites[s].id);
            g_fontSprites[s].flags    = SSFLAG_FONT;
        }
    }
}

BOOL CreateScreenSprites(PSCREEN_SPRITE sprites)
{
    BOOL success = TRUE;
    BYTE i = 0;
    RECT rect;

    HSPRITE subSprite = INVALID_HSPRITE_VALUE;
    HSPRITE someSprite = INVALID_HSPRITE_VALUE;

    /* debug */
    rect.left = 22;
    rect.top = 15;
    rect.right = rect.left + 24;
    rect.bottom = rect.top + 16;

    for (i = 0; i < MAX_SCREEN_SPRITES; i++)
    {
        if (sprites[i].sprite == INVALID_HSPRITE_VALUE)
        {
            switch( sprites[i].flags)
            {
            case SSFLAG_RAWUI:
                sprites[i].sprite = W3DCreateSprite(sprites[i].id, TRUE);
                break;
            case SSFLAG_RAWSPRITE:
                sprites[i].sprite = W3DCreateSprite(sprites[i].id, FALSE);
                break;
            case SSFLAG_FONT:
                sprites[i].sprite = W3DCreateFontSprite((CHAR)sprites[i].id);
                break;
            }
        }
    }

    if (!success)
    {
        DestroyScreenSprites(sprites);
    }

    return success;
}

VOID CreateScreenSubSprite(PSCREEN_SPRITE sprites, INT id, RECT* srcRect)
{
    HSPRITE subSprite = INVALID_HSPRITE_VALUE;
    if (!(sprites[id].flags & SSFLAG_SUBSPRITE))
    {
        subSprite = W3DCreateSubSprite(sprites[id].sprite, srcRect);
        W3DDestroySprite(sprites[id].sprite);
        sprites[id].sprite = subSprite;
        sprites[id].flags |= SSFLAG_SUBSPRITE;
    }
}

VOID DrawFullScreenSprite(HSPRITE sprite)
{
    RECT dest = { 0, 0, W3DGetScreenWidth(), W3DGetScreenHeight() };
    W3DDrawSprite2D(sprite, &dest);
}

VOID DrawSplashScreen(PSCREEN_SPRITE sprites) 
{
    CreateScreenSprites(sprites);
    DrawFullScreenSprite(sprites[SPLASH_BITMAP].sprite);
    if (IsKeyJustPressed(VK_RETURN) || IsKeyJustPressed(VK_SPACE))
    {
        ChangeScreen(OPTIONS_SCREEN);
    }
}

VOID DrawRatingScreen(PSCREEN_SPRITE sprites)
{
    RECT dest = { 0, 0, W3DGetScreenWidth()*30, W3DGetScreenHeight() };
    CreateScreenSprites(sprites);
    W3DDrawSprite2D(sprites[SPLASH_BITMAP].sprite, &dest); /* render oversized (to color entire background) */

    dest.top    = W3DGetScreenHeight() - W3DGetSpriteHeight(sprites[RATINGS_BITMAP].sprite);
    dest.left   = W3DGetScreenWidth() - W3DGetSpriteWidth(sprites[RATINGS_BITMAP].sprite);
    dest.right  = dest.left + W3DGetSpriteWidth(sprites[RATINGS_BITMAP].sprite);
    dest.bottom = dest.top + W3DGetSpriteHeight(sprites[RATINGS_BITMAP].sprite);
    
    W3DDrawSprite2D(sprites[RATINGS_BITMAP].sprite, &dest);

    if (IsKeyJustPressed(VK_RETURN) || IsKeyJustPressed(VK_SPACE))
    {
        ChangeScreen(SPLASH_SCREEN);
    }
}

VOID UpdateCursorAnimationFrame()
{
    g_cursorAnimationTime += g_elapsedTime;
    if (g_cursorAnimationTime > CURSOR_ANIMATION_INTERVAL)
    {
        if (g_currentCursorFrame == OPTIONS_CURSOR_1)
        {
            g_currentCursorFrame = OPTIONS_CURSOR_0;
        }
        else
        {
            g_currentCursorFrame = OPTIONS_CURSOR_1;
        }

        g_cursorAnimationTime = 0.0f;
    }
}

VOID DrawSoundScreen(PSCREEN_SPRITE sprites)
{
    RECT ssRect = {0,0,1,1};
    RECT dest = { 0, 0, 64, 64 };
    BYTE i = 0;
    W3DClear(41);
    CreateScreenSprites(sprites);
    CreateScreenSubSprite(sprites, SOUND_OPTIONS_MENUBACKGROUND, &ssRect);

    dest.left = 40;
    dest.top = 17;
    dest.right  = dest.left + 250;
    dest.bottom = dest.top + 46;
    W3DDrawSprite2D(sprites[SOUND_OPTIONS_MENUBACKGROUND].sprite, &dest);

    dest.left = 40;
    dest.top = 82;
    dest.right  = dest.left + 250;
    dest.bottom = dest.top + 46;
    W3DDrawSprite2D(sprites[SOUND_OPTIONS_MENUBACKGROUND].sprite, &dest);

    dest.left = 40;
    dest.top = 147;
    dest.right  = dest.left + 250;
    dest.bottom = dest.top + 33;
    W3DDrawSprite2D(sprites[SOUND_OPTIONS_MENUBACKGROUND].sprite, &dest);

    UpdateCursorAnimationFrame();

    dest.top    = g_soundMenu[g_currentSoundMenuItem].y;
    dest.left   = 45;
    dest.right  = dest.left + W3DGetSpriteWidth(sprites[OPTIONS_CURSOR_1].sprite);
    dest.bottom = dest.top + W3DGetSpriteHeight(sprites[OPTIONS_CURSOR_1].sprite);
    
    W3DDrawSprite2D(sprites[g_currentCursorFrame].sprite, &dest);

    dest.top = 0;
    dest.left = 96;
    dest.right  = dest.left + W3DGetSpriteWidth(sprites[SOUND_OPTION_SOUNDEFFECTS_LABEL].sprite);
    dest.bottom = dest.top + W3DGetSpriteHeight(sprites[SOUND_OPTION_SOUNDEFFECTS_LABEL].sprite);

    W3DDrawSprite2D(sprites[SOUND_OPTION_SOUNDEFFECTS_LABEL].sprite, &dest);

    dest.top = 67;
    dest.left = 96;
    dest.right  = dest.left + W3DGetSpriteWidth(sprites[SOUND_OPTION_SOUNDEFFECTS_LABEL].sprite);
    dest.bottom = dest.top + W3DGetSpriteHeight(sprites[SOUND_OPTION_SOUNDEFFECTS_LABEL].sprite);

    W3DDrawSprite2D(sprites[SOUND_OPTION_DIGITIZED_LABEL].sprite, &dest);

    dest.top = 132;
    dest.left = 96;
    dest.right  = dest.left + W3DGetSpriteWidth(sprites[SOUND_OPTION_SOUNDEFFECTS_LABEL].sprite);
    dest.bottom = dest.top + W3DGetSpriteHeight(sprites[SOUND_OPTION_SOUNDEFFECTS_LABEL].sprite);

    W3DDrawSprite2D(sprites[SOUND_OPTION_MUSIC_LABEL].sprite, &dest);

    dest.top = W3DGetScreenHeight() - W3DGetSpriteHeight(sprites[SOUND_OPTION_ESC_BACK].sprite);
    dest.left = W3DGetScreenWidth()/2 - W3DGetSpriteWidth(sprites[SOUND_OPTION_ESC_BACK].sprite)/2;
    dest.right  = dest.left + W3DGetSpriteWidth(sprites[SOUND_OPTION_ESC_BACK].sprite);
    dest.bottom = dest.top + W3DGetSpriteHeight(sprites[SOUND_OPTION_ESC_BACK].sprite);

    W3DDrawSprite2D(sprites[SOUND_OPTION_ESC_BACK].sprite, &dest);

    // Draw menu text
    for(i = 0; i < ARRAYSIZE(g_soundMenu);i++)
    {
        DrawString(sprites, 100, g_soundMenu[i].y, (i == g_currentSoundMenuItem) ? COLOR_WHITE:COLOR_GRAY, g_soundMenu[i].name);
        dest.left   = 72;
        dest.top    = g_soundMenu[i].y + 2;
        dest.right  = dest.left + W3DGetSpriteWidth(sprites[SOUND_OPTION_SELECTED].sprite);
        dest.bottom = dest.top + W3DGetSpriteHeight(sprites[SOUND_OPTION_SELECTED].sprite);
        switch(i)
        {
        case 0:
            W3DDrawSprite2D(sprites[!g_soundsEnabled ? SOUND_OPTION_SELECTED : SOUND_OPTION_UNSELECTED].sprite, &dest);
            break;
        case 1:
            W3DDrawSprite2D(sprites[g_soundsEnabled ? SOUND_OPTION_SELECTED : SOUND_OPTION_UNSELECTED].sprite, &dest);
            break;
        case 2:
            W3DDrawSprite2D(sprites[g_soundsEnabled ? SOUND_OPTION_SELECTED : SOUND_OPTION_UNSELECTED].sprite, &dest);
            break;
        case 3:
            W3DDrawSprite2D(sprites[!g_soundsEnabled ? SOUND_OPTION_SELECTED : SOUND_OPTION_UNSELECTED].sprite, &dest);
            break;
        case 4:
            W3DDrawSprite2D(sprites[g_soundsEnabled ? SOUND_OPTION_SELECTED : SOUND_OPTION_UNSELECTED].sprite, &dest);
            break;
        case 5:
            W3DDrawSprite2D(sprites[g_soundsEnabled ? SOUND_OPTION_SELECTED : SOUND_OPTION_UNSELECTED].sprite, &dest);
            break;
        case 6:
            W3DDrawSprite2D(sprites[!g_musicEnabled ? SOUND_OPTION_SELECTED : SOUND_OPTION_UNSELECTED].sprite, &dest);
            break;
        case 7:
        default:
            W3DDrawSprite2D(sprites[g_musicEnabled ? SOUND_OPTION_SELECTED : SOUND_OPTION_UNSELECTED].sprite, &dest);
            break;
        }
    }

    if (IsKeyJustPressed(VK_DOWN))
    {
        PlayScreenSound(SOUND_MENU_MOVE, TRUE);

        g_currentSoundMenuItem++;
        if (g_currentSoundMenuItem >= ARRAYSIZE(g_soundMenu))
        {
            g_currentSoundMenuItem = 0;
        }
    }

    if (IsKeyJustPressed(VK_UP))
    {
        PlayScreenSound(SOUND_MENU_MOVE, TRUE);

        g_currentSoundMenuItem--;
        if (g_currentSoundMenuItem < 0)
        {
            g_currentSoundMenuItem = ARRAYSIZE(g_soundMenu) - 1;
        }
    }

    if (IsKeyJustPressed(VK_RETURN))
    {
        PlayScreenSound(SOUND_MENU_SELECT, TRUE);

        if (g_soundMenu[g_currentSoundMenuItem].menuItemFunc != NULL)
        {
            g_soundMenu[g_currentSoundMenuItem].menuItemFunc();
        }
    }

    if (IsKeyJustPressed(VK_ESCAPE))
    {
        ChangeScreen(OPTIONS_SCREEN);
    }
}

VOID DrawOptionsScreen(PSCREEN_SPRITE sprites) 
{
    RECT dest = { 0, 0, 64, 64 };
    RECT ssRect = {0,0,1,1};
    BYTE i = 0;
    HSPRITE subSprite = INVALID_HSPRITE_VALUE;
    W3DClear(41);
    CreateFontSprites();
    CreateScreenSprites(sprites);
    CreateScreenSubSprite(sprites, OPTIONS_MENUBACKGROUND, &ssRect);

    ssRect.left = 0;
    ssRect.top = 0;
    ssRect.right = 7;
    ssRect.bottom = 48;

    CreateScreenSubSprite(sprites, OPTIONS_TOPBORDER, &ssRect);

    dest.top    = 0;
    dest.left   = W3DGetScreenWidth()/2 - W3DGetSpriteWidth(sprites[OPTIONS_BITMAP].sprite)/2;
    dest.right  = dest.left + W3DGetSpriteWidth(sprites[OPTIONS_BITMAP].sprite);
    dest.bottom = dest.top + W3DGetSpriteHeight(sprites[OPTIONS_BITMAP].sprite);
    
    W3DDrawSprite2D(sprites[OPTIONS_BITMAP].sprite, &dest);

    dest.top    = 0;
    dest.left   = 0;
    dest.right  = dest.left + 86;
    dest.bottom = dest.top + W3DGetSpriteHeight(sprites[OPTIONS_TOPBORDER].sprite);
    
    W3DDrawSprite2D(sprites[OPTIONS_TOPBORDER].sprite, &dest);

    dest.top    = 0;
    dest.left   = 236;
    dest.right  = dest.left + 86;
    dest.bottom = dest.top + W3DGetSpriteHeight(sprites[OPTIONS_TOPBORDER].sprite);

    W3DDrawSprite2D(sprites[OPTIONS_TOPBORDER].sprite, &dest);

    dest.top = W3DGetScreenHeight() - W3DGetSpriteHeight(sprites[OPTIONS_ESC_BACK].sprite);
    dest.left = W3DGetScreenWidth()/2 - W3DGetSpriteWidth(sprites[OPTIONS_ESC_BACK].sprite)/2;
    dest.right  = dest.left + W3DGetSpriteWidth(sprites[OPTIONS_ESC_BACK].sprite);
    dest.bottom = dest.top + W3DGetSpriteHeight(sprites[OPTIONS_ESC_BACK].sprite);

    W3DDrawSprite2D(sprites[OPTIONS_ESC_BACK].sprite, &dest);

    dest.top = 52;
    dest.left = 68;
    dest.right  = dest.left + 179;
    dest.bottom = dest.top + 137;

    W3DDrawSprite2D(sprites[OPTIONS_MENUBACKGROUND].sprite, &dest);

    UpdateCursorAnimationFrame();

    dest.top    = g_optionsMenu[g_currentOptionsMenuItem].y;
    dest.left   = 72;
    dest.right  = dest.left + W3DGetSpriteWidth(sprites[OPTIONS_CURSOR_1].sprite);
    dest.bottom = dest.top + W3DGetSpriteHeight(sprites[OPTIONS_CURSOR_1].sprite);
    
    W3DDrawSprite2D(sprites[g_currentCursorFrame].sprite, &dest);

    // Draw menu text
    for(i = 0; i < ARRAYSIZE(g_optionsMenu);i++)
    {
        if (!g_gameInProgress && i == 2)
        {

        }
        else
        {
            if (i == 2)
            {
                DrawString(sprites, 100, g_optionsMenu[i].y, (i == g_currentOptionsMenuItem) ? COLOR_YELLOW:COLOR_HI_YELLOW, g_optionsMenu[i].name);
            }
            else
            {
                DrawString(sprites, 100, g_optionsMenu[i].y, (i == g_currentOptionsMenuItem) ? COLOR_WHITE:COLOR_GRAY, g_optionsMenu[i].name);
            }
        }
    }

    if (IsKeyJustPressed(VK_DOWN))
    {
        PlayScreenSound(SOUND_MENU_MOVE, TRUE);

        g_currentOptionsMenuItem++;
        if (!g_gameInProgress && g_currentOptionsMenuItem == 2) g_currentOptionsMenuItem++;

        if (g_currentOptionsMenuItem >= ARRAYSIZE(g_optionsMenu))
        {
            g_currentOptionsMenuItem = 0;
        }
    }

    if (IsKeyJustPressed(VK_UP))
    {
        PlayScreenSound(SOUND_MENU_MOVE, TRUE);

        g_currentOptionsMenuItem--;
        if (!g_gameInProgress && g_currentOptionsMenuItem == 2) g_currentOptionsMenuItem--;
        if (g_currentOptionsMenuItem < 0)
        {
            g_currentOptionsMenuItem = ARRAYSIZE(g_optionsMenu) - 1;
        }
    }

    if (IsKeyJustPressed(VK_RETURN))
    {
        PlayScreenSound(SOUND_MENU_SELECT, TRUE);

        if (g_optionsMenu[g_currentOptionsMenuItem].menuItemFunc != NULL)
        {
            g_optionsMenu[g_currentOptionsMenuItem].menuItemFunc();
        }
    }
}

VOID MenuItem_BackToGame()
{
    ChangeScreen(GAMEHUD_SCREEN);
}

VOID MenuItem_NewGame()
{
    g_gameInProgress = FALSE;
    ChangeScreen(EPISODE_SCREEN);
}

VOID MenuItem_Sound()
{
    ChangeScreen(SOUND_SCREEN);
}

VOID MenuItem_ExitGame()
{
    /* TODO: Add "are you sure"? prompt */
    PostQuitMessage(0);
}

VOID MenuItem_ToggleSoundOnOff()
{
    g_soundsEnabled = !g_soundsEnabled;
}

VOID MenuItem_ToggleMusicOnOff()
{
    g_musicEnabled = !g_musicEnabled;
}

VOID DrawEpisodeScreen(PSCREEN_SPRITE sprites)
{
    RECT dest = { 6, 19, 313, 180 };
    RECT ssRect = { 0,0,1,1};
    BYTE i = 0;
    W3DClear(41);
    CreateFontSprites();
    CreateScreenSprites(sprites);
    CreateScreenSubSprite(sprites, EPISODES_MENUBACKGROUND, &ssRect);

    W3DDrawSprite2D(sprites[EPISODES_MENUBACKGROUND].sprite, &dest);

    UpdateCursorAnimationFrame();

    dest.top    = g_episodesMenu[g_currentEpisodeMenuItem].y;
    dest.left   = 9;
    dest.right  = dest.left + W3DGetSpriteWidth(sprites[OPTIONS_CURSOR_1].sprite);
    dest.bottom = dest.top + W3DGetSpriteHeight(sprites[OPTIONS_CURSOR_1].sprite);
    
    W3DDrawSprite2D(sprites[g_currentCursorFrame].sprite, &dest);

    dest.top = W3DGetScreenHeight() - W3DGetSpriteHeight(sprites[EPISODES_ESC_BACK].sprite);
    dest.left = W3DGetScreenWidth()/2 - W3DGetSpriteWidth(sprites[EPISODES_ESC_BACK].sprite)/2;
    dest.right  = dest.left + W3DGetSpriteWidth(sprites[EPISODES_ESC_BACK].sprite);
    dest.bottom = dest.top + W3DGetSpriteHeight(sprites[EPISODES_ESC_BACK].sprite);

    W3DDrawSprite2D(sprites[EPISODES_ESC_BACK].sprite, &dest);

    DrawString(sprites, 52, 2, COLOR_YELLOW, "Which episode to play?");

    for(i = 0; i < ARRAYSIZE(g_episodesMenu);i++)
    {
        DrawString(sprites, 97, g_episodesMenu[i].y, (i == g_currentEpisodeMenuItem)? COLOR_WHITE:COLOR_GREEN, g_episodesMenu[i].name);
        dest.left = 40;
        dest.top = g_episodesMenu[i].y;
        dest.right  = dest.left + W3DGetSpriteWidth(sprites[i + EPISODES_1].sprite);
        dest.bottom = dest.top + W3DGetSpriteHeight(sprites[i + EPISODES_1].sprite);
        W3DDrawSprite2D(sprites[i + EPISODES_1].sprite, &dest);
    }

    if (IsKeyJustPressed(VK_DOWN))
    {
        PlayScreenSound(SOUND_MENU_MOVE, TRUE);
#ifdef WOLF3D_FULL_VERSION
        g_currentEpisodeMenuItem++;
        if (g_currentEpisodeMenuItem >= ARRAYSIZE(g_episodesMenu))
        {
            g_currentEpisodeMenuItem = 0;
        }
#endif
    }

    if (IsKeyJustPressed(VK_UP))
    {
        PlayScreenSound(SOUND_MENU_MOVE, TRUE);
#ifdef WOLF3D_FULL_VERSION
        g_currentEpisodeMenuItem--;
        if (g_currentEpisodeMenuItem < 0)
        {
            g_currentEpisodeMenuItem = ARRAYSIZE(g_episodesMenu) - 1;
        }
#endif
    }

    if (IsKeyJustPressed(VK_RETURN))
    {
        PlayScreenSound(SOUND_MENU_SELECT, TRUE);

        if (g_episodesMenu[g_currentEpisodeMenuItem].menuItemFunc != NULL)
        {
            g_episodesMenu[g_currentEpisodeMenuItem].menuItemFunc();
        }

        ChangeScreen(DIFFICULTY_SCREEN);
    }

    if (IsKeyJustPressed(VK_ESCAPE))
    {
        ChangeScreen(OPTIONS_SCREEN);
    }
}

VOID DrawDifficultyScreen(PSCREEN_SPRITE sprites)
{
    RECT dest = { 45, 90, 272, 157 };
    RECT ssRect = { 0,0,1,1};
    BYTE i = 0;
    W3DClear(41);
    CreateFontSprites();
    CreateScreenSprites(sprites);
    CreateScreenSubSprite(sprites, DIFFICULTY_MENUBACKGROUND, &ssRect);
    
    UpdateCursorAnimationFrame();

    W3DDrawSprite2D(sprites[DIFFICULTY_MENUBACKGROUND].sprite, &dest);

    dest.top = W3DGetScreenHeight() - W3DGetSpriteHeight(sprites[DIFFICULTY_ESC_BACK].sprite);
    dest.left = W3DGetScreenWidth()/2 - W3DGetSpriteWidth(sprites[DIFFICULTY_ESC_BACK].sprite)/2;
    dest.right  = dest.left + W3DGetSpriteWidth(sprites[DIFFICULTY_ESC_BACK].sprite);
    dest.bottom = dest.top + W3DGetSpriteHeight(sprites[DIFFICULTY_ESC_BACK].sprite);

    W3DDrawSprite2D(sprites[DIFFICULTY_ESC_BACK].sprite, &dest);

    DrawString(sprites, 69, 69, COLOR_YELLOW, "How tough are you?");

    for(i = 0; i < ARRAYSIZE(g_difficultyMenu);i++)
    {
        DrawString(sprites, 73, g_difficultyMenu[i].y, (i == g_currentDifficultyMenuItem) ? COLOR_WHITE:COLOR_GRAY, g_difficultyMenu[i].name);
    }

    UpdateCursorAnimationFrame();

    dest.top    = g_difficultyMenu[g_currentDifficultyMenuItem].y;
    dest.left   = 48;
    dest.right  = dest.left + W3DGetSpriteWidth(sprites[DIFFICULTY_CURSOR_1].sprite);
    dest.bottom = dest.top + W3DGetSpriteHeight(sprites[DIFFICULTY_CURSOR_1].sprite);
    
    W3DDrawSprite2D(sprites[g_currentCursorFrame].sprite, &dest);

    dest.top = 106;
    dest.left = 232;
    dest.right  = dest.left + W3DGetSpriteWidth(sprites[DIFFICULTY_EASYPIC].sprite);
    dest.bottom = dest.top + W3DGetSpriteHeight(sprites[DIFFICULTY_EASYPIC].sprite);

    W3DDrawSprite2D(sprites[g_currentDifficultyMenuItem + DIFFICULTY_EASYPIC].sprite, &dest);

    if (IsKeyJustPressed(VK_DOWN))
    {
        PlayScreenSound(SOUND_MENU_MOVE, TRUE);

        g_currentDifficultyMenuItem++;
        if (g_currentDifficultyMenuItem >= ARRAYSIZE(g_difficultyMenu))
        {
            g_currentDifficultyMenuItem = 0;
        }
    }

    if (IsKeyJustPressed(VK_UP))
    {
        PlayScreenSound(SOUND_MENU_MOVE, TRUE);

        g_currentDifficultyMenuItem--;
        if (g_currentDifficultyMenuItem < 0)
        {
            g_currentDifficultyMenuItem = ARRAYSIZE(g_difficultyMenu) - 1;
        }
    }

    if (IsKeyJustPressed(VK_RETURN))
    {
        PlayScreenSound(SOUND_MENU_SELECT, TRUE);

        if (g_difficultyMenu[g_currentDifficultyMenuItem].menuItemFunc != NULL)
        {
            g_difficultyMenu[g_currentDifficultyMenuItem].menuItemFunc();
        }

        ChangeScreen(GETPSYCHED_SCREEN);
    }

    if (IsKeyJustPressed(VK_ESCAPE))
    {
        ChangeScreen(EPISODE_SCREEN);
    }
}

VOID DrawLevelDebugScreen(PSCREEN_SPRITE sprites)
{
    RECT ssRect = {0,0,1,1};
    RECT block = {0,0,2,2};
    BYTE blockSize = 2;
    char ch;
    USHORT drawOffset = 0;
    BYTE levelNumber = g_player.Level+1;
    BYTE x = 0;
    BYTE y = 0;
    BYTE blockColor = COLOR_WHITE;
    INT i = 0;
    char numBuff[64];
    BYTE totalGuards = 0;
    BYTE totalSS = 0;
    BYTE totalDogs = 0;
    BYTE totalZombies = 0;
    BYTE totalOfficers = 0;
    USHORT statsOffset = 64 * blockSize + 1;

    ZeroMemory(numBuff, sizeof(numBuff));
    g_debugLastlevelNumber = levelNumber;

    CreateFontSprites();
    CreateScreenSprites(sprites);
    CreateScreenSubSprite(sprites, DEBUG_BLOCK, &ssRect);

    if (!g_debugLevelLegendMode)
    {
        DrawString(sprites, 0,0,COLOR_WHITE, "F2-show level stats");
    }
    else
    {
        DrawString(sprites, 0,0,COLOR_WHITE, "F2-hide level stats");
    }

    drawOffset += 15;

    // Draw walls
    for (x = 0; x < 64; x++)
    {
        for (y = 0; y < 64; y++)
        {
            block.left   = y * blockSize;
            block.top    = (x * blockSize) + drawOffset;
            block.right  = block.left + blockSize;
            block.bottom = block.top + blockSize;

            blockColor = 0;
            if (g_map[i].Type != CELL_TYPE_EMPTY && g_map[i].Type != CELL_TYPE_WE_DOOR && g_map[i].Type != CELL_TYPE_NS_DOOR )
            {
                blockColor = COLOR_GRAY;
                switch(g_map[i].Type)
                {
                case CELL_TYPE_LEVEL_EXIT:
                    blockColor = COLOR_RED;
                    break;
                default:
                    break;
                }
            }
            else
            {
                if (g_map[i].DoorInfo.State == DOOR_STATE_CLOSED)
                {
                    switch(g_map[i].Type)
                    {
                    case CELL_TYPE_WE_DOOR:
                    case CELL_TYPE_NS_DOOR:
                        blockColor = COLOR_LIGHT_BLUE;
                        break;
                    default:
                        break;
                    }
                }
            }
            
            W3DDrawSprite2DSingleColor(sprites[DEBUG_BLOCK].sprite, blockColor, &block);

            if (g_debugLevelPatrolMarkersMode)
            {
                if (g_map[i].HasPatrolDir)
                {
                    blockColor = COLOR_YELLOW;
                    W3DDrawSprite2DSingleColor(sprites[DEBUG_BLOCK].sprite, blockColor, &block);
                }
            }

            i++;
        }
    }

    // draw player's position
    block.left   = (g_player.Position.x * blockSize) - blockSize/2;
    block.top    = (g_player.Position.y * blockSize) -(blockSize/2) + drawOffset;
    block.right  = block.left + blockSize;
    block.bottom = block.top + blockSize;
    
    W3DDrawSprite2DSingleColor(sprites[DEBUG_BLOCK].sprite, COLOR_GREEN, &block);

    // draw enemies positions
    for (i = 0; i < MAX_ENEMIES;i++)
    {
        block.left   = (g_enemies[i].Position.x * blockSize) - blockSize/2;
        block.top    = (g_enemies[i].Position.y * blockSize) - (blockSize/2) + drawOffset;
        block.right  = block.left + blockSize;
        block.bottom = block.top + blockSize;

        switch (g_enemies[i].Type)
        {
        case ENEMY_TYPE_SOLDIER:
            if (g_enemies[i].CurrentState != ENEMY_STATE_DEAD) totalGuards++;
            blockColor = COLOR_ORANGE;
            break;
        case ENEMY_TYPE_DOG:
            if (g_enemies[i].CurrentState != ENEMY_STATE_DEAD) totalDogs++;
            blockColor = COLOR_BROWN;
            break;
        case ENEMY_TYPE_BLUEDUDE:
            if (g_enemies[i].CurrentState != ENEMY_STATE_DEAD) totalSS++;
            blockColor = COLOR_BLUE;
            break;
        case ENEMY_TYPE_ZOMBIE:
            if (g_enemies[i].CurrentState != ENEMY_STATE_DEAD) totalZombies++;
            blockColor = COLOR_PURPLE;
            break;
        case ENEMY_TYPE_OFFICER:
            if (g_enemies[i].CurrentState != ENEMY_STATE_DEAD) totalOfficers++;
            blockColor = COLOR_BRIGHT_WHITE;
            break;
        default:
            break;
        }

        if (g_enemies[i].CurrentState != ENEMY_STATE_DEAD && g_enemies[i].Type != ENEMY_TYPE_INVALID)
        {
            W3DDrawSprite2DSingleColor(sprites[DEBUG_BLOCK].sprite, blockColor, &block);
        }
    }

    if (g_debugLevelLegendMode)
    {
        DrawString(sprites, statsOffset, drawOffset, COLOR_ORANGE, "Guards: ");
        itoa(totalGuards, numBuff, 10); DrawString(sprites, statsOffset + 80, drawOffset, COLOR_ORANGE, numBuff);
        DrawString(sprites, statsOffset, drawOffset + 13, COLOR_BLUE,  "SS: ");
        itoa(totalSS, numBuff, 10); DrawString(sprites, statsOffset + 80, drawOffset + 13, COLOR_BLUE, numBuff);
        DrawString(sprites, statsOffset, drawOffset + 26, COLOR_BROWN, "Dogs: ");
        itoa(totalDogs, numBuff, 10); DrawString(sprites, statsOffset + 80, drawOffset + 26, COLOR_BROWN, numBuff);
        DrawString(sprites, statsOffset, drawOffset + 39, COLOR_PURPLE, "Zombies: ");
        itoa(totalZombies, numBuff, 10); DrawString(sprites, statsOffset + 80, drawOffset + 39, COLOR_PURPLE, numBuff);
        DrawString(sprites, statsOffset, drawOffset + 52, COLOR_BRIGHT_WHITE, "Officers: ");
        itoa(totalOfficers, numBuff, 10); DrawString(sprites, statsOffset + 80, drawOffset + 52, COLOR_BRIGHT_WHITE, numBuff);

        /*
        DrawString(sprites, 0,0,COLOR_YELLOW, "cell ");
        itoa(g_player.Position.x, numBuff, 10); DrawString(sprites, 30,0, COLOR_YELLOW, numBuff);
        DrawString(sprites, 50,0,COLOR_YELLOW, ",");
        itoa(g_player.Position.y, numBuff, 10); DrawString(sprites, 55,0, COLOR_YELLOW, numBuff);
        */
    }

    if (IsKeyJustPressed(VK_F2))
    {
        g_debugLevelLegendMode = !g_debugLevelLegendMode;
    }

    if (IsKeyJustPressed(VK_F3))
    {
        g_debugLevelPatrolMarkersMode = !g_debugLevelPatrolMarkersMode;
    }
}

VOID DrawLevelDebugScreenRawData(PSCREEN_SPRITE sprites)
{
    RECT ssRect = {0,0,1,1};
    RECT block = {0,0,2,2};
    BYTE blockSize = 2;
    char ch;
    USHORT drawOffset = 0;
    BOOL loadNewData = (g_debugLastlevelNumber != g_player.Level+1);
    BYTE levelNumber = g_player.Level+1;
    BYTE x = 0;
    BYTE y = 0;
    BYTE blockColor = COLOR_WHITE;
    INT wallId = 0;
    INT i = 0;

    g_debugLastlevelNumber = levelNumber;

    CreateFontSprites();
    CreateScreenSprites(sprites);
    CreateScreenSubSprite(sprites, DEBUG_BLOCK, &ssRect);

    // only load layer data if not already loaded
    if (loadNewData)
    {
        if (g_debugLayer1Data) free(g_debugLayer1Data);
        if (g_debugLayer2Data) free(g_debugLayer2Data);
        GetLayerDataEx(1, levelNumber, &g_debugLayer1Data);
        GetLayerDataEx(2, levelNumber, &g_debugLayer2Data);
    }

    W3DClear(0);

    DrawString(sprites, 0,0,COLOR_WHITE, "Level ");
    if (levelNumber < 10)
    {
        ch = levelNumber + 48;
        DrawString(sprites, 50,0,COLOR_WHITE, &ch);
    }
    else
    {
        DrawString(sprites, 50,0,COLOR_WHITE, "10");
    }

    drawOffset += 15;

    // Draw walls
    for (x = 0; x < 64; x++)
    {
        for (y = 0; y < 64; y++)
        {
            block.left   = y * blockSize;
            block.top    = (x * blockSize) + drawOffset;
            block.right  = block.left + blockSize;
            block.bottom = block.top + blockSize;

            wallId = (g_debugLayer1Data[i] - 1) * 2 + 1;
            if (wallId != 0)
            {
                // avoid overflow
                if (wallId < MAX_WALL_SPRITES)
                {
                    blockColor = COLOR_WHITE;
                    if (wallId == WALL_ELEVATOR_SWITCH_DOWN)
                    {
                        blockColor = COLOR_RED;
                    }
                }
                else
                {
                    blockColor = 0;
                }
            }

            if((g_debugLayer1Data[i] >= WALL_DOOR_NS && g_debugLayer1Data[i] <= WALL_DOOR_SILVERKEY_WE) || g_debugLayer1Data[i] == WALL_DOOR_ELEVATOR_NS || g_debugLayer1Data[i] == WALL_DOOR_ELEVATOR_WE )
            {
                blockColor = COLOR_LIGHT_BLUE;
            }

            W3DDrawSprite2DSingleColor(sprites[DEBUG_BLOCK].sprite, blockColor, &block);

            i++;
        }
    }

    // Draw entites
    i = 0;
    for (x = 0; x < 64; x++)
    {
        for (y = 0; y < 64; y++)
        {
            block.left   = y * blockSize;
            block.top    = (x * blockSize) + drawOffset;
            block.right  = block.left + blockSize;
            block.bottom = block.top + blockSize;

            blockColor = 0;

            switch(g_debugLayer2Data[i])
            {
                case PLAYER_START_N:
                case PLAYER_START_E:
                case PLAYER_START_S:
                case PLAYER_START_W:
                    blockColor = COLOR_GREEN;
                    break;
                case ENEMY_GUARD_DEAD:
                    break;
                case EASY_STANDING_GUARD_N:
                case MEDIUM_STANDING_GUARD_N:
                case HARD_STANDING_GUARD_N:
                case EASY_PATROL_GUARD_N:
                case MEDIUM_PATROL_GUARD_N:
                case HARD_PATROL_GUARD_N:
                case EASY_STANDING_GUARD_E:
                case MEDIUM_STANDING_GUARD_E:
                case HARD_STANDING_GUARD_E:
                case EASY_PATROL_GUARD_E:
                case MEDIUM_PATROL_GUARD_E:
                case HARD_PATROL_GUARD_E:
                case EASY_STANDING_GUARD_S:
                case MEDIUM_STANDING_GUARD_S:
                case HARD_STANDING_GUARD_S:
                case EASY_PATROL_GUARD_S:
                case MEDIUM_PATROL_GUARD_S:
                case HARD_PATROL_GUARD_S:
                case EASY_STANDING_GUARD_W:
                case MEDIUM_STANDING_GUARD_W:
                case HARD_STANDING_GUARD_W:
                case EASY_PATROL_GUARD_W:
                case MEDIUM_PATROL_GUARD_W:
                case HARD_PATROL_GUARD_W:
                    blockColor = COLOR_LIGHTBROWN;
                    break;
                case EASY_STANDING_SS_N:
                case MEDIUM_STANDING_SS_N:
                case HARD_STANDING_SS_N:
                case EASY_PATROL_SS_N:
                case MEDIUM_PATROL_SS_N:
                case HARD_PATROL_SS_N:
                case EASY_STANDING_SS_E:
                case MEDIUM_STANDING_SS_E:
                case HARD_STANDING_SS_E:
                case EASY_PATROL_SS_E:
                case MEDIUM_PATROL_SS_E:
                case HARD_PATROL_SS_E:
                case EASY_STANDING_SS_S:
                case MEDIUM_STANDING_SS_S:
                case HARD_STANDING_SS_S:
                case EASY_PATROL_SS_S:
                case MEDIUM_PATROL_SS_S:
                case HARD_PATROL_SS_S:
                case EASY_STANDING_SS_W:
                case MEDIUM_STANDING_SS_W:
                case HARD_STANDING_SS_W:
                case EASY_PATROL_SS_W:
                case MEDIUM_PATROL_SS_W:
                case HARD_PATROL_SS_W:
                    blockColor = COLOR_BLUE;
                    break;
                case EASY_STANDING_DOG_N:
                case MEDIUM_STANDING_DOG_N:
                case HARD_STANDING_DOG_N:
                case EASY_PATROL_DOG_N:
                case MEDIUM_PATROL_DOG_N:
                case HARD_PATROL_DOG_N:
                case EASY_STANDING_DOG_E:
                case MEDIUM_STANDING_DOG_E:
                case HARD_STANDING_DOG_E:
                case EASY_PATROL_DOG_E:
                case MEDIUM_PATROL_DOG_E:
                case HARD_PATROL_DOG_E:
                case EASY_STANDING_DOG_S:
                case MEDIUM_STANDING_DOG_S:
                case HARD_STANDING_DOG_S:
                case EASY_PATROL_DOG_S:
                case MEDIUM_PATROL_DOG_S:
                case HARD_PATROL_DOG_S:
                case EASY_STANDING_DOG_W:
                case MEDIUM_STANDING_DOG_W:
                case HARD_STANDING_DOG_W:
                case EASY_PATROL_DOG_W:
                case MEDIUM_PATROL_DOG_W:
                case HARD_PATROL_DOG_W:
                    blockColor = COLOR_BROWN;
                    break;
                default:
                    break;
            }
            
            if (blockColor != 0)
            {
                W3DDrawSprite2DSingleColor(sprites[DEBUG_BLOCK].sprite, blockColor, &block);
            }
            i++;
        }
    }

    if (IsKeyJustPressed(VK_DOWN) || IsKeyJustPressed(VK_RIGHT))
    {
        g_player.Level++;
        if (g_player.Level > (MAX_LEVELS - 1))
        {
            g_player.Level = 0;
        }
    }

    if (IsKeyJustPressed(VK_UP) || IsKeyJustPressed(VK_LEFT))
    {
        if (g_player.Level == 0)
        {
            g_player.Level = MAX_LEVELS - 1;
        }
        else
        {
            g_player.Level--;
        }
    }
}

VOID DrawWelcomeStatsScreen(PSCREEN_SPRITE sprites)
{
    RECT ssRect = {1,0,172,40};
    RECT dest;
    W3DClear(41);
    
    CreateFontSprites();
    CreateScreenSprites(sprites);

    CreateScreenSubSprite(sprites, WELCOMESTATS_BITMAP, &ssRect);

#ifndef WOLF3D_FULL_VERSION
    dest.top    = 15;
    dest.left   = W3DGetScreenWidth()/2 - W3DGetSpriteWidth(sprites[WELCOMESTATS_BITMAP].sprite)/2;
    dest.right  = dest.left + W3DGetSpriteWidth(sprites[WELCOMESTATS_BITMAP].sprite);
    dest.bottom = dest.top + W3DGetSpriteHeight(sprites[WELCOMESTATS_BITMAP].sprite);
    
    W3DDrawSprite2D(sprites[WELCOMESTATS_BITMAP].sprite, &dest);
#endif

    ssRect.left = 0;
    ssRect.top = 0;
    ssRect.right = 7;
    ssRect.bottom = 48;

    CreateScreenSubSprite(sprites, WELCOMESTATS_TOPBORDER, &ssRect);

    dest.top    = 15;
    dest.left   = 0;
    dest.right  = dest.left + 75;
    dest.bottom = dest.top + W3DGetSpriteHeight(sprites[WELCOMESTATS_TOPBORDER].sprite);
    
    W3DDrawSprite2D(sprites[WELCOMESTATS_TOPBORDER].sprite, &dest);

    dest.top    = 15;
    dest.left   = 245;
    dest.right  = dest.left + 75;
    dest.bottom = dest.top + W3DGetSpriteHeight(sprites[WELCOMESTATS_TOPBORDER].sprite);

    W3DDrawSprite2D(sprites[WELCOMESTATS_TOPBORDER].sprite, &dest);

#ifdef WOLF3D_FULL_VERSION
    DrawString(sprites, 95, 30, COLOR_WHITE, "Wolfenstein 3D\n All 60 Levels");
#endif

    if (IsKeyJustPressed(VK_RETURN) || IsKeyJustPressed(VK_SPACE))
    {
        ChangeScreen(RATING_SCREEN);
    }

    if(!g_hackSoundPlayed)
    {
        PlayScreenSound(SOUND_TITLE_SCREEN, TRUE);
        g_hackSoundPlayed = TRUE;
    }
}

VOID  DrawGameHud(PSCREEN_SPRITE sprites)
{
    BYTE digits[6];
    RECT dest = { 0, 0, 64, 64 };
    RECT dest2 = { 0, 0, 64, 64 };
    BYTE currentWeaponAnimationFrame = 0;
    RECT vp = {0};
    BYTE weaponHeight = 80;
    BYTE weaponWidth = 80;

    CreateScreenSprites(sprites);

    g_gameInProgress = TRUE;

    g_faceAnimationTime += g_elapsedTime;

    /* HUD background */
    dest.right  = W3DGetSpriteWidth(sprites[HUD_BITMAP].sprite);
    dest.top    = W3DGetScreenHeight() - W3DGetSpriteHeight(sprites[HUD_BITMAP].sprite);
    dest.bottom = dest.top + W3DGetSpriteHeight(sprites[HUD_BITMAP].sprite);
    W3DDrawSprite2D(sprites[HUD_BITMAP].sprite, &dest);

    /* Player face */
    dest.top  = W3DGetScreenHeight() - W3DGetSpriteHeight(sprites[HUD_DUDELOOKING_AHEAD].sprite) - 4;
    dest.left = W3DGetScreenWidth()/2 - W3DGetSpriteWidth(sprites[HUD_DUDELOOKING_AHEAD].sprite);
    dest.right = dest.left + W3DGetSpriteWidth(sprites[HUD_DUDELOOKING_AHEAD].sprite);
    dest.bottom = dest.top + W3DGetSpriteHeight(sprites[HUD_DUDELOOKING_AHEAD].sprite);
    
    if (g_faceAnimationTime > FACE_ANIMATION_INTERVAL)
    {
        g_currentFaceFrame = g_faceAnimationFrames[rand() % ARRAYSIZE(g_faceAnimationFrames)];

        /* shift animation offset according to current health value */
        if (g_player.Health < 90) { g_currentFaceFrame += FACE_ANIMATION_HURT_OFFSET; }
        if (g_player.Health < 80) { g_currentFaceFrame += FACE_ANIMATION_HURT_OFFSET; }
        if (g_player.Health < 70) { g_currentFaceFrame += FACE_ANIMATION_HURT_OFFSET; }
        if (g_player.Health < 60) { g_currentFaceFrame += FACE_ANIMATION_HURT_OFFSET; }
        if (g_player.Health < 40) { g_currentFaceFrame += FACE_ANIMATION_HURT_OFFSET; }
        if (g_player.Health < 20) { g_currentFaceFrame += FACE_ANIMATION_HURT_OFFSET; }

        g_faceAnimationTime = 0.0f;
    }

    if (g_player.Health != 0)
    {
        W3DDrawSprite2D(sprites[g_currentFaceFrame].sprite, &dest);
    }
    else
    {
        /* dead face */
        W3DDrawSprite2D(sprites[HUD_DUDELOOKING_AHEAD_DEAD].sprite, &dest);
    }

    /* Current weapon */
    dest.top = W3DGetScreenHeight() - W3DGetSpriteHeight(sprites[HUD_KNIFE].sprite) - 10;
    dest.left = W3DGetScreenWidth() - W3DGetSpriteWidth(sprites[HUD_KNIFE].sprite) - 14;
    dest.right = dest.left + W3DGetSpriteWidth(sprites[HUD_KNIFE].sprite);
    dest.bottom = dest.top + W3DGetSpriteHeight(sprites[HUD_KNIFE].sprite);

    W3DGetViewport(&vp);

    weaponHeight = weaponHeight / (160.0f/(float)(vp.bottom - vp.top));
    weaponWidth = weaponWidth / (320.0f/(float)(vp.right - vp.left));

    dest2.top = vp.bottom - weaponHeight;
    dest2.left = W3DGetScreenWidth()/2 - weaponWidth/2;
    dest2.right = dest2.left + weaponWidth;
    dest2.bottom = dest2.top + weaponHeight;

    if (PlayerIsAttacking())
    {
        currentWeaponAnimationFrame = AnimationGetFrame(&g_player.GunAnimation);
    }

    switch(g_player.CurrentWeapon)
    {
        case WEAPON_KNIFE:
            W3DDrawSprite2D(sprites[HUD_KNIFE].sprite, &dest);
            if (!g_noHudGun) W3DDrawSprite2D(sprites[HUD_KNIFE_0 + currentWeaponAnimationFrame].sprite, &dest2);
            break;
        case WEAPON_PISTOL:
            W3DDrawSprite2D(sprites[HUD_PISTOL].sprite, &dest);
            if (!g_noHudGun) W3DDrawSprite2D(sprites[HUD_PISTOL_0 + currentWeaponAnimationFrame].sprite, &dest2);
            break;
        case WEAPON_MACHINEGUN:
            W3DDrawSprite2D(sprites[HUD_MACHINEGUN].sprite, &dest);
            if (!g_noHudGun) W3DDrawSprite2D(sprites[HUD_MACHINEGUN_0 + currentWeaponAnimationFrame].sprite, &dest2);
            break;
        case WEAPON_CHAINGUN:
            W3DDrawSprite2D(sprites[HUD_CHAINGUN].sprite, &dest);
            if (!g_noHudGun) W3DDrawSprite2D(sprites[HUD_CHAINGUN_0 + currentWeaponAnimationFrame].sprite, &dest2);
            break;
        default:
            break;
    }

    /* numerical stats */
    dest.top = W3DGetScreenHeight() - W3DGetSpriteHeight(sprites[HUD_0].sprite) - 8;

    /* Floor */
    NumberToDigitArray(g_player.Level % 10 + 1, digits, ARRAYSIZE(digits));
    DrawDigits(sprites, 23,dest.top,digits,ARRAYSIZE(digits));

    /* Score */
    NumberToDigitArray(g_player.Score, digits, ARRAYSIZE(digits));
    DrawDigits(sprites, 86,dest.top,digits,ARRAYSIZE(digits));

    /* Lives */
    NumberToDigitArray(g_player.Lives, digits, ARRAYSIZE(digits));
    DrawDigits(sprites, 114,dest.top,digits,ARRAYSIZE(digits));

    /* Health */
    NumberToDigitArray(g_player.Health, digits, ARRAYSIZE(digits));
    DrawDigits(sprites, 183,dest.top,digits,ARRAYSIZE(digits));

    /*  Ammo */
    NumberToDigitArray(g_player.Ammo, digits, ARRAYSIZE(digits));
    DrawDigits(sprites, 225,dest.top,digits,ARRAYSIZE(digits));

    /* Cell -- Debug*/
/*
    NumberToDigitArray((INT)g_player.Position.x, digits, ARRAYSIZE(digits));
    DrawDigits(sprites, 20,0,digits,ARRAYSIZE(digits));

    NumberToDigitArray((INT)g_player.Position.y, digits, ARRAYSIZE(digits));
    DrawDigits(sprites, 60,0,digits,ARRAYSIZE(digits));
*/    
    /* keys */

    /* top key location (gold key) */
    dest.top = W3DGetScreenHeight() - W3DGetSpriteHeight(sprites[HUD_KEYBOX].sprite) - 20;
    dest.left = W3DGetScreenWidth() - W3DGetSpriteWidth(sprites[HUD_KEYBOX].sprite) - 72;
    dest.right = dest.left + W3DGetSpriteWidth(sprites[HUD_KEYBOX].sprite);
    dest.bottom = dest.top + W3DGetSpriteHeight(sprites[HUD_KEYBOX].sprite);
    
    if (g_player.HasGoldKey)
    {
        W3DDrawSprite2D(sprites[HUD_GOLDKEY].sprite, &dest);
    }
    else
    {
        W3DDrawSprite2D(sprites[HUD_KEYBOX].sprite, &dest);
    }

    /* bottom key location (silver key) */
    dest.top = W3DGetScreenHeight() - W3DGetSpriteHeight(sprites[HUD_KEYBOX].sprite) - 4;
    dest.left = W3DGetScreenWidth() - W3DGetSpriteWidth(sprites[HUD_KEYBOX].sprite) - 72;
    dest.right = dest.left + W3DGetSpriteWidth(sprites[HUD_KEYBOX].sprite);
    dest.bottom = dest.top + W3DGetSpriteHeight(sprites[HUD_KEYBOX].sprite);
    
    if (g_player.HasSilverKey)
    {
        W3DDrawSprite2D(sprites[HUD_SILVERKEY].sprite, &dest);
    }
    else
    {
        W3DDrawSprite2D(sprites[HUD_KEYBOX].sprite, &dest);
    }

    if (IsKeyJustPressed(VK_F1))
    {
        g_drawDebugScreen = !g_drawDebugScreen;
    }

    if (g_drawDebugScreen)
    {
        DrawLevelDebugScreen(g_levelDebugSprites);
    }

    if (IsKeyJustPressed(VK_ESCAPE))
    {
        ChangeScreen(OPTIONS_SCREEN);
    }

    if (IsKeyJustPressed(VK_TAB))
    {
        if (IsKeyDown(VK_SHIFT))
        {
            StartPrevLevel();
        }
        else
        {
            StartNextLevel();
        }
    }

    // Wolfenstein 3D cheat code I-L-M
    if (IsKeyDown(VkKeyScan('i')) && IsKeyDown(VkKeyScan('l')) && IsKeyDown(VkKeyScan('m')))
    {
        PlayerSetGun(WEAPON_KNIFE); PlayerSetGun(WEAPON_PISTOL); PlayerSetGun(WEAPON_MACHINEGUN); PlayerSetGun(WEAPON_CHAINGUN);
        g_player.Health = 100;
        g_player.Ammo = 255;
        g_player.Score = 9999;
        g_player.Lives = 10;
        g_player.HasGoldKey = 1;
        g_player.HasSilverKey = 1;
    }
}

VOID DrawGetPsychedScreen(PSCREEN_SPRITE sprites)
{
    RECT dest = { 0, 0, 64, 64 };
    RECT ssRect = {0,0,1,1};
    RECT vp = {0};
    RECT block = {0,0,1,1};
    BOOL loadGame = FALSE;

    W3DClear(127);

    CreateScreenSprites(sprites);
    dest.top = W3DGetScreenHeight()/2 - W3DGetSpriteHeight(sprites[GETPSYCHED_TITLE].sprite)/2;
    dest.left = W3DGetScreenWidth()/2 - W3DGetSpriteWidth(sprites[GETPSYCHED_TITLE].sprite)/2;
    dest.right = dest.left + W3DGetSpriteWidth(sprites[GETPSYCHED_TITLE].sprite);
    dest.bottom = dest.top + W3DGetSpriteHeight(sprites[GETPSYCHED_TITLE].sprite);
    W3DDrawSprite2D(sprites[GETPSYCHED_TITLE].sprite, &dest);

    CreateScreenSubSprite(sprites, GETPSYCHED_PROGRESS_LINE, &ssRect);

    g_GetPsychedAnimationTime += g_elapsedTime;
    if (g_GetPsychedAnimationTime > PROGRESS_ANIMATION_INTERVAL)
    {
        g_percentComplete += .01;
        g_GetPsychedAnimationTime = 0;
    }

    if (g_percentComplete >= 1)
    {
        loadGame = TRUE;
    }

    block.top = dest.bottom - 3;
    block.left = dest.left + 2;
    block.right = (block.left + W3DGetSpriteWidth(sprites[GETPSYCHED_TITLE].sprite) * g_percentComplete) - 3;
    block.bottom = block.top + 1;

    W3DDrawSprite2DSingleColor(sprites[GETPSYCHED_PROGRESS_LINE].sprite, COLOR_RED, &block);

    g_noHudGun = TRUE;
    DrawGameHud(g_gameHudSprites);
    g_noHudGun = FALSE;

    if (loadGame)
    {
        g_percentComplete = 0;
        g_GetPsychedAnimationTime = 0.0f;
        PlayerInit(TRUE);
        StartNewLevel(g_player.Level);
        ChangeScreen(GAMEHUD_SCREEN);
    }
}

VOID DrawSmallString(PSCREEN_SPRITE sprites, INT x, INT y, CHAR* text)
{
    BYTE i = 0;
    RECT dest;
    RECT chRect;

    dest.left = x;
    dest.top  = y;

    for (i = 0; i < strlen(text); i++)
    {
        if (text[i] == 0)
        {
            break;
        }

        if (text[i] == '\n')
        {
            dest.left = x;
            dest.top += chRect.bottom - chRect.top;
        }
        else
        {
            RectFromCharacter(text[i], &chRect);

            dest.right = (dest.left + chRect.right - chRect.left) - 2;
            dest.bottom = (dest.top + chRect.bottom - chRect.top) - 2;

            W3DDrawSprite2D(g_fontSprites[text[i] - 32].sprite, &dest);
            dest.left += W3DGetSpriteWidth(g_fontSprites[text[i] - 32].sprite) - 1;
        }
    }
}

VOID DrawString(PSCREEN_SPRITE sprites, INT x, INT y, BYTE color, CHAR* text)
{
    BYTE i = 0;
    RECT dest;
    RECT chRect;

    dest.left = x;
    dest.top  = y;

    for (i = 0; i < strlen(text); i++)
    {
        if (text[i] == 0)
        {
            break;
        }

        if (text[i] == '\n')
        {
            dest.left = x;
            dest.top += chRect.bottom - chRect.top;
        }
        else
        {
            RectFromCharacter(text[i], &chRect);

            dest.right = dest.left + chRect.right - chRect.left;
            dest.bottom = dest.top + chRect.bottom - chRect.top;

            W3DDrawSprite2DSingleColor(g_fontSprites[text[i] - 32].sprite, color, &dest);
            dest.left += W3DGetSpriteWidth(g_fontSprites[text[i] - 32].sprite);
        }
    }
}

VOID DrawDigits(PSCREEN_SPRITE sprites, INT x, INT y, BYTE* digits, INT count)
{
    BYTE i = 0;
    RECT dest;
    dest.left = x;
    dest.top  = y;
    
    for (i = 0; i < count; i++)
    {
        if (digits[i] == DIGIT_TERMINATOR)
        {
            break;
        }

        dest.right = dest.left + W3DGetSpriteWidth(sprites[HUD_0].sprite);
        dest.bottom = dest.top + W3DGetSpriteHeight(sprites[HUD_0].sprite);
        W3DDrawSprite2D(sprites[digits[i] + HUD_0].sprite, &dest);

        dest.left -= W3DGetSpriteWidth(sprites[HUD_0].sprite);
    }
}

VOID NumberToDigitArray(INT value, BYTE* digits, INT count)
{
    INT temp = value;
    INT i = 0;
    INT totalDigits = 0;

    memset(digits, DIGIT_TERMINATOR, count);

    if (value == 0)
    {
        digits[0] = 0;
        return;
    }

    for(;temp!=0;temp=temp/10) 
    {
        totalDigits++;
    }

    if (totalDigits > count)
    {
        return;
    }

    temp = value;
    while (temp!=0)
    {
        digits[i] = temp % 10;
        temp /= 10;
        i++;
    }
}

BOOL IsGamePlayScreenActive()
{
    return (g_currentScreenId == GAMEHUD_SCREEN);
}

VOID DrawScreenOverlays()
{
    g_screens[g_currentScreenId].drawScreenFunc(g_screens[g_currentScreenId].sprites);
}

VOID PlayScreenSound(INT soundId, BOOL playNow)
{
    if ((soundId > ARRAYSIZE(g_screenSounds)))
    {
        return;
    }

    // lazy load any sounds not loaded
    if (g_screenSounds[soundId].sound == INVALID_HSOUND_VALUE)
    {
        g_screenSounds[soundId].sound = AudioCreateSFX(g_screenSounds[soundId].filePath);
    }

    // Only attempt to play sounds that successfully loaded
    if (g_screenSounds[soundId].sound != INVALID_HSOUND_VALUE)
    {
        AudioPlay(g_screenSounds[soundId].sound, g_player.Position);
    }
}

VOID PlayWeaponSound(BYTE weapon, BOOL playNow)
{
    if (weapon > 3)
    {
        return;
    }

    PlayScreenSound(SOUND_HUD_KNIFE + weapon, playNow);
}

VOID PlayEnemyAlertSound(BYTE enemyType, BOOL playNow)
{
    if (enemyType > ENEMY_TYPE_BOSS)
    {
        return;
    }

    PlayScreenSound(SOUND_SOLDIER_YELL + enemyType, playNow);
}

VOID MenuItem_CanIPlayDaddy()
{
    g_player.Difficulty = DIFFICULTY_EASY;
}

VOID MenuItem_DontHurtMe()
{
    g_player.Difficulty = DIFFICULTY_MEDIUM;
}

VOID MenuItem_BringEmOn()
{
    g_player.Difficulty = DIFFICULTY_HARD;
}

VOID MenuItem_DeathIncarnate()
{
    g_player.Difficulty = DIFFICULTY_SUPERHARD;
}

VOID MenuItem_Episode1Selected()
{
    g_player.Level = 0; // 0-9
}

VOID MenuItem_Episode2Selected()
{
    g_player.Level = 10; // 10-19
}

VOID MenuItem_Episode3Selected()
{
    g_player.Level = 20; // 20-29
}

VOID MenuItem_Episode4Selected()
{
    g_player.Level = 30; // 30-39
}

VOID MenuItem_Episode5Selected()
{
    g_player.Level = 40; // 40-49
}

VOID MenuItem_Episode6Selected()
{
    g_player.Level = 50; // 50-59
}
