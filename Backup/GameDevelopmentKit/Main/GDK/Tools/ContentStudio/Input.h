#pragma once

struct mousestate_t
{
    POINT Point;
    byte_t Buttons[2];
};

struct keyboardstate_t
{
    byte_t Current[500];
    byte_t Previous[500];
};

class Keyboard
{
public:
   static void Update();
   static bool IsKeyDown(_In_ short key);
   static bool IsKeyHeld(_In_ short key);
   static bool IsKeyJustPressed(_In_ short key);
   static bool IsKeyJustReleased(_In_ short key);
   static void ClearState();
};