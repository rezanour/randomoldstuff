#include "common.h"

KEYBOARD_STATE g_keyboardState;

VOID UpdateInput()
{
    memcpy(g_keyboardState.previous, g_keyboardState.current, sizeof(g_keyboardState.current));
    GetKeyboardState(g_keyboardState.current);
}

VOID ClearInputState()
{
    ZeroMemory(&g_keyboardState.previous, sizeof(g_keyboardState.previous));
    ZeroMemory(&g_keyboardState.current, sizeof(g_keyboardState.current));
}

BOOL IsKeyDown(BYTE key )
{
    return (g_keyboardState.current[key] & 0x80) != 0;
}

BOOL IsKeyHeld(BYTE key )
{
    return ((g_keyboardState.current[key] & g_keyboardState.previous[key]) & 0x80) != 0;
}

BOOL IsKeyJustPressed(BYTE key)
{
    return (g_keyboardState.current[key] & 0x80) != 0 && (g_keyboardState.previous[key] & 0x80) == 0;
}

BOOL IsKeyJustReleased(BYTE key)
{
    return (g_keyboardState.current[key] & 0x80) == 0 && (g_keyboardState.previous[key] & 0x80) != 0;
}