#ifndef _SCREENS_H_
#define _SCREENS_H_

/* screen identifiers, these must remain in order */
#define SPLASH_SCREEN   0
#define RATING_SCREEN   1
#define OPTIONS_SCREEN  2
#define EPISODE_SCREEN  3
#define GAMEHUD_SCREEN  4
#define WELCOME_STATS_SCREEN 5
#define SOUND_SCREEN 6
#define DIFFICULTY_SCREEN 7
#define LEVELDEBUG_SCREEN 8
#define GETPSYCHED_SCREEN 9
#define INVALID_SCREEN  255

/* Public Screen API */
VOID InitializeScreens();
VOID ShutdownScreenSounds();

BOOL ScreenCreate(BYTE screen);
VOID ScreenDestroy(BYTE screen);

BOOL IsGamePlayScreenActive();

VOID DrawScreenOverlays();

VOID PlayWeaponSound(BYTE weapon, BOOL playNow);
VOID PlayEnemyAlertSound(BYTE enemyType, BOOL playNow);

#endif /*_SCREENS_H_ */