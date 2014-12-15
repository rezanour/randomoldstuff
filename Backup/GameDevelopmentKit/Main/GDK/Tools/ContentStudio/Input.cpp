#include "stdafx.h"

keyboardstate_t g_keyboardState;

void Keyboard::Update()
{
    memcpy(g_keyboardState.Previous, g_keyboardState.Current, sizeof(g_keyboardState.Current));
    IGNORE_RETURN GetKeyboardState(g_keyboardState.Current);
}

void Keyboard::ClearState()
{
    ZeroMemory(&g_keyboardState.Previous, sizeof(g_keyboardState.Previous));
    ZeroMemory(&g_keyboardState.Current, sizeof(g_keyboardState.Current));
}

bool Keyboard::IsKeyDown(_In_ short key )
{
    assert(key >= 0 && key < ARRAYSIZE(g_keyboardState.Current));
    return (g_keyboardState.Current[key] & 0x80) != 0;
}

bool Keyboard::IsKeyHeld(_In_ short key )
{
    assert(key >= 0 && key < ARRAYSIZE(g_keyboardState.Current));
    return ((g_keyboardState.Current[key] & g_keyboardState.Previous[key]) & 0x80) != 0;
}

bool Keyboard::IsKeyJustPressed(_In_ short key)
{
    assert(key >= 0 && key < ARRAYSIZE(g_keyboardState.Current));
    return (g_keyboardState.Current[key] & 0x80) != 0 && (g_keyboardState.Previous[key] & 0x80) == 0;
}

bool Keyboard::IsKeyJustReleased(_In_ short key)
{
    assert(key >= 0 && key < ARRAYSIZE(g_keyboardState.Current));
    return (g_keyboardState.Current[key] & 0x80) == 0 && (g_keyboardState.Previous[key] & 0x80) != 0;
}
