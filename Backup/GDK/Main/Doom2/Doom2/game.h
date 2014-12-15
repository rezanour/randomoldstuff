#ifndef _GAME_H_
#define _GAME_H_

typedef enum
{
    gamestate_splash = 0,
    gamestate_demo = 0,
    gamestate_play = 0,
    gamestate_stats = 0,
} gamestate_t;

HWND GameGetWindow();
BOOL GameSetTitle(const char* title);
void GamePause(BOOL pause);
BOOL GameGetPause();
void GameExit();
float GameGetTargetElapsedTime();
void GameSetTargetElapsedTime(float time);
gamestate_t GameGetState();
void GameSetState(gamestate_t gamestate);
BOOL GameLoadLevel(byte_t level);
byte_t GameGetLevel();
BOOL GameLoadNextLevel();
BOOL GameLoadPreviousLevel();

#endif