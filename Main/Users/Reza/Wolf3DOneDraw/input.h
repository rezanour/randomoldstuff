#pragma once

#include <Xinput.h>

#pragma pack(push,1)

typedef struct
{
    uint8_t KeyboardState[256];
} InputState;

typedef struct
{
    uint8_t Current[256];
    uint8_t Previous[256];
} KeyboardState;

typedef struct
{
    XINPUT_STATE Current;
    XINPUT_STATE Previous;
} GamePadState;

#pragma pack(pop)

class Input
{
public:
    void Tick();
    bool IsMenuUpPressed() const;
    bool IsMenuSelectPressed() const;
    bool IsMenuDownPressed() const;
    bool IsMenuDismissPressed() const;
    bool IsMenuButtonPressed() const;
    bool IsMenuRightPressed() const;
    bool IsMenuLeftPressed() const;

    XMFLOAT2 GetLookVector() const;
    XMFLOAT2 GetMovementVector() const;

private:
    friend void InputStartup();
    Input();
    Input(const Input&);

    bool IsKeyDown(_In_ int16_t key) const;
    bool IsKeyHeld(_In_ int16_t key) const;
    bool IsKeyJustPressed(_In_ int16_t key) const;
    bool IsKeyJustReleased(_In_ int16_t key) const;
    bool IsControllerButtonDown(_In_ int16_t button) const;
    bool IsControllerButtonHeld(_In_ int16_t button) const;
    bool IsControllerButtonJustPressed(_In_ int16_t button) const;
    bool IsControllerButtonJustReleased(_In_ int16_t button) const;
    void GetLeftThumbStickValue(_Out_ float* x, _Out_ float* y) const;
    void GetRightThumbStickValue(_Out_ float* x, _Out_ float* y) const;

    void UpdateKeyboard(_In_ InputState* inputState);
    void ClearKeyboardState();
    void UpdateController();

private:
    KeyboardState _keyboardState;
    GamePadState _gamepadState;
};

void InputStartup();
void InputShutdown();

Input& GetInput();
