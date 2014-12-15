#include "common.h"

typedef struct
{
    point_t Point;
    byte_t Buttons[2];
} mousestate_t;

typedef struct
{
    byte_t Current[500];
    byte_t Previous[500];
} keyboardstate_t;

keyboardstate_t g_keyboardState;

BOOL InputInitialize()
{
    return TRUE;
}

void InputUninitialize()
{
}

void InputUpdate()
{
    memcpy(g_keyboardState.Previous, g_keyboardState.Current, sizeof(g_keyboardState.Current));
    GetKeyboardState(g_keyboardState.Current);
}

void InputClearState()
{
    ZeroMemory(&g_keyboardState.Previous, sizeof(g_keyboardState.Previous));
    ZeroMemory(&g_keyboardState.Current, sizeof(g_keyboardState.Current));
}

BOOL InputIsKeyDown(short key )
{
    assert(key >= 0 && key < ARRAYSIZE(g_keyboardState.Current));
    return (g_keyboardState.Current[key] & 0x80) != 0;
}

BOOL InputIsKeyHeld(short key )
{
    assert(key >= 0 && key < ARRAYSIZE(g_keyboardState.Current));
    return ((g_keyboardState.Current[key] & g_keyboardState.Previous[key]) & 0x80) != 0;
}

BOOL InputIsKeyJustPressed(short key)
{
    assert(key >= 0 && key < ARRAYSIZE(g_keyboardState.Current));
    return (g_keyboardState.Current[key] & 0x80) != 0 && (g_keyboardState.Previous[key] & 0x80) == 0;
}

BOOL InputIsKeyJustReleased(short key)
{
    assert(key >= 0 && key < ARRAYSIZE(g_keyboardState.Current));
    return (g_keyboardState.Current[key] & 0x80) == 0 && (g_keyboardState.Previous[key] & 0x80) != 0;
}
