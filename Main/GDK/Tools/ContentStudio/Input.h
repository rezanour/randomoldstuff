#pragma once

struct mousestate_t
{
    POINT Point;
    POINT LastPoint;
    short Current[3];
    short Previous[3];
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

class Mouse
{
    enum class MouseButton
    {
        Left,
        Middle,
        Right
    };

public:
    static void Update();
    static bool IsButtonDown(_In_ short button );
    static bool IsButtonHeld(_In_ short button );
    static bool IsButtonJustPressed(_In_ short button);
    static bool IsButtonJustReleased(_In_ short button);

    static GDK::Vector2 GetPosition();
    static GDK::Vector2 GetPositionDelta();
    static byte_t VkeyToIndex(_In_ short vkey);
};