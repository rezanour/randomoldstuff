#include "stdafx.h"

keyboardstate_t g_keyboardState;
mousestate_t g_mouseState;

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

_Use_decl_annotations_
bool Keyboard::IsKeyDown(short key )
{
    assert(key >= 0 && key < ARRAYSIZE(g_keyboardState.Current));
    return (g_keyboardState.Current[key] & 0x80) != 0;
}

_Use_decl_annotations_
bool Keyboard::IsKeyHeld(short key )
{
    assert(key >= 0 && key < ARRAYSIZE(g_keyboardState.Current));
    return ((g_keyboardState.Current[key] & g_keyboardState.Previous[key]) & 0x80) != 0;
}

_Use_decl_annotations_
bool Keyboard::IsKeyJustPressed(short key)
{
    assert(key >= 0 && key < ARRAYSIZE(g_keyboardState.Current));
    return (g_keyboardState.Current[key] & 0x80) != 0 && (g_keyboardState.Previous[key] & 0x80) == 0;
}

_Use_decl_annotations_
bool Keyboard::IsKeyJustReleased(short key)
{
    assert(key >= 0 && key < ARRAYSIZE(g_keyboardState.Current));
    return (g_keyboardState.Current[key] & 0x80) == 0 && (g_keyboardState.Previous[key] & 0x80) != 0;
}

void Mouse::Update()
{
    memcpy(g_mouseState.Previous, g_mouseState.Current, sizeof(g_mouseState.Current));
    memcpy(&g_mouseState.LastPoint, &g_mouseState.Point, sizeof(g_mouseState.Point));
    g_mouseState.Current[0] = GetKeyState(VK_LBUTTON);
    g_mouseState.Current[1] = GetKeyState(VK_MBUTTON);
    g_mouseState.Current[2] = GetKeyState(VK_RBUTTON);
    GetCursorPos(&g_mouseState.Point);
}

_Use_decl_annotations_
byte_t Mouse::VkeyToIndex(short vkey)
{
    switch (vkey)
    {
    case VK_LBUTTON:
        return 0;
    case VK_MBUTTON:
        return 1;
    case VK_RBUTTON:
        return 2;
    };

    return 0;
}

_Use_decl_annotations_
bool Mouse::IsButtonDown(short button )
{
    byte_t key = VkeyToIndex(button);
    assert(key >= 0 && key < ARRAYSIZE(g_mouseState.Current));
    return (g_mouseState.Current[key] & 0x80) != 0;
}

_Use_decl_annotations_
bool Mouse::IsButtonHeld(short button )
{
    byte_t key = VkeyToIndex(button);
    assert(key >= 0 && key < ARRAYSIZE(g_mouseState.Current));
    return ((g_mouseState.Current[key] & g_mouseState.Previous[key]) & 0x80) != 0;
}

_Use_decl_annotations_
bool Mouse::IsButtonJustPressed(short button)
{
    byte_t key = VkeyToIndex(button);
    assert(key >= 0 && key < ARRAYSIZE(g_mouseState.Current));
    return (g_mouseState.Current[key] & 0x80) != 0 && (g_mouseState.Previous[key] & 0x80) == 0;
}

_Use_decl_annotations_
bool Mouse::IsButtonJustReleased(short button)
{
    byte_t key = VkeyToIndex(button);
    assert(key >= 0 && key < ARRAYSIZE(g_mouseState.Current));
    return (g_mouseState.Current[key] & 0x80) == 0 && (g_mouseState.Previous[key] & 0x80) != 0;
}

GDK::Vector2 Mouse::GetPosition()
{
    GDK::Vector2 pos;
    pos.x = (float)g_mouseState.Point.x;
    pos.y = (float)g_mouseState.Point.y;

    return pos;
}

GDK::Vector2 Mouse::GetPositionDelta()
{
    GDK::Vector2 pos;
    pos.x = (float)g_mouseState.Point.x - g_mouseState.LastPoint.x;
    pos.y = (float)g_mouseState.Point.y - g_mouseState.LastPoint.y;

    return pos;
}
